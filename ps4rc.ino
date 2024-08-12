#include "StaticConfig.h"
#include <SoftwareSerial.h>
#include "utils.hpp"
#include "Blinker.hpp"
#include "Ps4Client.hpp"
#include "sbus.hpp"

SoftwareSerial serial;
Sbus itx(serial);
Blinker blk(2, 500);
Ps4Client ps4;

void setup() {
  Serial.begin(115200);
  blk.begin();
  itx.begin(SERIAL_PIN);
  ps4.begin(PS4_BT_ADDR);
  delay(1000);
  Serial.println("Start");
}

void loop() {
  blk.update();
  itx.set_channel(0, ps4.yaw);
  itx.set_channel(1, ps4.pitch);
  itx.set_channel(2, ps4.throttle);
  itx.set_channel(3, ps4.roll);
  itx.update();
  delay(0);
}
