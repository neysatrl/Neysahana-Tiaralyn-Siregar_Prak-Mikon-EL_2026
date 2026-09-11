#include <Arduino.h>

#define LED_PIN 2

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);

}

void loop() {
digitalWrite(LED_PIN, HIGH);
delay(3000); 
digitalWrite(LED_PIN, LOW);
delay(3000);
}