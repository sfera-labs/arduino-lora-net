#include "LoRaNet.h"

LoRaRemoteMaster _REMOTE_MASTER = LoRaRemoteMaster();
LoRaNode *_MASTER_ARRAY[1];

LoRaLocalSlave::LoRaLocalSlave()
: LoRaLocalSlave(0xff) {
}

LoRaLocalSlave::LoRaLocalSlave(byte unitAddr)
: LoRaLocalUnit(unitAddr) {
  _REMOTE_MASTER._set_local_slave(this);
  _MASTER_ARRAY[0] = &_REMOTE_MASTER;
  LoRaNet.setNodes(_MASTER_ARRAY, 1);
}

void LoRaLocalSlave::process() {
  LoRaLocalUnit::process();
  _process_ios();
  _REMOTE_MASTER._process();
}
