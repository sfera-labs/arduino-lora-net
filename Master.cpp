#include "LoRaNet.h"

Master::Master()
: LocalUnit(0) {
}

void Master::setSlaves(RemoteSlave *slaves, int numOfSlaves) {
  LoRaNet.setNodes(slaves, numOfSlaves);
  _slaves = slaves;
  _num_of_slaves = numOfSlaves;
}

void Master::process() {
  LocalUnit::process();
  for (int i = 0; i < _num_of_slaves; i++) {
    _slaves[i].process();
  }
}
