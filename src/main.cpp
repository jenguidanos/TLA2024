#include <Arduino.h>
#include <TLA2024.h>
#include <Wire.h>

TLA2024 adc = TLA2024();

void setup() {
  Serial.begin(115200);
  if (adc.begin()) {
    Serial.println("ADC Init Error");
    while (true) {
    }
  } else {
    Serial.println("ADC Init OK");
    adc.setFSR(5);
  }
}

void loop() {
  float val = adc.analogRead();
  Serial.println(val);
  delay(10);
}