#include <Arduino.h>
#include <Wire.h>
#pragma once
#define SINGLE true
#define CONT false
class TLA2024 {
 public:
  TLA2024();

  /*
    Initializes I2C bus
    returns:
       0 - adc responds with correct default conf
       1 - otherwise
  */
  int begin();

  // resets device to default configuration
  void reset();

  // restores device to last config
  void restore();

  float analogRead();

  /*
    Sets the Full Scale Range of the ADC

    gain:
      60 -> ± 6.144v
      40 -> ± 4.096
      20 -> ± 2.048 (default)
      10 -> ± 1.024
      5  -> ± 0.512
      2  -> ± 0.256
  */
  void setFSR(uint8_t gain);

  /*
    Configures the input signals to the ADC

    option:
      1 -> P = 0, N = 1 (TP1 - TP2)
      2 -> P = 0, N = 3 (TP1 - VGND)
      3 -> P = 1, N = 3 (TP2 - VGND)
      4 -> P = 0, N = GND
  */
  void setMux(uint8_t option);

  /*
    Continous conversion (0) or single shot (1)
  */
  void setMode(bool mode);

  void setDR(uint8_t rate);

 private:
  uint8_t addr = 0x48;
  uint8_t conv_reg = 0x00;
  uint8_t conf_reg = 0x01;

  // this is default conf.
  uint16_t init_conf = 0x8583;
  uint16_t saved_conf = 0x8583;
  bool mode = SINGLE;

  union I2C_data {
    uint8_t packet[2];
    uint16_t value;
  } data;

  /*
    A generic read from mem_addr.
  */
  uint16_t read(uint8_t mem_addr);

  /*
    We only write to the configuration register.
    out_data is the 16 bits of conf_regs

    Should always return 2

    Saves data to current_conf
  */
  int write(uint16_t data);
};

