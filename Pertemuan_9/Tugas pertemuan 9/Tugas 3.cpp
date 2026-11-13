#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <CTBot.h>

Adafruit_INA219 ina219;
CTBot myBot;

String ssid   = "IphoNe";
String pass   = "Neysahana123";
String token  = "9134731906:AAFm_n2wBLRvnDTWyMqJMHyWEMdcb8675B4";
String apiKey = "321I76D1Z64XJGLI";

const int servoPin = 16;
const int servoChannel = 0;
const int servoFreq = 50;
const int servoResolution = 12;

const unsigned long intervalThingspeak = 20000;
unsigned long lastSend = 0;

void setServo(int angle) {
  angle = constrain(angle, 0, 180);
  int duty = map(angle, 0, 180, 102, 512);
  ledcWrite(servoChannel, duty);
}

void kirimThingspeak(float arus_mA, float tegangan_V) {
  HTTPClient http;
  String url = "http://api.thingspeak.com/update?api_key=" + apiKey +
               "&field1=" + String(arus_mA) +
               "&field2=" + String(tegangan_V);

  http.begin(url);
  int httpCode = http.GET();
  String respon = http.getString();
  http.end();

  if (httpCode == 200 && respon != "0") {
    Serial.println(">> ThingSpeak terkirim, entry ke-" + respon);
  } else {
    Serial.println(">> ThingSpeak GAGAL (kode " + String(httpCode) + ")");
  }
}

void setup() {
  Serial.begin(115200);
  ledcSetup(servoChannel, servoFreq, servoResolution);
  ledcAttachPin(servoPin, servoChannel);
  setServo(90);

  Wire.begin();
  ina219.begin();

  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);
  Serial.println(myBot.testConnection() ? "Telegram OK" : "Telegram GAGAL");
}

void loop() {
  if (millis() - lastSend >= intervalThingspeak) {
    lastSend = millis();
    float arus_mA = ina219.getCurrent_mA();
    float tegangan_V = ina219.getBusVoltage_V();
    kirimThingspeak(arus_mA, tegangan_V);
  }

  TBMessage msg;
  if (myBot.getNewMessage(msg)) {
    Serial.print("Pesan masuk: ");
    Serial.println(msg.text);

    if (msg.text.equalsIgnoreCase("MIN")) {
      setServo(0);
      myBot.sendMessage(msg.sender.id, "Servo ke 0 derajat");
    } else if (msg.text.equalsIgnoreCase("MID")) {
      setServo(90);
      myBot.sendMessage(msg.sender.id, "Servo ke 90 derajat");
    } else if (msg.text.equalsIgnoreCase("MAX")) {
      setServo(180);
      myBot.sendMessage(msg.sender.id, "Servo ke 180 derajat");
    } else if (msg.text.equalsIgnoreCase("BACA ARUS")) {
      float arus_mA = ina219.getCurrent_mA();
      String reply = "Arus: " + String(arus_mA, 1) + " mA";
      myBot.sendMessage(msg.sender.id, reply);
    } else if (msg.text.equalsIgnoreCase("BACA TEGANGAN")) {
      float tegangan_V = ina219.getBusVoltage_V();
      String reply = "Tegangan: " + String(tegangan_V, 2) + " V";
      myBot.sendMessage(msg.sender.id, reply);
    } else {
      int angle = msg.text.toInt();
      if (angle >= 0 && angle <= 180) {
        setServo(angle);
        String reply = "Servo ke " + String(angle) + " derajat";
        myBot.sendMessage(msg.sender.id, reply);
      } else {
        String reply = "Perintah: MIN / MID / MAX / angka 0-180\nBACA ARUS / BACA TEGANGAN";
        myBot.sendMessage(msg.sender.id, reply);
      }
    }
  }
  delay(50);
}