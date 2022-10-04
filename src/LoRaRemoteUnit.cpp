#include "LoRaNet.h"

LoRaRemoteUnit::LoRaRemoteUnit()
: LoRaNode() {
}

LoRaRemoteUnit::LoRaRemoteUnit(byte unitAddr)
: LoRaNode(unitAddr) {
}
