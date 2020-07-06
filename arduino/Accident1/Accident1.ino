#include <DFRobot_sim808.h>

  //Mobile phone number, need to change
  #define PHONE_NUMBER "0771742350"

  //The content of messages sent
  #define MESSAGE  "hello,world"

#define PIN_TX 10
#define PIN_RX 11

SoftwareSerial mySerial(PIN_TX,PIN_RX);
DFRobot_SIM808 sim808(&mySerial);//Connect RX,TX,PWR,


  void setup() {
    //mySerial.begin(9600);
    Serial.begin(9600);
      mySerial.begin(9600);

   //******** Initialize sim808 module *************
    while(!sim808.init()) {
        delay(1000);
        Serial.print("Sim808 init error\r\n");
    }
    Serial.println("Sim808 init success");
    Serial.println("Start to send message ...");

    //******** define phone number and text **********
    sim808.sendSMS(PHONE_NUMBER,MESSAGE);
  }

  void loop() {
    //nothing to do
  }
