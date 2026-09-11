#include <Arduino.h>

const int potPin    = 32;      // Pin analog (ADC) membaca wiper potensiometer
const float VREF    = 3.3;     // Tegangan referensi ESP32 (3.3V)
const int ADC_MAX   = 4095;    // Resolusi ADC 12-bit (0 - 4095)
const float R_TOTAL = 10000.0; // Total resistansi potensiometer (10k Ohm)

void setup() {
  Serial.begin(115200);        // Inisialisasi Serial Monitor
}

void loop() {
  int rawADC = analogRead(potPin);                     // Baca nilai mentah ADC (0-4095)

  float tegangan = (rawADC / (float)ADC_MAX) * VREF;   // Konversi ke Volt
  float hambatan = (tegangan / VREF) * R_TOTAL;        // Konversi ke Ohm

  // Menampilkan hasil pembacaan ke Serial Monitor
  Serial.print("Raw ADC: ");
  Serial.print(rawADC);
  Serial.print("\tTegangan: ");
  Serial.print(tegangan, 3);
  Serial.print(" V\tHambatan: ");
  Serial.print(hambatan, 1);
  Serial.println(" Ohm");

  delay(500); // Jeda pembacaan tiap 0.5 detik
}