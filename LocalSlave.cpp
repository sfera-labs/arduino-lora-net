#include "LoRaNet.h"

RemoteMaster _MASTER = RemoteMaster();
RemoteMaster _MASTER_ARRAY[1];

LocalSlave::LocalSlave(byte unitAddr)
: LocalUnit(unitAddr) {
  _MASTER_ARRAY[0] = _MASTER;
  _MASTER._set_local_slave(this);
  //_master = &RemoteMaster(this);
  //_MASTER_NODE[0] = _master;
  LoRaNet.setNodes(_MASTER_ARRAY, 1);
}

void LocalSlave::process() {
  LocalUnit::process();
  // TODO monitor IOs
}
