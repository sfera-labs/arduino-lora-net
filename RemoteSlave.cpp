#include "LoRaNet.h"

#define _CMD_ACK_TIMEOUT 5000
#define _CMD_REPEAT_DELAY (_CMD_ACK_TIMEOUT + 5500)
#define _CMD_REPEAT_CHANCES 5

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
  __DEBUGprintln("RemoteSlave::_send_cmd");
  if (send(_MSG_CMD, _get_cmd_data(), _get_cmd_data_len())) {
    _last_cmd_ts = millis();
    return true;
  }
  return false;
}

void RemoteSlave::_process_message(byte msg_type, byte *data, int data_len) {
  if (msg_type == _MSG_UPD) {
    __DEBUGprintln("RemoteSlave::_process_message UPD");
    _last_update_ts = millis();
    _last_update_ts_valid = true;
    _update_state(data, data_len);
    send(_MSG_ACK, data, data_len);
  }
}

unsigned int RemoteSlave::stateAge() {
  if (_last_update_ts_valid) {
    return (millis() - _last_update_ts) / 1000;
  }
  return (unsigned int) -1;
}

bool RemoteSlave::process() {
  if (getAddr() == 0xff) {
    return true;
  }

  if (_has_cmds()) {
    __DEBUGprintln("RemoteSlave::process sending cmd");
    _last_cmd_attempts = 0;
    _send_cmd();
  }

  if (_check_cmd_success()) {
    return true;
  }

  if (_last_cmd_attempts < _CMD_REPEAT_CHANCES) {
    if (millis() - _last_cmd_ts >= _CMD_REPEAT_DELAY) {
      __DEBUGprintln("RemoteSlave::process repeat cmd");
      if (_send_cmd()) {
        _last_cmd_attempts++;
      }
    }
  }

  return (millis() - _last_cmd_ts >= _CMD_ACK_TIMEOUT);
}
