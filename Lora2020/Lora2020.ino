#include <SPI.h>
#include <LoRa.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
//#include "images.h"

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    433E6

SSD1306 display(0x3c, 4, 15);

int counter = 0;
String id = "~b";
String message;
String chk;
String chk_rm;
String r_message;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  LoRa.setPins(10,5,3);
  Serial.println("LoRa Sender");

  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1){
      ;
    }
  }
  Serial.println("init ok");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    
    Serial.println("message received");
    // read packet
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }
    
    if (message.indexOf(id) == -1){
      chk_rm = message.substring(message.length()-2, message.length());
      r_message = message.substring(0, message.length()-2);
        if (String(checksum(r_message),HEX) == chk_rm){
          r_message += id;
          LoRa.beginPacket();
          LoRa.print(r_message);
          chk = String(checksum(r_message), HEX);
          LoRa.print(chk);
          LoRa.endPacket();
          Serial.println("Starting repeating");
          Serial.println(r_message+chk);
          Serial.println("Ending repeating");
          message = "";
      }else{
        Serial.println("chk invalid");
      }
    }else{
    
    // print RSSI of packet
    Serial.println("Start Not repeating");
    Serial.print(message + "' with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.println("End Not repeating");
    message = "";
    }

    
  }
  if (Serial.available())
  {
    message = Serial.readString();
    message = message.substring(0, message.length()-1);
    message += id;
    LoRa.beginPacket();
    LoRa.print(message);
    chk = String(checksum(message), HEX);
    LoRa.print(chk);
    LoRa.endPacket();
    Serial.println(message+chk);
    message = "";
  }
  
  
}

int checksum(String message){
  int sum = 0;
  for(int i = 0; i<=(message.length()); i++){
    sum += message[i];
  }
  return sum%256;
}
