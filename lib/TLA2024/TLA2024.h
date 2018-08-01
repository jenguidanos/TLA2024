#include <Arduino.h>
#include <Wire.h>
#pragma once
class TLA2024 {
 public:
  TLA2024();

  int begin();

  float analogRead();

 private:
  uint8_t addr = 0x48;
  uint8_t conv_reg = 0x00;
  uint8_t conf_reg = 0x01;
  // this is default conf.
  uint16_t init_conf = 0x8583;

  union I2C_data {
    uint8_t packet[2];
    uint16_t value;
  } data;
  uint16_t read(uint8_t mem_addr);
  int write(uint16_t data);
};

TLA2024::TLA2024() {}

int TLA2024::begin() {
  Wire.begin();
  uint16_t init = read(conf_reg);
  // make sure communication with device is working and that it is OK
  if (init == init_conf) {
    // make adc measure between AIN0 and GND
    init |= 0b100 << 12;
    write(init);
    return 0;
  } else {
    return 1;
  }
}

/*
  A generic read from mem_addr.
*/
uint16_t TLA2024::read(uint8_t mem_addr) {
  Wire.beginTransmission(addr);
  Wire.write(mem_addr);
  Wire.endTransmission();
  delay(10);
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

/*
  We only write to the configuration register.
  out_data is the 16 bits of conf_regs

  Should always return 2
*/
int TLA2024::write(uint16_t out_data) {
  int written = 0;
  // put our out_data into the I2C data union so we can send MSB and LSB
  data.value = out_data;
  Wire.beginTransmission(addr);
  Wire.write(conf_reg);
  written += Wire.write(data.packet[1]);
  written += Wire.write(data.packet[0]);
  Wire.endTransmission();
  return written;
}

float TLA2024::analogRead() {
  // write 1 to OS bit to start conv
  uint16_t current_conf = read(conf_reg);
  current_conf |= 0x8000;
  write(current_conf);
  delay(10);
  // OS bit will be 0 until conv is done.
  do {
    delay(10);
  } while ((read(conf_reg) & 0x8000) == 0);

  // get data from conv_reg
  uint16_t in_data = read(conv_reg);

  // shift out unused bits
  in_data >>= 4;

  // get sign and mask accordingly
  if (in_data & (1 << 11)) {
    // 11th bit is sign bit. if its set, set bits 15-12
    in_data |= 0xF000;
  } else {
    // not set, clear bits 15-12
    in_data &= ~0xF000;
  }

  // now store it as a signed 16 bit int.
  int16_t ret = in_data;

  // default Full Scale Range is -2.048V to 2.047V.
  // our 12bit 2's complement goes from -2048 to 2047 :)
  return in_data / 1000.0;
}
