Otomatik toprak sulama sistemi; NodeMCU, Arduino Uno ve Blynk kullanılarak geliştirilmiş IoT tabanlı bir projedir.

Kullanıcı mobil uygulama üzerinden bitki türünü seçer ve buna göre sistem uygun toprak nem eşik değerini otomatik olarak belirler. Toprak nem sensöründen alınan veriler analiz edilerek nem seviyesi düşük olduğunda röle aracılığıyla iki su pompası çalışır ve toprak hedeflenen nem seviyesine ulaşana kadar sulama yapılır. Su tankındaki seviye kritik düzeye düştüğünde ise uygulama üzerinden kullanıcıya bildirim gönderilir.

Ayrıca sistemde büyük veri (Big Data) analizi için InfluxDB kullanılmıştır. Sensörlerden elde edilen zaman serisi verileri InfluxDB üzerinde depolanarak geçmiş nem değerleri izlenebilir, analiz edilebilir ve sistem performansı optimize edilebilir.

Bu yapı sayesinde sistem hem otomatik hem de veri odaklı akıllı bir sulama çözümü sunar.
