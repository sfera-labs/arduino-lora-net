#include "LoRaNet.h"

RemoteMaster::RemoteMaster()
: RemoteUnit(0) {
}

void RemoteMaster::_set_local_slave(LocalSlave *local_slave) {
  _local_slave = local_slave;
}

void RemoteMaster::_on_session_reset() {
  // TODO
  Serial.println("RemoteMaster::_on_session_reset");
}

void RemoteMaster::_process_message(byte msg_type, byte *data, int data_len) {
  // TODO
  Serial.println("RemoteMaster::_process_message");
}
