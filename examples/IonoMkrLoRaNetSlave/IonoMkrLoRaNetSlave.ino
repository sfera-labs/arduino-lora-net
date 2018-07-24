#include <Arduino.h>
#include <LoRa.h>
#include <LoRaNet.h>
#include <Iono.h>

byte siteId[] = {'L', 'O', 'L'};
byte cryptoKey[] = {'1','6','b','y','t','e','s','S','e','c','r','e','t','K','e','y'};

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
}

unsigned long ts = 0;

void loop() {
  slave.process();
  
  if (millis() - ts >= 1000) {
    Serial.println("---------");
    ts = millis();
  }
}

