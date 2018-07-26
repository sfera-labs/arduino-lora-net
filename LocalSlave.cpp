#include "LoRaNet.h"

RemoteMaster _MASTER_ARRAY[1];

LocalSlave::LocalSlave(byte unitAddr)
: LocalUnit(unitAddr) {
  _MASTER_ARRAY[0] = RemoteMaster();
  _MASTER_ARRAY[0]._set_local_slave(this);
  LoRaNet.setNodes(_MASTER_ARRAY, 1);
}

void LocalSlave::process() {
  LocalUnit::process();
  _process_ios();
  if (_has_updates()) {
    Serial.println("LocalSlave::process update");
    _send_update();
  }
  if (_MASTER_ARRAY[0]._needs_repetition_or_heartbeat()) {
    Serial.println("LocalSlave::process repeat");
    _send_update();
  }
}

void LocalSlave::_send_update() {
  _MASTER_ARRAY[0]._send_update(_get_state_data(), _get_state_data_len());
}
