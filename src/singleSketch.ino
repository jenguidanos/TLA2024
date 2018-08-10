#ifdef SINGLE
#include <Arduino.h>
#include <Wire.h>

#define SNSR_POWER 7
#define nI2C_ENABLE 15

uint8_t addr = 0x48;
uint8_t conv_reg = 0x00;
uint8_t conf_reg = 0x01;

union I2C_data {
  uint8_t packet[2];
  uint16_t value;
} data;

void setup() {
  Serial.begin(115200);
  adcInit();
}

void loop() {
  Serial.println(adcAnalogRead());
  delay(20);
}

void adcInit() {
  // enable external power
  pinMode(SNSR_POWER, OUTPUT);
  digitalWrite(SNSR_POWER, HIGH);
  // enable external I2C
  pinMode(nI2C_ENABLE, OUTPUT);
  digitalWrite(nI2C_ENABLE, LOW);
  delay(10);
  Wire.begin();
  Wire.setClock(100000);
  uint16_t conf = adcRead(conf_reg);
  // set inputs to ADC (P = 0, N = GND)
  conf &= ~0x7000;
  conf |= 0x4000;
  // set ADC FSR
  conf &= ~0x0E00;
  // send new conf
  adcWrite(conf);
}

/*
  We only write to the configuration register.
  out_data is the 16 bits of conf_regs

  Should always return 2
*/
int adcWrite(uint16_t out_data) {
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

/*
  A generic read from mem_addr.
*/
uint16_t adcRead(uint8_t mem_addr) {
  Wire.beginTransmission(addr);
  Wire.write(mem_addr);
  byte error = Wire.endTransmission();
  // Serial.println("Ended Transmission");
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

float adcAnalogRead() {
  // write 1 to OS bit to start conv
  uint16_t current_conf = adcRead(conf_reg);
  current_conf |= 0x8000;
  adcWrite(current_conf);
  // OS bit will be 0 until conv is done.
  do {
    delay(5);
  } while ((adcRead(conf_reg) & 0x8000) == 0);

  // get data from conv_reg
  uint16_t in_data = adcRead(conv_reg);

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

  // return raw adc data
  return ret;
}
#endif  // SINGLE