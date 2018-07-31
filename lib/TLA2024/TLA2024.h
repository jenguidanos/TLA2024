#include <Wire.h>
#pragma once
class TLA2024 {
 public:
  TLA2024();

  int begin();

  uint16_t read(uint8_t mem_addr);

  int analogRead();

  void write(uint16_t data);

  void end();

 private:
  uint8_t addr = 0x48;
  uint8_t conv_reg = 0x00;
  uint8_t conf_reg = 0x01;
  // this is default conf.
  uint16_t init_conf = 0x02C3;

  union I2C_data {
    uint8_t packet[2];
    uint16_t value;
  } data;
};

TLA2024::TLA2024() {}

int TLA2024::begin() {
  Wire.begin();
  if (read(conf_reg) == init_conf) {
    return 0;
  } else {
    return -1;
  }
}

uint16_t TLA2024::read(uint8_t mem_addr) {
  Wire.beginTransmission(addr);
  Wire.write(mem_addr);
  Wire.endTransmission();
  Wire.requestFrom(addr, 2);
  if (2 <= Wire.available()) {
    // bring in data
    data.packet[1] = Wire.read();
    data.packet[0] = Wire.read();
    uint16_t ret = data.value;
    return ret;
  }
  return -1;
}

int TLA2024::analogRead() {
  // write 1 to OS bit to start conv
  uint16_t current_conf = read(conf_reg);
  current_conf |= 1 << 15;

  // OS bit will be 0 until conv is done.
  while (read(conf_reg) & 0x8000 == 0) {
    delay(20);
  }

  // get data from conv_reg
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
