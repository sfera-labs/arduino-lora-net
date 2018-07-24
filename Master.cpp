#include "LoRaNet.h"

Master::Master()
: LocalUnit(0) {
}

void Master::setSlaves(RemoteSlave *slaves, int numOfSlaves) {
  LoRaNet.setNodes(slaves, numOfSlaves);
}

void Master::process() {
  LocalUnit::process();
  // TODO
}
