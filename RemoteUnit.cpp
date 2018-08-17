#include "LoRaNet.h"

RemoteUnit::RemoteUnit()
: Node() {
}

RemoteUnit::RemoteUnit(byte unitAddr)
: Node(unitAddr) {
}
