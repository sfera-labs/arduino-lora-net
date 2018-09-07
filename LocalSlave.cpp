#include "LoRaNet.h"

RemoteMaster _REMOTE_MASTER = RemoteMaster();
Node *_MASTER_ARRAY[1];

LocalSlave::LocalSlave()
: LocalSlave(0xff) {
}

LocalSlave::LocalSlave(byte unitAddr)
: LocalUnit(unitAddr) {
  _REMOTE_MASTER._set_local_slave(this);
  _MASTER_ARRAY[0] = &_REMOTE_MASTER;
  LoRaNet.setNodes(_MASTER_ARRAY, 1);
}

void LocalSlave::process() {
  LocalUnit::process();
  _process_ios();
  _REMOTE_MASTER._process();
}
