#include "LoRaNet.h"

Node::Node() {
}

Node::Node(byte unitAddr) {
  _unit_addr = unitAddr;
  _session[0] = 0;
  _counter_send = 0;
  _counter_recv = -1;
  _reset_trial = 0;
  _reset_last = millis();
  _reset_intvl = 0;
}

byte Node::getAddr() {
  return _unit_addr;
}
