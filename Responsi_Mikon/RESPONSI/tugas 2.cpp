#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

const char* ssid = "IphoNe";
const char* password = "Neysahana123";
String apiKey = "Z73IG1JG6LVARMY4";

const unsigned long intervalThingspeak = 20000;
unsigned long lastSend = 0;

const int LED_PIN = 18;
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  bme.begin(0x76);

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(LED_PIN, ledChannel);

  Serial.print("Koneksi WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
}

void loop() {
for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    ledcWrite(ledChannel, dutyCycle);
    delay(15);
}
float suhu = bme.readTemperature();
float kelembapan = bme.readHumidity();
float tekanan = bme.readPressure() / 100.0F;

int kecerahan = map((int)suhu, 25, 35, 0, 255);
kecerahan = constrain(kecerahan, 0, 255);

if (suhu > 30.0) {
  ledcWrite(ledChannel, 255);
} else {
  ledcWrite(ledChannel, 0);
}

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" C | Kelembapan: ");
  Serial.print(kelembapan);
  Serial.print(" % | Tekanan: ");
  Serial.print(tekanan);

  if (millis() - lastSend >= intervalThingspeak) {
    lastSend = millis();

    HTTPClient http;
    String url = "http://api.thingspeak.com/update?api_key=" + apiKey +
                 "&field1=" + String(suhu) +
                 "&field2=" + String(kelembapan) +
                 "&field3=" + String(tekanan);
                 
    http.begin(url);
    int httpCode = http.GET();
    String respon = http.getString();
    http.end();

    if (httpCode == 200 && respon != "0") {
      Serial.println("Data terkirim ke ThingSpeak, entry ke-" + respon);
    } else {
      Serial.println("Gagal kirim ke ThingSpeak (kode " + String(httpCode) + ")");
    }
  }

  delay(500);
}