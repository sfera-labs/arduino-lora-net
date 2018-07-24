#include <Arduino.h>
#include <LoRa.h>
#include <LoRaNet.h>
#include <Iono.h>

byte siteId[] = {'L', 'O', 'L'};
byte cryptoKey[] = {'1','6','b','y','t','e','s','S','e','c','r','e','t','K','e','y'};

IonoRemoteSlave iono3 = IonoRemoteSlave(3);
IonoRemoteSlave slaves[] = { iono3 };

Master master = Master();

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Starting...");

  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa init failed.");
    while (true);
  }

  LoRa.enableCrc();
  LoRa.setSyncWord(0x34);

  LoRaNet.init(siteId, sizeof(siteId), cryptoKey);

  master.setSlaves(slaves, 1);

  Serial.println("LoRa init succeeded.");
}

unsigned long ts = 0;

void loop() {
  master.process();
  
  if (millis() - ts >= 1000) {
    Serial.println("---------");
    ts = millis();
  }
}

