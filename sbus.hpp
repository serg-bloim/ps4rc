#ifndef SBUS_H
#define SBUS_H
#include <Arduino.h>
#include "utils.hpp"
#include "SoftwareSerial.h"
#include "ByteBuffer.hpp"

using SbusByteBuffer = StaticByteBuffer<25>;

class Sbus {
private:
  SoftwareSerial &serial;
  unsigned int time_frame;
  unsigned long time_frame_next;
  uint16_t channels[16];

protected:
public:
  Sbus(SoftwareSerial &serial, int time_frame = 14)
    : serial(serial),
      time_frame(time_frame) {
    this->time_frame_next =  millis() + time_frame;
    for(int i = 0; i < arraySize(this->channels); i++){
      this->channels[i] = 0;
    }
  }
  void begin(size_t pin) {
    serial.enableRxGPIOPullUp(true);
    serial.begin(100000, EspSoftwareSerial::Config::SWSERIAL_8E2, pin, pin, false, 256);
    serial.enableIntTx(false);
    serial.enableTx(true);
  }
  void update() {
    auto now = millis();
    if (now > time_frame_next) {
      do_send();
      time_frame_next += time_frame;
    }
  }
  size_t do_send() {
    SbusByteBuffer buff;
    uint8_t header = 0x0F;
    uint8_t footer = 0x00;
    uint8_t opts = 0x03;
    buff.write(header);
    uint32_t ch_buffer = 0;
    uint8_t shift = 0;
    for(int i = 0; i < arraySize(this->channels); i++){
      uint32_t ch_val = this->channels[i] & 0x7FF;
      ch_val <<= shift;
      shift += 11;
      ch_buffer |= ch_val;
      while(shift >= 8){
        uint8_t to_write = ch_buffer;
        buff.write(to_write);
        ch_buffer >>= 8;
        shift -= 8;
      }
    }
    buff.write(opts);
    buff.write(footer);
    size_t sz = 0;
    sz = serial.write(buff.c_str(), buff.len());
    return sz;
  }
  void set_channel(uint8_t ch, int32_t v) {
    const int32_t MIN_RANGE = -0xFFFF;
    const int32_t MAX_RANGE = +0xFFFF;
    uint16_t sbus_v = map(cap(v, MIN_RANGE, MAX_RANGE), MIN_RANGE, MAX_RANGE, 0, 2047)
    assert(ch < arraySize(this->channels));
    this->channels[ch] = sbus_v;
  }
};

#endif  // SBUS_H
