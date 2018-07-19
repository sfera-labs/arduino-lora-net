#include "LoRaNet.h"
#include <Iono.h>

IonoLocalSlave *IonoLocalSlave::_INSTANCE;

IonoLocalSlave::IonoLocalSlave()
: LocalSlave(-1) {
}

IonoLocalSlave::IonoLocalSlave(byte unitAddr)
: LocalSlave(unitAddr) {
  _INSTANCE = this;
  _state_changed = true;
  _state_modes = 0x00;
  _state_dos = 0x00;
  _state_ao1 = 0xffff;
  _state_dis = 0x00;
  _state_a1 = 0xffff;
  _state_a2 = 0xffff;
  _state_a3 = 0xffff;
  _state_a4 = 0xffff;
}

void IonoLocalSlave::_process_ios() {
  Iono.process();
}

bool IonoLocalSlave::_has_updates() {
  return _state_changed;
}

int IonoLocalSlave::_get_state_data_len() {
  return 13;
}

byte* IonoLocalSlave::_get_state_data() {
  _state_changed = false;
  _state_data[0] = _state_modes;
  _state_data[1] = _state_dos;
  _state_data[2] = (byte) ((_state_ao1 >> 8) & 0xff);
  _state_data[3] = (byte) (_state_ao1 & 0xff);
  _state_data[4] = _state_dis;
  _state_data[5] = (byte) ((_state_a1 >> 8) & 0xff);
  _state_data[6] = (byte) (_state_a1 & 0xff);
  _state_data[7] = (byte) ((_state_a2 >> 8) & 0xff);
  _state_data[8] = (byte) (_state_a2 & 0xff);
  _state_data[9] = (byte) ((_state_a3 >> 8) & 0xff);
  _state_data[10] = (byte) (_state_a3 & 0xff);
  _state_data[11] = (byte) ((_state_a4 >> 8) & 0xff);
  _state_data[12] = (byte) (_state_a4 & 0xff);
  return _state_data;
}

void IonoLocalSlave::subscribeCallback(uint8_t pin, float value) {
  int dVal;
  _INSTANCE->_state_changed = true;

  switch (pin) {
    case DO1:
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dos = (_INSTANCE->_state_dos & 0b0111) | (dVal << 3);
      break;

    case DO2:
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dos = (_INSTANCE->_state_dos & 0b1011) | (dVal << 2);
      break;

    case DO3:
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dos = (_INSTANCE->_state_dos & 0b1101) | (dVal << 1);
      break;

    case DO4:
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dos = (_INSTANCE->_state_dos & 0b1110) | dVal;
      break;

    case AO1:
      _INSTANCE->_state_ao1 = (uint16_t) (value * 1000);
      break;

    case DI1:
      _INSTANCE->_state_modes |= 0b10000000;
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dis = (_INSTANCE->_state_dis & 0b011111) | (dVal << 5);
      break;

    case DI2:
      _INSTANCE->_state_modes |= 0b00100000;
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dis = (_INSTANCE->_state_dis & 0b101111) | (dVal << 4);
      break;

    case DI3:
      _INSTANCE->_state_modes |= 0b00001000;
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dis = (_INSTANCE->_state_dis & 0b110111) | (dVal << 3);
      break;

    case DI4:
      _INSTANCE->_state_modes |= 0b00000010;
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dis = (_INSTANCE->_state_dis & 0b111011) | (dVal << 2);
      break;

    case DI5:
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dis = (_INSTANCE->_state_dis & 0b111101) | (dVal << 1);
      break;

    case DI6:
      dVal = value == HIGH ? 1 : 0;
      _INSTANCE->_state_dis = (_INSTANCE->_state_dis & 0b111110) | dVal;
      break;

    case AI1:
      _INSTANCE->_state_modes |= 0b01000000;
    case AV1:
      _INSTANCE->_state_a1 = (uint16_t) (value * 1000);
      break;

    case AI2:
      _INSTANCE->_state_modes |= 0b00010000;
    case AV2:
      _INSTANCE->_state_a2 = (uint16_t) (value * 1000);
      break;

    case AI3:
      _INSTANCE->_state_modes |= 0b00000100;
    case AV3:
      _INSTANCE->_state_a3 = (uint16_t) (value * 1000);
      break;

    case AI4:
      _INSTANCE->_state_modes |= 0b00000001;
    case AV4:
      _INSTANCE->_state_a4 = (uint16_t) (value * 1000);
      break;
  }

}
