#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "XL Home_19AE";
const char* password = "enakandirumah4267";

const char* mqtt_server =
    "2d5cd8d3bcbd425a8865b60469b32931.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "Praktikan25";
const char* mqtt_pass = "Mikrokontroler2026";

const char* topic_cmd = "ugm/mikon/kelompok11/cmd";
const char* topic_status = "ugm/mikon/kelompok11/status";

const int pinLED = 18;

const int pwmChannel = 0;
const int pwmFreq = 5000;
const int pwmResolution = 8;

const int pinBtn = 21;
bool isEmergency = false;
int currentPWM = 0;

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  if (isEmergency) {
    Serial.println("Emergency Stop aktif!");
    return;
  }

  char message[32];
  if (length >= sizeof(message)) length = sizeof(message) - 1;
  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.print("Pesan masuk: ");
  Serial.println(message);

  if (strcmp(message, "ON") == 0) {
    currentPWM = 255;
    ledcWrite(pwmChannel, currentPWM);
  } else if (strcmp(message, "OFF") == 0) {
    currentPWM = 0;
    ledcWrite(pwmChannel, currentPWM);
  } else {
    int val = atoi(message);

    if (val >= 0 && val <= 255) {
      currentPWM = val;
      ledcWrite(pwmChannel, currentPWM);
    }
  }
}

void setup_wifi() {
  Serial.print("Koneksi WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Terhubung!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Koneksi MQTT...");
    String clientId = "ESP32-LED-Kel-" + String(random(1000, 9999));

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println(" Terhubung!");
      client.subscribe(topic_cmd);
      client.publish(topic_status, "Tugas 3: Ready");
    } else {
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(pinLED, pwmChannel);
  ledcWrite(pwmChannel, 0);

  pinMode(pinBtn, INPUT);

  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  int btn = digitalRead(pinBtn);
  static int lastBtn = LOW;

  if (btn != lastBtn) {
    lastBtn = btn;

    if (btn == HIGH) {
      isEmergency = true;
      ledcWrite(pwmChannel, 0);
      client.publish(topic_status, "ALERT: EMERGENCY STOP ON");
    } else {
      isEmergency = false;
      ledcWrite(pwmChannel, currentPWM);
      client.publish(topic_status, "STATUS: NORMAL");
    }
  }
}

