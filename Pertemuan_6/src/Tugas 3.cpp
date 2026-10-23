#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

const char* ssid = "tum";
const char* password = "nadinerev";

WebServer server(80);
Servo myServo;
Adafruit_MPU6050 mpu; // Pin SDA ke GPIO 21, SCL ke GPIO 22

const int servoPin = 18; // Pin Signal Servo ke GPIO 18
const int buzzerPin = 23; // Pin Positif Buzzer ke GPIO 23

int currentServoPos = 90;
float accYTerakhir = 0.0;
float sudutTerakhir = 0.0;
String statusTerakhir = "";
String buzzerTerakhir = "OFF";

unsigned long waktuSebelumnya = 0;
const long interval = 2000; // (1) selang waktu 2 detik, ditulis dalam milidetik

bool isNumeric(String str) {
  if (str.length() == 0) return false;
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) return false;
  }
  return true;
}

float bacaKemiringan(float &nilaiAccelY) {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  nilaiAccelY = a.acceleration.y;
  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  return abs(roll);
}

void cekKondisiMPU() {
  sudutTerakhir = bacaKemiringan(accYTerakhir);
  String statusBaru = "";

  if (sudutTerakhir > 35.0) {
    statusBaru = "BAHAYA";
    buzzerTerakhir = "ON (MENYALA)";
    digitalWrite(buzzerPin, HIGH); // (2) nilai agar buzzer menyala
    currentServoPos = 180; // Servo ke 180 derajat saat BAHAYA
    myServo.write(currentServoPos);
  }
  else if (sudutTerakhir >= 15.0) {
    statusBaru = "SEDANG";
    buzzerTerakhir = "OFF";
    digitalWrite(buzzerPin, LOW); // Buzzer mati
    // Jika baru saja pindah ke kondisi SEDANG, kembalikan servo ke 90 derajat
    if (statusTerakhir != "SEDANG") { // (3) servo hanya digerakkan jika status sebelumnya BUKAN "SEDANG"
      currentServoPos = 90;
      myServo.write(currentServoPos);
    }
  }
  else {
    statusBaru = "DATAR";
    buzzerTerakhir = "OFF";
    digitalWrite(buzzerPin, LOW); // Buzzer mati
    // Jika baru saja pindah ke kondisi DATAR, kembalikan servo ke 0 derajat
    if (statusTerakhir != "DATAR") {
      currentServoPos = 0;
      myServo.write(currentServoPos);
    }
  }

  statusTerakhir = statusBaru;

  // Tampilkan di Serial Monitor
  Serial.print("Accel Y: ");
  Serial.print(accYTerakhir, 2);
  Serial.print(" m/s2 | Sudut: ");
  Serial.print(sudutTerakhir, 1);
  Serial.print("° | Kondisi: ");
  Serial.print(statusTerakhir);
  Serial.print(" | Buzzer: ");
  Serial.print(buzzerTerakhir);
  Serial.print(" | Servo: ");
  Serial.print(currentServoPos);
  Serial.println("°");
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tugas 3 - MPU6050 + Servo + Buzzer</title>
<style>
body { font-family: Arial, sans-serif; text-align: center; margin-top: 30px; }
.badge { padding: 12px 25px; color: white; font-weight: bold; border-radius: 8px; display:
inline-block; font-size: 20px; }
.DATAR { background-color: #6c757d; }
.SEDANG { background-color: #28a745; }
.BAHAYA { background-color: #dc3545; }

.card { border: 1px solid #ccc; padding: 20px; margin: 15px auto; width: 340px; border-
radius: 10px; }

input[type=range] { width: 90%; }
</style>
</head>
<body>
<h2>Sistem Otomatis Kemiringan (MPU6050, Servo & Buzzer)</h2>
<div class="card">
<h3>Status Sensor MPU6050</h3>
<p>Kemiringan: <span id="sudut">--</span> &deg; (Accel Y: <span id="accY">--</span>)</p>
<div id="statusBox" class="badge SEDANG">MEMUAT...</div>
<p>Buzzer: <b id="buzzerStatus">OFF</b></p>
</div>
<div class="card">
<h3>Kendali Servo</h3>
<p>Sudut Servo: <span id="posVal">90</span>&deg;</p>
<input type="range" min="0" max="180" value="90" id="servoSlider"
oninput="updateServo(this.value)">
<p id="statusMsg"></p>
</div>
<script>
function updateServo(val) {
document.getElementById("posVal").innerText = val;
fetch("/servo?pos=" + val)
.then(res => res.text().then(text => ({ status: res.status, body: text })))
.then(res => {
document.getElementById("statusMsg").innerText = `Status ${res.status}: ${res.body}
`;
});
}
function ambilData() {
fetch("/data")
.then(res => res.json())
.then(data => {
document.getElementById("sudut").innerText = data.sudut.toFixed(1);
document.getElementById("accY").innerText = data.accY.toFixed(2);
let box = document.getElementById("statusBox");
box.innerText = data.status;
box.className = "badge " + data.status;
document.getElementById("buzzerStatus").innerText = data.buzzer;
document.getElementById("posVal").innerText = data.servo;
document.getElementById("servoSlider").value = data.servo;
});
}
setInterval(ambilData, 2000);
ambilData();
</script>
</body>
</html>
)rawliteral";

void handleServo() {
  if (!server.hasArg("pos") || !isNumeric(server.arg("pos"))) {
    server.send(400, "text/plain", "Input tidak valid (harus angka 0-180)!");
    return;
  }
  int pos = server.arg("pos").toInt();
  if (pos < 0 || pos > 180) {
    server.send(400, "text/plain", "Sudut di luar batas (0-180)!");
    return;
  }
  currentServoPos = pos;
  myServo.write(currentServoPos);
  Serial.println("[WEB] Servo diubah manual ke: " + String(currentServoPos) + "°");
  server.send(200, "text/plain", "Servo diatur ke " + String(pos) + " derajat");
}

void handleData() {
  String json = "{\"sudut\":" + String(sudutTerakhir) +
  ",\"accY\":" + String(accYTerakhir) +
  ",\"status\":\"" + statusTerakhir + "\"" +
  ",\"buzzer\":\"" + buzzerTerakhir + "\"" +
  ",\"servo\":" + String(currentServoPos) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT); // (4) mode pin buzzer sebagai keluaran
  digitalWrite(buzzerPin, LOW);
  myServo.attach(servoPin);
  myServo.write(currentServoPos);
  
  if (!mpu.begin()) {
    Serial.println("Gagal menemukan sensor MPU6050! Cek kabel SDA(21) & SCL(22).");
    while (1) { delay(10); }
  }
  Serial.println("MPU6050 Siap!");
  
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung! IP Address: " + WiFi.localIP().toString());
  
  cekKondisiMPU(); // Baca kondisi awal
  
  server.on("/", []() { server.send(200, "text/html", index_html); });
  server.on("/servo", handleServo); // (5) path yang dipanggil slider servo
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long waktuSekarang = millis();
  if (waktuSekarang - waktuSebelumnya >= interval) {
    waktuSebelumnya = waktuSekarang;
    cekKondisiMPU(); // (6) panggil fungsi pengecekan kondisi sensor
  }
}