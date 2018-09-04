#include "LoRaNet.h"

LocalUnit::LocalUnit() {
}

LocalUnit::LocalUnit(byte unitAddr) {
  setAddr(unitAddr);
}

void LocalUnit::setAddr(byte unitAddr) {
  LoRaNet.setLocalAddr(unitAddr);
}

byte LocalUnit::getAddr() {
  return LoRaNet.getLocalAddr();
}

void LocalUnit::process() {
  LoRaNet.process();
}
