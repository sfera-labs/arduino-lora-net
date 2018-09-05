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
  if (_has_updates()) {
    Serial.println("LocalSlave::process: sending update");
    _send_update();
  }
  if (_REMOTE_MASTER._needs_repetition_or_heartbeat()) {
    Serial.println("LocalSlave::process: repeat update");
    _send_update();
  }
}

void LocalSlave::_send_update() {
  _REMOTE_MASTER._send_update(_get_state_data(), _get_state_data_len());
}
