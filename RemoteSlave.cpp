#include "LoRaNet.h"

#define _CMD_REPEAT_CHANCES 5
#define _CMD_REPEAT_DELAY 10500

RemoteSlave::RemoteSlave()
: RemoteSlave(0xff) {
}

RemoteSlave::RemoteSlave(byte unitAddr)
: RemoteUnit(unitAddr) {
  _last_update_ts_valid = false;
}

void RemoteSlave::_on_session_reset() {
  _last_cmd_attempts = 0;
}

bool RemoteSlave::_send_cmd() {
  Serial.println("RemoteSlave::_send_cmd");
  _last_cmd_ts = millis();
  return send(_MSG_CMD, _get_cmd_data(), _get_cmd_data_len());
}

void RemoteSlave::_process_message(byte msg_type, byte *data, int data_len) {
  if (msg_type == _MSG_UPD) {
    Serial.println("RemoteSlave::_process_message UPD");
    _last_update_ts = millis();
    _last_update_ts_valid = true;
    _update_state(data, data_len);
    send(_MSG_ACK, data, data_len);
    // trigger a cmd repeat within 300ms from now if needed
    _last_cmd_ts = millis() - (_CMD_REPEAT_DELAY - 300);
    _last_cmd_attempts = 0;
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
    Serial.println("RemoteSlave::process sending cmd");
    _send_cmd();
    _last_cmd_attempts = 0;
  }
  if (_last_cmd_attempts < _CMD_REPEAT_CHANCES && !_check_cmd_success()) {
    if (millis() - _last_cmd_ts >= _CMD_REPEAT_DELAY) {
      Serial.println("RemoteSlave::process repeat cmd");
      if (_send_cmd()) {
        _last_cmd_attempts++;
      }
    }
  }
}
