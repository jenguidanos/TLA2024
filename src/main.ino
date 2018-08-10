#include <Arduino.h>
#include <TLA2024.h>
#include <Wire.h>

#define SNSR_POWER 7
#define nI2C_ENABLE 15

TLA2024 adc = TLA2024();

void setup() {
  pinMode(SNSR_POWER, OUTPUT);
  digitalWrite(SNSR_POWER, HIGH);
  pinMode(nI2C_ENABLE, OUTPUT);
  digitalWrite(nI2C_ENABLE, LOW);
  delay(20);
  Serial.begin(115200);
  Serial.print("Starting");
  if (adc.begin()) {
    Serial.println("ADC Init Error");
    // while (true) {
    // }
  } else {
    Serial.println("ADC Init OK");
    adc.setFSR(60);
  }
}

void loop() {
  float val = adc.analogRead();
  Serial.println(val);
  delay(10);
}