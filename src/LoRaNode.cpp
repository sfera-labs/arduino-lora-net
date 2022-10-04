#include "LoRaNet.h"

LoRaNode::LoRaNode()
: LoRaNode(0xff) {
}

LoRaNode::LoRaNode(byte unitAddr) {
  _unit_addr = unitAddr;
  _session_set = false;
  _counter_send = 0;
  _counter_recv = -1;
  _reset_trial = 0;
  _reset_last = millis();
  _reset_intvl = 0;
}

byte LoRaNode::getAddr() {
  return _unit_addr;
}

void LoRaNode::setAddr(byte unitAddr) {
  _unit_addr = unitAddr;
}

bool LoRaNode::send(byte msg_type, byte *data, int data_len) {
  return LoRaNet._send(*this, msg_type, data, data_len);
}

int LoRaNode::loraRssi() {
  return _lora_rssi;
}

float LoRaNode::loraSnr() {
  return _lora_snr;
}
