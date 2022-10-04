#include "LoRaNet.h"

#define _HEARTBEAT_DELAY 600000
#define _UPD_REPEAT_DELAY 9500
#define _UPD_REPEAT_CHANCES 5

LoRaRemoteMaster::LoRaRemoteMaster()
: LoRaRemoteUnit(0) {
}

void LoRaRemoteMaster::_set_local_slave(LoRaLocalSlave *local_slave) {
  _local_slave = local_slave;
}

void LoRaRemoteMaster::_on_session_reset() {
  __DEBUGprint("LoRaRemoteMaster::_on_session_reset: ");
  for (int i = 0; i < 8; i++) {
    __DEBUGprint(_session[i], HEX);
    __DEBUGprint(" ");
  }
  __DEBUGprintln();

  // trigger an update within 300ms from now
  _last_update_ts = millis() - (_HEARTBEAT_DELAY - 300);
  _last_update_attempts = 0;
}

bool LoRaRemoteMaster::_send_update() {
  return _send_update(_local_slave->_get_state_data(),
                        _local_slave->_get_state_data_len());
}

bool LoRaRemoteMaster::_send_update(byte *data, int data_len) {
  __DEBUGprint("LoRaRemoteMaster::_send_update: ");
  for (int i = 0; i < data_len; i++) {
    __DEBUGprint(data[i], HEX);
    __DEBUGprint(" ");
  }
  __DEBUGprintln();

  _last_update_data = data;
  _last_update_ts = millis();

  return send(_MSG_UPD, data, data_len);
}

void LoRaRemoteMaster::_process() {
  if (_local_slave->_has_updates()) {
    __DEBUGprintln("LoRaRemoteMaster::_process: sending update");
    _send_update();
    _last_update_attempts = 0;

  } else if (millis() - _last_update_ts >= _HEARTBEAT_DELAY) {
    __DEBUGprintln("LoRaRemoteMaster::_process: sending heartbeat");
    _send_update();

  } else if (_last_update_attempts < _UPD_REPEAT_CHANCES
        && memcmp(_last_update_ack, _last_update_data, _local_slave->_get_state_data_len()) != 0
        && (millis() - _last_update_ts) >= _UPD_REPEAT_DELAY) {
    __DEBUGprintln("LoRaRemoteMaster::_process: repeat update");
    if (_send_update()) {
      _last_update_attempts++;
    }
  }
}

void LoRaRemoteMaster::_process_message(byte msg_type, byte *data, int data_len) {
  if (msg_type == _MSG_ACK) {
    __DEBUGprintln("LoRaRemoteMaster::_process_message ACK");
    memcpy(_last_update_ack, data, data_len);

  } else if (msg_type == _MSG_CMD) {
    __DEBUGprintln("LoRaRemoteMaster::_process_message CMD");
    _local_slave->_set_state(data, data_len);
  }
}
