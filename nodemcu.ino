// NodeMCU (ESP8266)
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL68nHCsnZA" 
#define BLYNK_TEMPLATE_NAME "Toprak Sulama Sistemi" 
#define BLYNK_AUTH_TOKEN "QzPlUb5hyNvY8HmBUET8_7oo26wZJN9D"

// --- INFLUXDB AYARLARI ---
#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com" // Kendi URL'ini yaz
#define INFLUXDB_TOKEN "ggUaAn6II_5MPhYcKS3N7RPaPVBmzhtTyvuQBEDHFtyih6C3mnDtZhBsaBUd4cklzIjm_ngSupWA9jlGoAMAzg=="
#define INFLUXDB_ORG "e.ranaakyol@gmail.com"
#define INFLUXDB_BUCKET "Toprak_Verileri"
#define TZ_INFO "UTC3" // Türkiye için saat dilimi

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <BlynkSimpleEsp8266.h>

#define rolePin   D1      // GPIO5
#define nemSensor A0      // ESP8266 tek analog pin
#define suSensor  D2

char ssid[] = "rana";
char pass[] = "fazlasomurme";

int tersnem = 0;
int suDurumuYuzde = 0; // Blynk Gauge için 0 veya 100
bool suUyarildi   = false;
bool pompaManuel  = true;

int sinirDeger = 200;

BLYNK_WRITE(V2) {
  int secim = param.asInt(); // Menüden gelen indeks (0, 1, 2...)

  switch (secim) {
    case 0: // Kaktüs
      sinirDeger = 50;
      break;
    case 1: // maydanoz
      sinirDeger = 500;
      break;
    case 2: // Orkide
      sinirDeger = 190;
      break;
    default:
      sinirDeger = 200;
      break;
  }

  
  Serial.print("Yeni Nem Eşiği Atandı: %");
  Serial.println(sinirDeger);
}
BlynkTimer timer;
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensorData("sulama_istatistikleri"); // Tablo adı

void veriGonder() {
  // 1. Sensörü Oku ve Yüzdeye Çevir
  int nem = analogRead(nemSensor);
  tersnem = 1024 - nem;

  // MAP Fonksiyonu: 0-1023 arası değeri 0-100 arasına oranlar
  int nemYuzde = map(tersnem, 0, 1023, 0, 100);
  
  // Değerin 0-100 dışına taşmaması için sabitleyelim 
  nemYuzde = constrain(nemYuzde, 0, 100);
  if (digitalRead(suSensor) == LOW) { 
    suDurumuYuzde = 100; // Su VAR
  } else {
    suDurumuYuzde = 0;   // Su YOK
  }

  // 2. Terminale Yazdır
  Serial.print("NEM : ");
  Serial.println(tersnem);
  Serial.print("SU: ");
  Serial.println(suDurumuYuzde);

  // 3. Blynk'e Gönder (V0 artık 0-100 arası gider)
  Blynk.virtualWrite(V0, nemYuzde); 
  // Su sensörü için de benzer bir map yapabilirsin (su değişkeni güncelleniyorsa)
  Blynk.virtualWrite(V1, suDurumuYuzde);

  // 4. InfluxDB'ye Gönder
  sensorData.clearFields();
  sensorData.addField("toprak_nemi_yuzde", nemYuzde); // Yüzde olarak kaydetmek daha mantıklı
  sensorData.addField("su_seviyesi", suDurumuYuzde);
  sensorData.addField("pompa_durumu", (digitalRead(rolePin) == HIGH) ? 1 : 0);

  if (!client.writePoint(sensorData)) {
    Serial.print("InfluxDB Hatası: ");
    Serial.println(client.getLastErrorMessage());
  } else {
    Serial.println("Veri InfluxDB'ye başarıyla kaydedildi.");
  }

  // 5. Otomatik Sulama Kontrolü (Burada yapmak daha sağlıklı)
  if (tersnem <= sinirDeger) {
    digitalWrite(rolePin, LOW);   // Nem düşükse (kurusa) sulamayı Kapat? 
    // Not: Tersnem mantığına göre sinirDeger altı "KURU" demekse burası HIGH olmalı.
  } else {
    digitalWrite(rolePin, HIGH); 
  }
}

void setup() {
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(rolePin, OUTPUT);
  digitalWrite(rolePin, HIGH); // Röle kapalı

  // InfluxDB Bağlantısını Başlat
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  if (client.validateConnection()) {
    Serial.println("InfluxDB Bağlantısı Başarılı");
  }// ÖNEMLİ: Zamanlayıcıyı kuruyoruz (Örn: 5 saniyede bir veri gönder)
  timer.setInterval(1000L, veriGonder);
}

void loop() {
  Blynk.run();
  timer.run();

  
}
