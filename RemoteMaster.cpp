#include "LoRaNet.h"

RemoteMaster::RemoteMaster()
: RemoteUnit(0) {
}

void RemoteMaster::_set_local_slave(LocalSlave *local_slave) {
  _local_slave = local_slave;
}

void RemoteMaster::_on_session_reset() {
  Serial.print("RemoteMaster::_on_session_reset: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(_session[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // TODO
}

void RemoteMaster::_process_message(byte msg_type, byte *data, int data_len) {
  if (msg_type == _MSG_ACK) {
    Serial.println("RemoteMaster::_process_message ACK");
    memcpy(_last_update_ack, data, data_len);
  }

  /*
  TODO
  elif msg_type == _MSG_CMD:
            print("RemoteMaster._process_message CMD")
            self._local_slave._set_state(data)
  */
}

bool RemoteMaster::_needs_repetition_or_heartbeat() {
  return (memcmp(_last_update_ack, _last_update_data, _local_slave->_get_state_data_len()) != 0
      && (millis() - _last_update_ts) >= 5000)
      || (millis() - _last_update_ts) >= 60000;
}

void RemoteMaster::_send_update(byte *data, int data_len) {
  Serial.print("RemoteMaster::_send_update: ");
  for (int i = 0; i < 13; i++) {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  _last_update_data = data;
  _last_update_ts = millis();

  // TODO remove LoRaNet._send_with_session(*this, _session, _MSG_UPD, data, data_len);


  send(_MSG_UPD, data, data_len);
}
