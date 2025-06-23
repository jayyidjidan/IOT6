#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Konfigurasi WiFi Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT Broker
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// DHT Config
#define DHTPIN 21   // Sesuai wokwi kamu
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  Serial.print("Menghubungkan ke WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Terhubung ke broker MQTT!");
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  if (!isnan(suhu) && !isnan(kelembapan)) {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print("°C | Kelembapan: ");
    Serial.print(kelembapan);
    Serial.println(" %");

    client.publish("sensor/suhu", String(suhu).c_str());
    client.publish("sensor/kelembapan", String(kelembapan).c_str());
  } else {
    Serial.println("Gagal membaca sensor");
  }

  delay(3000);
}
