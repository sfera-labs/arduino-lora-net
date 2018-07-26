#include <Arduino.h>
#include <LoRa.h>
#include <LoRaNet.h>
#include <Iono.h>

byte siteId[] = {'L', 'O', 'L'};
byte cryptoKey[] = {'1','6','b','y','t','e','s','S','e','c','r','e','t','K','e','y'};

IonoRemoteSlave iono2 = IonoRemoteSlave(2);
IonoRemoteSlave *slaves[] = { &iono2 };

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

  master.setSlaves(*slaves, 1);

  Serial.println("LoRa init succeeded.");
}

unsigned long ts = millis();
int do1 = 0;

void loop() {
  master.process();
  
  if (millis() - ts >= 1000) {
    Serial.println("---------");
    Serial.print("DO1 ");
    Serial.println(iono2.read(DO1));
    Serial.print("DO2 ");
    Serial.println(iono2.read(DO2));
    Serial.print("DO3 ");
    Serial.println(iono2.read(DO3));
    Serial.print("DO4 ");
    Serial.println(iono2.read(DO4));

    Serial.print("DI1 ");
    Serial.println(iono2.read(DI1));
    Serial.print("DI2 ");
    Serial.println(iono2.read(DI2));
    Serial.print("DI3 ");
    Serial.println(iono2.read(DI3));
    Serial.print("DI4 ");
    Serial.println(iono2.read(DI4));

    Serial.print("AV3 ");
    Serial.println(iono2.read(AV3));
    Serial.print("AI3 ");
    Serial.println(iono2.read(AI3));
    Serial.print("AV4 ");
    Serial.println(iono2.read(AV4));
    Serial.print("AI4 ");
    Serial.println(iono2.read(AI4));

    Serial.print("RSSI ");
    Serial.println(iono2.loraRssi());
    Serial.print("SNR ");
    Serial.println(iono2.loraSnr());
    Serial.print("AGE ");
    Serial.println(iono2.stateAge());
    
    do1 = (do1 + 1) % 2;
    iono2.write(DO1, do1);
    iono2.write(AO1, do1 * 5);
    
    ts = millis();
  }
}

