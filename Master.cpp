#include "LoRaNet.h"

Master::Master()
: LocalUnit(0) {
  _process_idx = 0;
  _num_of_slaves = 0;
}

void Master::setSlaves(RemoteSlave **slaves, int numOfSlaves) {
  LoRaNet.setNodes((Node**) slaves, numOfSlaves);
  _slaves = slaves;
  _num_of_slaves = numOfSlaves;
}

void Master::enableDiscovery(RemoteSlave **buff, int maxSize) {
  LoRaNet.enableDiscovery((Node**) buff, maxSize);
  _slaves = buff;
  _num_of_slaves = maxSize;
}

void Master::process() {
  LocalUnit::process();
  if (_process_idx < _num_of_slaves) {
    if (_slaves[_process_idx]->process()) {
      _process_idx++;
    }
  } else {
    _process_idx = 0;
  }
}
