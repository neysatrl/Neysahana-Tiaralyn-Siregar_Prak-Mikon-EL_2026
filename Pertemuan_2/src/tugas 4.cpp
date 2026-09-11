#include <Arduino.h>

// 1. Deklarasi Pin Menggunakan const
const int PIN_POT        = 32;
const int PIN_BUTTON     = 4;
const int PIN_LED_MERAH  = 18;
const int PIN_LED_KUNING = 19;
const int PIN_LED_HIJAU  = 21;

// 2. Deklarasi Konstanta ADC & Hambatan
const int ADC_MAX        = 4095;    // Resolusi ADC 12-bit ESP32 (0 - 4095)
const float VREF         = 3.3;     // Tegangan Referensi ESP32
const float R_TOTAL      = 10000.0; // Resistansi Total Potensiometer (10k Ohm)

// Pembagian 3 Wilayah Level Tegangan dari Nilai ADC (0 - 4095)
const int BATAS_RENDAH   = ADC_MAX / 3;       // 0 - 1365 (Level Rendah)
const int BATAS_SEDANG   = (ADC_MAX * 2) / 3; // 1366 - 2730 (Level Sedang)
                                              // 2731 - 4095 (Level Tinggi)

// Fungsi Pembantu untuk Mematikan Seluruh LED
void matikanSemuaLED() {
  digitalWrite(PIN_LED_MERAH, LOW);
  digitalWrite(PIN_LED_KUNING, LOW);
  digitalWrite(PIN_LED_HIJAU, LOW);
}

void setup() {
  // Inisialisasi Serial Monitor dengan Baudrate 115200
  Serial.begin(115200);

  // Konfigurasi Mode Pin Output
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_KUNING, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);

  // Konfigurasi Mode Pin Input Button dengan Internal Pull-Down
  // Default = LOW (0V), Saat ditekan = HIGH (3.3V)
  pinMode(PIN_BUTTON, INPUT_PULLDOWN);
}

void loop() {
  // Pembacaan Input Digital dan Analog
  int rawADC       = analogRead(PIN_POT);
  int statusTombol = digitalRead(PIN_BUTTON);

  // Konversi Nilai ADC ke Tegangan dan Hambatan
  float tegangan = (rawADC / (float)ADC_MAX) * VREF;
  float hambatan = (tegangan / VREF) * R_TOTAL;

  // Cek Logika Utamakan Tombol Darurat
  if (statusTombol == HIGH) {
    // KONDISI DARURAT: Tombol ditekan -> Seluruh LED WAJIB MATI
    matikanSemuaLED();
    Serial.println("[DARURAT] Push Button Ditekan! -> Seluruh LED MATI");
  } else {
    // KONDISI NORMAL: LED menyala berdasarkan level nilai ADC potensiometer
    if (rawADC < BATAS_RENDAH) {
      // Level Tegangan Rendah: Hanya LED Merah Menyala
      digitalWrite(PIN_LED_MERAH, HIGH);
      digitalWrite(PIN_LED_KUNING, LOW);
      digitalWrite(PIN_LED_HIJAU, LOW);
      Serial.print("[STATUS] Level RENDAH\t| ");
    } 
    else if (rawADC < BATAS_SEDANG) {
      // Level Tegangan Sedang: Hanya LED Kuning Menyala
      digitalWrite(PIN_LED_MERAH, LOW);
      digitalWrite(PIN_LED_KUNING, HIGH);
      digitalWrite(PIN_LED_HIJAU, LOW);
      Serial.print("[STATUS] Level SEDANG\t| ");
    } 
    else {
      // Level Tegangan Tinggi: Hanya LED Hijau Menyala
      digitalWrite(PIN_LED_MERAH, LOW);
      digitalWrite(PIN_LED_KUNING, LOW);
      digitalWrite(PIN_LED_HIJAU, HIGH);
      Serial.print("[STATUS] Level TINGGI\t| ");
    }

    // Menampilkan Informasi Pembacaan ke Serial Monitor
    Serial.print("Raw ADC: ");
    Serial.print(rawADC);
    Serial.print("\t| Tegangan: ");
    Serial.print(tegangan, 2);
    Serial.print(" V\t| Hambatan: ");
    Serial.print(hambatan, 0);
    Serial.println(" Ohm");
  }

  // Delay pembacaan 100 ms
  delay(100);
}