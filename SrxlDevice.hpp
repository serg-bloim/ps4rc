#ifndef SrxlDevice_H
#define SrxlDevice_H
#include <Arduino.h>
#include "utils.hpp"
#include "SoftwareSerial.h"
#include "ByteBuffer.hpp"
#include "SrxlCommon.hpp"
#include "SrxlStreamReader.hpp"
#include "List.hpp"

class SrxlDevice {
private:
  SoftwareSerial &serial;
  unsigned int time_frame;
  unsigned long time_frame_start;
  unsigned long time_frame_next;
  SrxlStreamReader ssr;
  SrxlPacketBuffer next_msg;

protected:
  SrxlDeviceID id;
  uint32_t uid = 0x524753;
  byte baud_rate = 0;
  byte info = 1;
  byte priority = 0xA;

  virtual void event_on_before_sending_msg(SrxlPacketBuffer &) {}
  virtual void event_on_after_sending_msg(SrxlPacketBuffer &) {}
  virtual void event_on_received_msg(SrxlGenericPack &) {}

public:
  SrxlDevice(SoftwareSerial &serial, SrxlDeviceID id, int time_frame = 22)
    : serial(serial),
      time_frame(time_frame),
      id(id) {
    this->time_frame_start = millis();
    this->time_frame_next = time_frame_start + time_frame;
  }
  void begin(size_t pin) {
    serial.enableRxGPIOPullUp(true);
    serial.begin(115200, EspSoftwareSerial::Config::SWSERIAL_8N1, pin, pin, false, 256);
    serial.enableIntTx(false);
    serial.enableTx(false);
  }
  void update() {
    if (serial.available()) {
      ssr.append(serial.read());
      if (ssr.msg_ready()) {
        SrxlGenericPack msg;
        ssr.read_msg_into(msg);
        event_on_received_msg(msg);
      }
    }
    auto now = millis();
    if (now > time_frame_next) {
      next_msg.clear();
      event_on_before_sending_msg(next_msg);
      do_send();
      event_on_after_sending_msg(next_msg);
      time_frame_next += time_frame;
    }
  }
  size_t do_send() {
    serial.enableTx(true);
    size_t sz = 0;
    sz = serial.write(next_msg.c_str(), next_msg.len());
    serial.enableTx(false);
    return sz;
  }
};

class SrxlMaster : public SrxlDevice {
  struct ChannelData {
    int32_t value;
    bool enabled;
  };
private:
  ChannelData channels[32];
protected:
  enum State {
    HANDSHAKE,
    CONTROL,
  };
  State state = HANDSHAKE;
  StaticList<byte, 255> slaves;
  size_t handshake_order_ind;
  byte handshake_order[4] = { 1, 2, 0x31, 0x31 };
  void event_on_before_sending_msg(SrxlPacketBuffer &msg) override {
    switch (state) {
      case HANDSHAKE:
        handshake(msg);
        break;
      case CONTROL:
        control(msg);
        break;
      default:
        break;
    }
  }
  void event_on_after_sending_msg(SrxlPacketBuffer &msg) override {
    if (handshake_order_ind == 0) {
      event_on_finished_handshake(state == State::CONTROL);
    }
  }
  void event_on_received_msg(SrxlGenericPack &msg) override {
    SrxlMaster &master = *this;
    switch (msg.type) {
      case PT_HANDSHAKE:
        auto hs = (SrxlHandshakePack &)(msg);
        master.on_received_handshake(hs);
        break;
    }
  }
  virtual void event_on_finished_handshake(bool result) {}

  void handshake(SrxlPacketBuffer &msg) {
    if (handshake_order_ind == arraySize(handshake_order)) {
      if (slaves.size() > 0) {
        Serial.println("Handshake complete");
        msg.clear();
        state = State::CONTROL;
        byte dst_id = 0xFF;
        msg.p_type = PT_HANDSHAKE;
        msg.write(this->id);
        msg.write(dst_id);
        byte priority = this->priority;
        msg.write(priority);
        msg.write(this->baud_rate);
        msg.write(this->info);
        msg.write(this->uid);
        msg.pack();
      } else {
        // Serial.println("Ended handshake loop, but no slaves found.");
        reset();
      }
    } else {
      msg.clear();
      byte dst_id = handshake_order[handshake_order_ind++];
      msg.p_type = PT_HANDSHAKE;
      msg.write(this->id);
      msg.write(dst_id);
      msg.write(this->priority);
      msg.write(this->baud_rate);
      msg.write(this->info);
      msg.write(this->uid);
      msg.pack();
    }
  }
  void control(SrxlPacketBuffer &msg) {
    msg.clear();
    msg.p_type = PT_CONTROL;
    byte cmd = 0;
    byte reply_id = 0;
    byte rssi = 0;
    uint16_t frame_losses = 0;
    uint32_t ch_mask = build_ch_mask();
    uint32_t ch_mask2 = ch_mask;
    msg.write(cmd);
    msg.write(reply_id);
    msg.write(rssi);
    msg.write(frame_losses);
    msg.write(ch_mask);
    for (auto ch : channels) {
      if (ch.enabled) {
        auto v = convert_ch_data(ch.value);
        msg.write(v);
      }
    }
    msg.pack();
  }
  uint32_t build_ch_mask() {
    uint32_t mask = 0;
    uint32_t flag = 1;
    for (auto ch : channels) {
      if (ch.enabled) {
        mask |= flag;
      }
      flag = flag << 1;
    }
    return mask;
  }

  uint16_t convert_ch_data(uint16_t v) {
    return 0xFFFC & cap(v, 0x2AA0, 0xD554);
  }
  void on_received_handshake(SrxlHandshakePack &msg) {
    if (!slaves.contains(msg.data.src_id)) {
      slaves.add(msg.data.src_id);
      Serial.printf("Adding slave id (%02X) into slaves list\n", msg.data.src_id);
    } else {
      Serial.printf("Slave id (%02X) is already in slaves list\n", msg.data.src_id);
    }
  }
  void reset() {
    state = HANDSHAKE;
    handshake_order_ind = 0;
    slaves.clear();
  }

public:
  SrxlMaster(SoftwareSerial &serial, SrxlDeviceID id, int time_frame = 22)
    : SrxlDevice(serial, id, time_frame) {
    reset();
    for (auto ch : channels) {
      ch.enabled = false;
      ch.value = 0;
    }
  }
  
  void set_channel(uint8_t ch, uint16_t v) {
    assert(ch < arraySize(this->channels));
    this->channels[ch].value = v;
  }

  void enable_channel(uint8_t ch, bool enabled) {
    assert(ch < arraySize(this->channels));
    this->channels[ch].enabled = enabled;
  }
};
#endif  // SrxlDevice_H
