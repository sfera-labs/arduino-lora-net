#include "LoRaNet.h"

Node::Node() {
  _unit_addr = 0xff; // invalid address
}

Node::Node(byte unitAddr) {
  _unit_addr = unitAddr;
  _session_set = false;
  _counter_send = 0;
  _counter_recv = -1;
  _reset_trial = 0;
  _reset_last = millis();
  _reset_intvl = 0;
}

byte Node::getAddr() {
  return _unit_addr;
}

void Node::send(byte msg_type, byte *data, int data_len) {
  LoRaNet._send(*this, msg_type, data, data_len);
}

int Node::loraRssi() {
  return _lora_rssi;
}

float Node::loraSnr() {
  return _lora_snr;
}
