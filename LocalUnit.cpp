#include "LoRaNet.h"

LocalUnit::LocalUnit() {
}

LocalUnit::LocalUnit(byte unitAddr) {
  LoRaNet.setLocalAddr(unitAddr);
}

void LocalUnit::process() {
  LoRaNet.process();
}
