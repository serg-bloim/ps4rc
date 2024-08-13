#pragma once
#include "../utils/utils.hpp"
#include "SrxlDevice.hpp"
#include "SrxlCommon.hpp"

class RemoteReceiver : public SrxlMaster {

private:
protected:
  byte transmitter_id;
  void event_on_finished_handshake(bool result) override {
    if (result) {
      transmitter_id = slaves[0];
    }
  }
 

  uint16_t convert_to_srxl(int32_t v) {
    return 0x8000;
  }

public:
  RemoteReceiver(SoftwareSerial &serial, SrxlDeviceID id, int time_frame = 22)
    : SrxlMaster(serial, id, time_frame) {}
};
