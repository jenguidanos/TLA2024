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
  adc.begin();
  adc.setFSR(60);
  adc.setMux(4);
  adc.setDR(1);
  adc.setMode(false);
}

void loop() {
  // digitalWrite(SNSR_POWER, LOW);
  // delay(80);
  // digitalWrite(SNSR_POWER, HIGH);
  // delay(5);
  // adc.restore();
  // delay(12);
  float val = adc.analogRead();
  Serial.println(val);
  // delay();
}