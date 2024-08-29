#include <Preferences.h>
#include "StaticConfig.h"
#include <SoftwareSerial.h>
#include "utils/utils.hpp"
#include "utils/Blinker.hpp"
#include "utils/config.hpp"
#include "bluetooth/Ps4Client.hpp"
#include "sbus/sbus.hpp"

SoftwareSerial serial;
#if SERIAL_MODE == SRXL
  #include "srxl2/RemoteReceiver.hpp"
  #define SERIAL_MODE_STR "SRXL"
  RemoteReceiver itx(serial, DEVICES_REMOTE_RECEIVER);
#elif SERIAL_MODE == SBUS
  #define SERIAL_MODE_STR "SBUS"
  #include "sbus/sbus.hpp"
  Sbus itx(serial);
#else
  #define SERIAL_MODE_STR "NONE"
  #error "SERIAL_MODE should be 1 of : SRXL, SBUS"
#endif
Blinker blk(2, 500);
Ps4Client ps4;
Preferences prefs;
Configurator config(prefs, CONFIG_PIN_OUT, CONFIG_PIN_IN);

void setup() {
  Serial.begin(115200);
  prefs.begin("app", RO_MODE);
  config.begin();
  blk.begin();
  delay(1000);

  String bt_address;
  int serial_pin;
  bool inverted;
  read_config(bt_address, serial_pin, inverted);
  Serial.println("PS4RC version: " PS4RC_VERSION);
  Serial.println("Serial mode: " SERIAL_MODE_STR);
  Serial.printf("bt_address = '%s'\n", bt_address.c_str());
  Serial.printf("serial_pin = '%d'\n", serial_pin);

  if(config.enabled()){
    Serial.println("Starting in configuration mode");
    blk.set_delay(200);
  }else{
    Serial.println("Starting in normal mode");
    blk.set_delay(500);
    itx.begin(serial_pin, inverted);
    ps4.begin(bt_address.c_str());
  }
}
void read_config(String &bt_address, int &serial_pin, bool &inverted){
    bt_address = prefs.getString("bt_address");
    if(bt_address.isEmpty()){
        bt_address = DEFAULT_PS4_BT_ADDR;
    }
    String serial_pin_str = prefs.getString("serial_pin");
    serial_pin = serial_pin_str.toInt();
    if(serial_pin == 0){
        Serial.printf("Cannot parse serial_pin='%s', using the default value\n", serial_pin_str.c_str());
        serial_pin = DEFAULT_SERIAL_PIN;
    }
    String inverted_str = prefs.getString("serial_inverted");
    inverted_str.toLowerCase();
    if(inverted_str == "true"){
        inverted = true;
    }else if(inverted_str == "false"){
        inverted = false;
    }else{
        Serial.printf("Cannot parse serial_inverted='%s', using the default value\n", inverted_str.c_str());
        inverted = DEFAULT_SERIAL_INVERTED;
    }
}
void loop() {
  blk.update();
  if(config.enabled()){
    config.update();
  }else{
      itx.set_channel(itx.CH_THROTTLE, ps4.throttle);
      itx.set_channel(itx.CH_AILERON, ps4.roll);
      itx.set_channel(itx.CH_ELEVATOR, ps4.pitch);
      itx.set_channel(itx.CH_RUDDER, ps4.yaw);
      itx.update();
  }
  delay(0);
}
