#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>

const char* ssid = "tum";
const char* password = "nadinerev";
const char* mqtt_server = "2d5cd8d3bcbd425a8865b60469b32931.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "Praktikan25";
const char* mqtt_pass = "Mikrokontroler2026";

const char* topic_cmd = "ugm/mikon/kelompok12/cmd"; // SESUAIKAN KELOMPOK
const char* topic_status = "ugm/mikon/kelompok12/status";
const char* topic_ax = "ugm/mikon/kelompok12/ax";

const int pinSDA = 21; const int pinSCL = 22; // ESP32-S3: 8 // ESP32-S3: 9

const int pinAlarm = 18;
float axMax = 5.0; // batas percepatan sumbu X (m/s^2), sekitar 30 derajat
bool axAlarm = false;

Adafruit_MPU6050 mpu;
unsigned long interval = 1000; unsigned long lastPub = 0; // periode kirim data (ms)

WiFiClientSecure espClient;
PubSubClient client(espClient);

void kirimData() {
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  char buf[16];
  dtostrf(a.acceleration.x, 1, 2, buf); 
  client.publish(topic_ax, buf); // satuan m/s^2

  float ax = a.acceleration.x;
  if (!axAlarm && fabs(ax) > axMax) {
    axAlarm = true;
    digitalWrite(pinAlarm, HIGH);
    client.publish(topic_status, "ALERT: KEMIRINGAN BERLEBIH");
  } else if (axAlarm && fabs(ax) < axMax - 1.0) { // selisih 1 agar buzzer tidak berkedip
    axAlarm = false;
    digitalWrite(pinAlarm, LOW);
    client.publish(topic_status, "STATUS: NORMAL");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char message[32];
  if (length >= sizeof(message)) length = sizeof(message) - 1;
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.print("Pesan masuk: ");
  Serial.println(message);

  if (strncmp(message, "AXMAX:", 6) == 0) {
    float v = atof(message + 6);
    if (v > 1 && v < 9) axMax = v;
    return;
  }

  long v = atol(message);
  if (v >= 200 && v <= 60000) {
    interval = v;
  }
}

void setup_wifi() {
  Serial.print("Koneksi WiFi... ");
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
    Serial.print("Koneksi MQTT... ");
    String clientId = "ESP32-IMU-Kel-" + String(random(1000, 9999));
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
  pinMode(pinAlarm, OUTPUT);
  digitalWrite(pinAlarm, LOW);

  Wire.begin(pinSDA, pinSCL);
  if (!mpu.begin(0x68)) {
    Serial.println("MPU6050 tidak terdeteksi!");
    while (1) delay(1000);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  if (millis() - lastPub >= interval) {
    lastPub = millis();
    kirimData();
  }
}