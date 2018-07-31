#include <Arduino.h>
#include <TLA2024.h>
#include <Wire.h>

TLA2024 adc = TLA2024();

void setup() {
  adc.begin();
  Serial.begin(115200);
}

void loop() {
  Serial.println(adc.read());
  delay(100);
}