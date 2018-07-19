#include <Arduino.h>
#include <LoRa.h>
#include <LoRaNet.h>
#include <Iono.h>

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

  Iono.subscribeDigital(DO1, 50, &IonoLocalSlave::subscribeCallback);
  Iono.subscribeDigital(DO2, 50, &IonoLocalSlave::subscribeCallback);
  Iono.subscribeDigital(DO3, 50, &IonoLocalSlave::subscribeCallback);
  Iono.subscribeDigital(DO4, 50, &IonoLocalSlave::subscribeCallback);

  Iono.subscribeAnalog(AO1, 50, 1, &IonoLocalSlave::subscribeCallback);
  
  Iono.subscribeDigital(DI1, 50, &IonoLocalSlave::subscribeCallback);
  Iono.subscribeDigital(DI2, 50, &IonoLocalSlave::subscribeCallback);
  Iono.subscribeAnalog(AV3, 50, 0.1, &IonoLocalSlave::subscribeCallback);
  Iono.subscribeAnalog(AI4, 50, 0.1, &IonoLocalSlave::subscribeCallback);
  Iono.subscribeDigital(DI5, 50, &IonoLocalSlave::subscribeCallback);
  Iono.subscribeDigital(DI6, 50, &IonoLocalSlave::subscribeCallback);
  
  Serial.println("LoRa init succeeded.");

  delay(2000);

  
  //LoRaNet.initNode(3);

  //LoRaNet.initGW(nodes, 2);
}

float vvv = 0.0;

unsigned long ts = 0;

void loop() {
  //LoRaNet._send(gw, cryptoKey, 7, siteId, sizeof(siteId));
  //LoRaNet._send(gw, cryptoKey, 7, NULL, 0);
  //delay(2000);

  slave.process();
  
  //LoRaNet.process();

  /*
  vvv += 0.001;
  if (vvv > 10) {
    vvv = 0;
  }
  Iono.write(AO1, vvv);
  */

  if (millis() - ts >= 1000) {
    Serial.println("---------");
    ts = millis();
  }
}

