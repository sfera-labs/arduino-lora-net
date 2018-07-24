#include "LoRaNet.h"

RemoteSlave::RemoteSlave(byte unitAddr)
: RemoteSlave(unitAddr) {
}

void RemoteSlave::_on_session_reset() {
  Serial.print("RemoteSlave::_on_session_reset: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(_session[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void RemoteSlave::_send_cmd() {
  Serial.println("RemoteMaster::_send_cmd");
  // TODO
}

void RemoteSlave::_process_message(byte msg_type, byte *data, int data_len) {
  // TODO
}
