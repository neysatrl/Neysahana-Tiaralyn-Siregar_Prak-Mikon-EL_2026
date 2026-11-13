#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
const char* ssid = "Iphone";
const char* password = "Neysahana123";
String apiKey = "321176D1Z64XJGLI"; // GANTI dengan API Key kalian
const unsigned long intervalThingspeak = 20000;
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  ina219.begin();
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
  if (millis() - lastSend >= intervalThingspeak) {
    lastSend = millis();
    float arus_mA = ina219.getCurrent_mA();
    float tegangan_V = ina219.getBusVoltage_V();
    Serial.print("Arus: ");
    Serial.print(arus_mA);
    Serial.print(" mA | Tegangan: ");
    Serial.println(tegangan_V);
    
    HTTPClient http;
    String url = "http://api.thingspeak.com/update?api_key=" + apiKey +
                 "&field1=" + String(arus_mA) +
                 "&field2=" + String(tegangan_V);
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
}
