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
#define BAND    43305E3

SSD1306 display(0x3c, 4, 15);

//packet. sender destination message message_id
char const SEPARATOR ='~';
String localAddress = "EICDHZB";
String Messages[250][2];
int current_msg = 0;
//next_hop node hops
//
String sender;
String destination;
String repeaters;
String message = "";

void setup() {
  //start OLED
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high、

  Serial.begin(115200);
  while(!Serial){
    ;
  }
  Serial.println();
  Serial.println("LoRa Repeater Callback");

  //start lora module
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1){
      ;
    }
  }
  Serial.println("init ok");
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0, 15, 128,  "INIT OK WAITING FOR FIRST PACKET");
  display.display();
  delay(1500);

}




//packet. sender destination message message_id
void loop() {
    int obj = 0;
    repeaters = "";
    message = "";
    destination = "";
    sender = "";
    String string_read = "";
    int mess_id;
    int will_send = 0;
    if (LoRa.parsePacket() > 0){
      while (LoRa.available()) {
        Serial.println(sender);
        byte char_read = LoRa.read();
        if (char_read != SEPARATOR)
          string_read += char_read;
        else{
          if(obj==0)
            sender=string_read;
          if(obj==1)
            destination=string_read;
          if(obj==2)
            message=string_read;
          if(obj==3)
            mess_id=string_read.toInt();
          if(obj>3)
            repeaters += string_read;
          string_read = "";
          obj++;
        }  
      }
    }

  for (int i=0; i<250;i++){
    if (Messages[i][0] == sender && Messages[i][1].toInt() == mess_id){
      will_send=0;
    }else{
      will_send = 1;
      Messages[current_msg][0] = sender;
      Messages[current_msg][1] = mess_id;
      if(current_msg != 249)
        current_msg++;
      else
        current_msg = 0;
      
      
    }
  }
  
  if(current_msg==1){
    LoRa.beginPacket();
    LoRa.print(sender);
    LoRa.print(destination);
    LoRa.print(message);
    LoRa.print(mess_id);
    repeaters = repeaters + SEPARATOR + localAddress;
    LoRa.print(repeaters);
    LoRa.endPacket();
    current_msg=0;
  }

}
