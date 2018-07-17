#include <Arduino.h>
#include <LoRa.h>
#include <LoRaNet.h>

byte myUnitAddr = 0x05;

byte siteId[] = {'L', 'O', 'L'};
byte cryptoKey[] = {'1','6','b','y','t','e','s','S','e','c','r','e','t','K','e','y'};

/*
Node n2 = Node(2);
Node n3 = Node(3);
Node nodes[] = { n2, n3 };
*/

IonoLocalSlave slave = IonoLocalSlave(3);

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
  
  Serial.println("LoRa init succeeded.");

  delay(2000);

  
  //LoRaNet.initNode(3);

  //LoRaNet.initGW(nodes, 2);
}

void loop() {
  //LoRaNet._send(gw, cryptoKey, 7, siteId, sizeof(siteId));
  //LoRaNet._send(gw, cryptoKey, 7, NULL, 0);
  //delay(2000);

  slave.process();
  //LoRaNet.process();
}

