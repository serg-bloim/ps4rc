#include "StaticConfig.h"
#include "RemoteReceiver.hpp"
#include <SoftwareSerial.h>
#include "utils.hpp"
#include "Blinker.hpp"
#include "Ps4Client.hpp"

SoftwareSerial serial;
RemoteReceiver srxl(serial, DEVICES_REMOTE_RECEIVER);
Blinker blk(2, 500);
Ps4Client ps4;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("setup 1");
  blk.begin();
  srxl.begin(SRXL_PIN);
  ps4.begin("a8:2b:b9:45:ac:89");
  Serial.println("Start");
  srxl.enable_channel(0, true);
  srxl.enable_channel(1, true);
  srxl.enable_channel(2, true);
  srxl.enable_channel(3, true);
  srxl.enable_channel(4, true);
  srxl.enable_channel(5, true);
  srxl.set_channel(4, 0xD540); // beginner
  // srxl.set_channel(4, 0x8000); // advanced
  // srxl.set_channel(4, 0x2AC0); // expert
  srxl.set_channel(5, 0xD540);
}

void loop() {
  blk.update();
  srxl.set_channel(0, convert_signed(ps4.throttle));
  srxl.set_channel(1, convert_signed(-ps4.roll));
  srxl.set_channel(2, convert_signed(ps4.pitch));
  srxl.set_channel(3, convert_signed(-ps4.yaw));
  srxl.update();
}

const int32_t MIN_RANGE = -0xFFFF;
const int32_t MAX_RANGE = +0xFFFF;
uint16_t convert_unsigned(int32_t v){
  return cap(v, 0, MAX_RANGE);
}

uint16_t convert_signed(int32_t v){
  return map(cap(v, MIN_RANGE, MAX_RANGE), MIN_RANGE, MAX_RANGE, 0x2AA0, 0xD554);
}

