#include <Arduino.h>
#include <LoRa.h>
#include <LoRaNet.h>

byte myUnitAddr = 0x05;

byte siteId[] = {'L', 'O', 'L'};
byte cryptoKey[] = {'1','6','b','y','t','e','s','S','e','c','r','e','t','K','e','y'};

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

  LoRaNet.begin(siteId, sizeof(siteId), cryptoKey);
  
  Serial.println("LoRa init succeeded.");

  delay(2000);
  
  LoRaNet.initNode(3);
}

Node gw = Node(0);

void loop() {
  //LoRaNet._send(gw, cryptoKey, 7, siteId, sizeof(siteId));
  //LoRaNet._send(gw, cryptoKey, 7, NULL, 0);
  //delay(2000);

  LoRaNet._recv();
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();
  LoRa.print(outgoing);
  LoRa.endPacket();
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  Serial.print("<<< ");
  Serial.print(millis());
  Serial.print(" ");
  Serial.println(incoming);
}
