#include "LoRaNet.h"

LoRaLocalUnit::LoRaLocalUnit() {
}

LoRaLocalUnit::LoRaLocalUnit(byte unitAddr) {
  setAddr(unitAddr);
}

void LoRaLocalUnit::setAddr(byte unitAddr) {
  LoRaNet.setLocalAddr(unitAddr);
}

byte LoRaLocalUnit::getAddr() {
  return LoRaNet.getLocalAddr();
}

void LoRaLocalUnit::process() {
  LoRaNet.process();
}
