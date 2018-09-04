#include "LoRaNet.h"

Master::Master()
: LocalUnit(0) {
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
  for (int i = 0; i < _num_of_slaves; i++) {
    _slaves[i]->process();
  }
}
