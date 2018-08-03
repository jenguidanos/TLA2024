#include <Arduino.h>
#include <TLA2024.h>
#include <Wire.h>

#define SNSR_POWER 7
#define nI2C_enable 15

TLA2024 adc = TLA2024();

void setup() {
  pinMode(SNSR_POWER, OUTPUT);
  digitalWrite(SNSR_POWER, HIGH);
  pinMode(nI2C_enable, OUTPUT);
  digitalWrite(nI2C_enable, LOW);

  Serial.begin(115200);
  if (adc.begin()) {
    Serial.println("ADC Init Error");
    // while (true) {
    // }
  } else {
    Serial.println("ADC Init OK");
    adc.setFSR(40);
  }
}

void loop() {
  float val = adc.analogRead();
  Serial.println(val);
  delay(10);
}