#include "LoRaNet.h"

LoRaLocalMaster::LoRaLocalMaster()
: LoRaLocalUnit(0) {
  _process_idx = 0;
  _num_of_slaves = 0;
}

void LoRaLocalMaster::setSlaves(LoRaRemoteSlave **slaves, int numOfSlaves) {
  LoRaNet.setNodes((LoRaNode**) slaves, numOfSlaves);
  _slaves = slaves;
  _num_of_slaves = numOfSlaves;
}

void LoRaLocalMaster::enableDiscovery(LoRaRemoteSlave **buff, int maxSize) {
  LoRaNet.enableDiscovery((LoRaNode**) buff, maxSize);
  _slaves = buff;
  _num_of_slaves = maxSize;
}

void LoRaLocalMaster::process() {
  LoRaLocalUnit::process();
  if (_process_idx < _num_of_slaves) {
    if (_slaves[_process_idx]->process()) {
      _process_idx++;
    }
  } else {
    _process_idx = 0;
  }
}
