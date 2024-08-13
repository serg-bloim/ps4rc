#pragma once
#include <PS4Controller.h>
#include <functional>
#include "../utils/utils.hpp"
#include "esp_gap_bt_api.h"

class Ps4Client;
void on_packet_received_global();
Ps4Client* inst2 = nullptr;

class Ps4Client {
public:
  int32_t throttle;
  int32_t pitch;
  int32_t roll;
  int32_t yaw;
  Ps4Client()
    : ps4controller(PS4) {
    inst2 = this;
  }
  void begin(const char* mac) {
    ps4controller.attach(on_packet_received_global);
    ps4controller.begin(mac);
    this->removePairedDevices(); // This helps to solve connection issues
  }
  void on_packet_received() {
    throttle = parseThrottle();
    pitch = parseSlider(ps4controller.RStickY());
    roll = parseSlider(ps4controller.RStickX());
    yaw = parseSlider(ps4controller.LStickX());
  }
protected:
  void removePairedDevices() {
    uint8_t pairedDeviceBtAddr[20][6];
    int count = esp_bt_gap_get_bond_device_num();
    esp_bt_gap_get_bond_device_list(&count, pairedDeviceBtAddr);
    for (int i = 0; i < count; i++) {
      esp_bt_gap_remove_bond_device(pairedDeviceBtAddr[i]);
    }
  }
private:
  PS4Controller& ps4controller;
  int32_t parseThrottle(){
    return parseSlider(ps4controller.LStickY());
  }
  int32_t parseSlider(int8_t v){
    v = cap(v, -127, 127);
    return map(v, -127, 127, -0xFFFF, 0xFFFF);
  }
};


void on_packet_received_global(){
  if(inst2){
    inst2->on_packet_received();
  }
}