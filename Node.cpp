#include "LoRaNet.h"

Node::Node()
: Node(0xff) {
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

void Node::setAddr(byte unitAddr) {
  _unit_addr = unitAddr;
}

bool Node::send(byte msg_type, byte *data, int data_len) {
  return LoRaNet._send(*this, msg_type, data, data_len);
}

int Node::loraRssi() {
  return _lora_rssi;
}

float Node::loraSnr() {
  return _lora_snr;
}
