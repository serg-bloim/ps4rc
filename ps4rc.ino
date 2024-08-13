#include "StaticConfig.h"
#include <SoftwareSerial.h>
#include "utils/utils.hpp"
#include "utils/Blinker.hpp"
#include "bluetooth/Ps4Client.hpp"
#include "sbus/sbus.hpp"

SoftwareSerial serial;
#if SERIAL_MODE == SRXL

#include "srxl2/RemoteReceiver.hpp"
RemoteReceiver itx(serial, DEVICES_REMOTE_RECEIVER);

#elif SERIAL_MODE == SBUS

#include "sbus/sbus.hpp"
Sbus itx(serial);

#else
    #error "SERIAL_MODE should be 1 of : SRXL, SBUS"
#endif
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
  itx.set_channel(itx.CH_THROTTLE, ps4.throttle);
  itx.set_channel(itx.CH_AILERON, ps4.roll);
  itx.set_channel(itx.CH_ELEVATOR, ps4.pitch);
  itx.set_channel(itx.CH_RUDDER, ps4.yaw);
  itx.update();
  delay(0);
}
