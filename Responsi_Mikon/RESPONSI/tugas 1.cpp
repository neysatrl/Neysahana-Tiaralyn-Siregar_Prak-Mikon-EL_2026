#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

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
Serial.println(tekanan);
Serial.print(" C | PWM LED: "); Serial.println(kecerahan);
delay(200);
}