#include "LoRaNet.h"

#define _CMD_REPEAT_DELAY 5000

RemoteSlave::RemoteSlave()
: RemoteUnit() {
}

RemoteSlave::RemoteSlave(byte unitAddr)
: RemoteUnit(unitAddr) {
  _last_update_ts_valid = false;
}

void RemoteSlave::_on_session_reset() {
}

void RemoteSlave::_send_cmd() {
  Serial.println("RemoteSlave::_send_cmd");
  _last_cmd_ts = millis();
  send(_MSG_CMD, _get_cmd_data(), _get_cmd_data_len());
}

void RemoteSlave::_process_message(byte msg_type, byte *data, int data_len) {
  if (msg_type == _MSG_UPD) {
    Serial.println("RemoteSlave::_process_message UPD");
    _last_update_ts = millis();
    _last_update_ts_valid = true;
    _update_state(data, data_len);
    send(_MSG_ACK, data, data_len); // send ack
    _last_cmd_ts = millis() - (_CMD_REPEAT_DELAY - 300);
  }
}

unsigned int RemoteSlave::stateAge() {
  if (_last_update_ts_valid) {
    return (millis() - _last_update_ts) / 1000;
  }
  return (unsigned int) -1;
}

void RemoteSlave::process() {
  if (getAddr() == 0xff) {
    return;
  }
  if (_has_cmds()) {
    Serial.println("RemoteSlave::process cmd");
    _send_cmd();
  }
  if (!_check_cmd_success()) {
    if ((millis() - _last_cmd_ts) >= _CMD_REPEAT_DELAY) {
      Serial.println("RemoteSlave::process repeat");
      _send_cmd();
    }
  }
}
