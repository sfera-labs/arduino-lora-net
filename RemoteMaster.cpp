#include "LoRaNet.h"

#define _HEARTBEAT_DELAY 600000
#define _UPD_REPEAT_DELAY 9500
#define _UPD_REPEAT_CHANCES 5

RemoteMaster::RemoteMaster()
: RemoteUnit(0) {
}

void RemoteMaster::_set_local_slave(LocalSlave *local_slave) {
  _local_slave = local_slave;
}

void RemoteMaster::_on_session_reset() {
  __DEBUGprint("RemoteMaster::_on_session_reset: ");
  for (int i = 0; i < 8; i++) {
    __DEBUGprint(_session[i], HEX);
    __DEBUGprint(" ");
  }
  __DEBUGprintln();

  // trigger an update within a second from now
  _last_update_ts = millis() - (_HEARTBEAT_DELAY - 1000);
  _last_update_attempts = 0;
}

bool RemoteMaster::_send_update() {
  return _send_update(_local_slave->_get_state_data(),
                        _local_slave->_get_state_data_len());
}

bool RemoteMaster::_send_update(byte *data, int data_len) {
  __DEBUGprint("RemoteMaster::_send_update: ");
  for (int i = 0; i < data_len; i++) {
    __DEBUGprint(data[i], HEX);
    __DEBUGprint(" ");
  }
  __DEBUGprintln();

  _last_update_data = data;
  _last_update_ts = millis();

  return send(_MSG_UPD, data, data_len);
}

void RemoteMaster::_process() {
  if (_local_slave->_has_updates()) {
    __DEBUGprintln("RemoteMaster::_process: sending update");
    _send_update();
    _last_update_attempts = 0;

  } else if (millis() - _last_update_ts >= _HEARTBEAT_DELAY) {
    __DEBUGprintln("RemoteMaster::_process: sending heartbeat");
    _send_update();

  } else if (_last_update_attempts < _UPD_REPEAT_CHANCES
        && memcmp(_last_update_ack, _last_update_data, _local_slave->_get_state_data_len()) != 0
        && (millis() - _last_update_ts) >= _UPD_REPEAT_DELAY) {
    __DEBUGprintln("RemoteMaster::_process: repeat update");
    if (_send_update()) {
      _last_update_attempts++;
    }
  }
}

void RemoteMaster::_process_message(byte msg_type, byte *data, int data_len) {
  if (msg_type == _MSG_ACK) {
    __DEBUGprintln("RemoteMaster::_process_message ACK");
    memcpy(_last_update_ack, data, data_len);

  } else if (msg_type == _MSG_CMD) {
    __DEBUGprintln("RemoteMaster::_process_message CMD");
    _local_slave->_set_state(data, data_len);
  }
}
