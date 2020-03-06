#include <SPI.h>
#include <LoRa.h>

int counter = 0;
String id = "~a";
String message;
String chk;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  LoRa.setPins(10,5,3);
  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");
    
    // read packet
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }
    
    if (message.indexOf(id) == -1){
      message += id;
      LoRa.beginPacket();
      LoRa.print(message);
      chk = LoRa.print(String(checksum(message), HEX));
      LoRa.print(chk);
      LoRa.endPacket();
      Serial.print(message+chk);
    }
    
    // print RSSI of packet
    Serial.print(message + "' with RSSI ");
    Serial.println(LoRa.packetRssi());

    
  }
  if (Serial.available())
  {
    message = Serial.readString();
    message += id;
    LoRa.beginPacket();
    LoRa.print(message);
    chk = LoRa.print(String(checksum(message), HEX));
    LoRa.print(chk);
    LoRa.endPacket();
    Serial.print(message+chk);
  }
  
  
}

int checksum(String message){
  int sum = 0;
  for(int i = 0; i<=(message.length()); i++){
    sum += message[i];
  }
  return sum%256;
}
