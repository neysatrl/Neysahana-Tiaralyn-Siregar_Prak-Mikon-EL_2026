#include <Arduino.h>
#include <WiFi.h>
#include <CTBot.h>

CTBot myBot;

String ssid   = "IphoNe";
String pass   = "Neysahana123";
String token  = "9134731906:AAFm_n2wBLRvnDTWyMqJMHyWEMdcb8675B4";

const int servoPin = 16;
const int servoChannel = 0;
const int servoFreq = 50;
const int servoResolution = 12;

void setServo(int angle) {
  angle = constrain(angle, 0, 180);
  int duty = map(angle, 0, 180, 102, 512);
  ledcWrite(servoChannel, duty);
}

void setup() {
  Serial.begin(115200);
  ledcSetup(servoChannel, servoFreq, servoResolution);
  ledcAttachPin(servoPin, servoChannel);
  setServo(90);

  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);

  if (myBot.testConnection())
    Serial.println("Telegram OK");
  else
    Serial.println("Telegram GAGAL");
}

void loop() {
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
    } else {
      int angle = msg.text.toInt();
      if (angle >= 0 && angle <= 180) {
        setServo(angle);
        String reply = "Servo ke " + String(angle) + " derajat";
        myBot.sendMessage(msg.sender.id, reply);
      } else {
        myBot.sendMessage(msg.sender.id, "Kirim MIN, MID, MAX, atau angka 0-180");
      }
    }
  }
  delay(500);
}