#include "TLA2024.h"


TLA2024::TLA2024() {}

int TLA2024::begin() {
  Serial.println("Begin");
  Wire.begin();
  Wire.setClock(100000);
  Serial.println("Done Wire");
  uint16_t init = read(conf_reg);
  Serial.println("Done Read");
  // make sure communication with device is working and that it is OK
  if ((init == init_conf) || 1) {
    return 0;
  } else {
    return 1;
  }
}

uint16_t TLA2024::read(uint8_t mem_addr) {
  Wire.beginTransmission(addr);
  Wire.write(mem_addr);
  Wire.endTransmission();
  // Serial.println("Ended Transmission");
  delay(5);
  Wire.requestFrom(addr, 2);
  if (2 <= Wire.available()) {
    // bring in data
    data.packet[1] = Wire.read();
    data.packet[0] = Wire.read();
    uint16_t ret = data.value;
    data.value = 0;
    return ret;
  }
  return -1;
}

int TLA2024::write(uint16_t out_data) {
  int written = 0;
  // save conf
  saved_conf = out_data;
  // put our out_data into the I2C data union so we can send MSB and LSB
  data.value = out_data;
  Wire.beginTransmission(addr);
  Wire.write(conf_reg);
  written += Wire.write(data.packet[1]);
  written += Wire.write(data.packet[0]);
  Wire.endTransmission();
  data.value = 0;
  return written;
}

void TLA2024::reset(void) { write(init_conf); }

void TLA2024::restore(void) {
  uint16_t restore_conf = saved_conf & ~0x8000;
  write(restore_conf);
  // Serial.println(restore_conf, BIN);
}

float TLA2024::analogRead() {
  // this only needs to run when in single shot.
  if (mode) {
    // write 1 to OS bit to start conv
    uint16_t current_conf = read(conf_reg);
    current_conf |= 0x8000;
    write(current_conf);
    // OS bit will be 0 until conv is done.
    do {
      delay(5);
    } while ((read(conf_reg) & 0x8000) == 0);
  }

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
  // return ret /1000.0;

  // return raw adc data
  return ret;
}

void TLA2024::setFSR(uint8_t gain) {
  // bring in conf reg
  uint16_t conf = read(conf_reg);
  // clear the PGA bits:
  conf &= ~0x0E00;

  switch (gain) {
    case 60:
      // PGA bits already cleared
      break;

    case 40:
      // set bit 9
      conf |= 0x0200;
      break;

    case 20:
      // set bit 10
      conf |= 0x0400;
      break;

    case 10:
      // set bit 10-9
      conf |= 0x0600;
      break;

    case 5:
      // set bit 11
      conf |= 0x0800;
      break;

    case 2:
      // set bit 11-9
      conf |= 0x0E00;
      break;
  }
  write(conf);
}

void TLA2024::setMux(uint8_t option) {
  // bring in conf reg
  uint16_t conf = read(conf_reg);
  // clear MUX bits
  conf &= ~0x7000;

  switch (option) {
    case 1:
      // bits already cleared
      break;

    case 2:
      // set bit 12
      conf |= 0x1000;
      break;

    case 3:
      // set bit 13
      conf |= 0x2000;
      break;

    case 4:
      // set bit 14
      conf |= 0x4000;
  }
  write(conf);
}

void TLA2024::setMode(bool mode) {
  // bring in conf reg
  TLA2024::mode = mode;
  uint16_t conf = read(conf_reg);
  // clear MODE bit (8) (continous conv)
  conf &= ~(1 << 8);
  if (mode) {
    // single shot
    conf |= (1 << 8);
  }
  write(conf);
}

void TLA2024::setDR(uint8_t rate) {
  // bring in conf reg
  uint16_t conf = read(conf_reg);
  // set bits 7:5
  conf |= 0b111 << 5;
  write(conf);
}