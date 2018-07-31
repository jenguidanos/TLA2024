#include <Wire.h>
#pragma once
class TLA2024 {
 public:
  TLA2024();

  void begin();

  int read();

  void write(uint16_t data);

  void end();

 private:
  uint8_t addr = 0x48;
  uint8_t conv_reg = 0x00;
  uint8_t conf_reg = 0x01;
  uint16_t init_conf = 0;

  union I2C_data {
    uint8_t packet[2];
    uint16_t value;
  } data;
};

TLA2024::TLA2024() {}

void TLA2024::begin() { Wire.begin(); }

int TLA2024::read() {
  Wire.beginTransmission(addr);
  Wire.write(conv_reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, 2);
  if (2 <= Wire.available()) {
    // bring in data
    data.packet[1] = Wire.read();
    data.packet[0] = Wire.read();
    // shiftout unused data
    data.value >>= 4;
    // get sign and mask accordingly
    if (data.value & (1 << 11)) {
      data.value |= 0xF000;
    } else {
      data.value &= ~0xF000;
    }
    int16_t ret = data.value;
    return (int)ret;
  }
  return -1;
}

void TLA2024::write(uint16_t data) {
  Wire.beginTransmission(addr);
  Wire.write(conf_reg);
  Wire.write(data);
  Wire.endTransmission();
}
