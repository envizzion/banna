#include <DFRobot_sim808.h>
#include <util/twi.h>
#include<Wire.h>


byte lowByte,highByte,address,gyro_address;


unsigned long loop_timer;
double gyro_pitch, gyro_roll, gyro_yaw;

int acc_x, acc_y, acc_z, acc_total_vector;
int temperature;
float temp;
long crashGValue = 16;
float dangerTempValue = 60;

boolean detectedEmergency = false;

  //make sure that the baud rate of SIM900 is 9600!
  //you can use the AT Command(AT+IPR=9600) to set it through SerialDebug

#define PIN_TX 10
#define PIN_RX 11

SoftwareSerial mySerial(PIN_TX,PIN_RX);
DFRobot_SIM808 sim808(&mySerial);//Connect RX,TX,PWR,


  

//  char http_cmd[] = "GET http://us-central1-accident-444a8.cloudfunctions.net/webApi/api/v1/locations/0713245678&10.2&11.3 HTTP/1.0\r\n\r\n";
  char buffer[400];

  

//  String apiStart="";
//  String apiEnd = " HTTP/1.0\r\n\r\n";

//  String userID = "0713245678&";
//  String longitude = "12&";
//  String latitude = "13";

  String apiTemp = "";
  void setup(){
    //mySerial.begin(9600);
     Serial.begin(9600);
      mySerial.begin(9600);
//   apiStart +=userID;
//   apiStart="";F("GET http://us-central1-accident-444a8.cloudfunctions.net/webApi/api/v1/locations/0713245678&");
//sendGPSData();

 findMpu();
 setupMpu();

  
 pinMode(9,OUTPUT);
 digitalWrite(9,LOW);
  }

  void loop(){


  readGyro();

  
  if(detectCrash() || detectTemp()){
      printValues();

    //sending led on
     digitalWrite(9,HIGH);

         sendGPSData();
    //     sending led off
     digitalWrite(9,LOW);
    }
  printValues();
  delay(25);
  }


  void sendGPSData(){

    //******** Initialize sim808 module *************
    while(!sim808.init()) {
        delay(1000);
        Serial.print(F("Sim808 init error\r\n"));
    }
    delay(3000);
   
   while(!sim808.attachGPS()) {Serial.println(F("Open the GPS power failure")); delay(1000);}
    Serial.println(F("Open the GPS power success"));

   while(!sim808.getGPS());

   //************** Get GPS data *******************

//    
    Serial.print(F("latitude :"));
//    Serial.println(sim808.GPSdata.lat,6);
//    latitude = String(sim808.GPSdata.lat,6);
//    Serial.println(latitude);
    Serial.print(F("longitude :"));
//    longitude = String(sim808.GPSdata.lon,6);
//    Serial.println(longitude);
//    Serial.println(sim808.GPSdata.lon,6);
   apiTemp = F("GET http://us-central1-accident-444a8.cloudfunctions.net/webApi/api/v1/locations/0713245678&");
   apiTemp += String(sim808.GPSdata.lon,6);
   apiTemp += "&";
   apiTemp += String(sim808.GPSdata.lat,6);
   apiTemp += F(" HTTP/1.0\r\n\r\n");
//   
//       char apiArray[apiTemp.length() + 1];
//
//    apiTemp.toCharArray(apiArray,apiTemp.length() + 1);
    
//    Serial.println(apiArray);

    //************* Turn off the GPS power ************
   sim808.detachGPS();
   delay(2000);
           
char apiArray[apiTemp.length() + 1];
apiTemp.toCharArray(apiArray,apiTemp.length() + 1);
apiTemp = "";
//Serial.println(apiArray);
sendData(apiArray,sizeof(apiArray));  
  
 

}

void sendData(char apiArray[],int arraySize){


    //*********** Attempt DHCP *******************
    while(!sim808.join(F("hutch3g"))) {
        Serial.println(F("Sim808 join network error"));
        delay(2000);
    }

    //************ Successful DHCP ****************
    Serial.print("IP Address is ");
    Serial.println(sim808.getIPAddress());

    //*********** Establish a TCP connection ************
    if(!sim808.connect(TCP,"us-central1-accident-444a8.cloudfunctions.net", 80)) {
        Serial.println("Connect error");
    }else{
        Serial.println(F("Connect mbed.org success"));
    }

    //*********** create request *****************
//    apiTemp = apiStart;
//    apiTemp +=longitude;
//    apiTemp +=latitude;
//    apiTemp +=apiEnd;
//
//    char apiArray[apiTemp.length() + 1];
//
//    apiTemp.toCharArray(apiArray,apiTemp.length() + 1);
    
//    Serial.println(apiArray);

    //*********** Send a GET request *****************
    Serial.println(F("waiting to fetch..."));
//   char http_cmd[] = "GET http://us-central1-accident-444a8.cloudfunctions.net/webApi/api/v1/locations/0713245678&10.2&11.3 HTTP/1.0\r\n\r\n";
//      Serial.println(sizeof(http_cmd));
//      Serial.println(arraySize);
//      Serial.println("sdffs");
//      sim808.send(http_cmd, sizeof(http_cmd)-1);

   Serial.println(apiArray);

      sim808.send(apiArray, arraySize - 1);
    while (true) {
        int ret = sim808.recv(buffer, sizeof(buffer)-1);
        if (ret <= 0){
            Serial.println(F("fetch over..."));
            break;
        }
        buffer[ret] = '\0';
        Serial.print("Recv: ");
        Serial.print(ret);
        Serial.print(" bytes: ");
        Serial.println(buffer);
        break;
    }

    //************* Close TCP or UDP connections **********
    sim808.close();

    //*** Disconnect wireless connection, Close Moving Scene *******
//    sim808.disconnect();
  }

byte search_gyro(int gyro_address, int who_am_i){
  Wire.beginTransmission(gyro_address);
  Wire.write(who_am_i);
  Wire.endTransmission();
  Wire.requestFrom(gyro_address, 1);
  long timer = millis() + 100;
  while(Wire.available() < 1 && timer > millis());
  lowByte = Wire.read();
  address = gyro_address;
  return lowByte;
}

void setupMpu(){

   Wire.beginTransmission(address);                             //Start communication with the gyro
    Wire.write(0x6B);                                            //PWR_MGMT_1 register
    Wire.write(0x00);                                            //Set to zero to turn on the gyro
    Wire.endTransmission();                                      //End the transmission
    
    Wire.beginTransmission(address);                             //Start communication with the gyro
    Wire.write(0x6B);                                            //Start reading @ register 28h and auto increment with every read
    Wire.endTransmission();                                      //End the transmission
    Wire.requestFrom(address, 1);                                //Request 1 bytes from the gyro
    while(Wire.available() < 1);                                 //Wait until the 1 byte is received
    Serial.print(F("Register 0x6B is set to:"));
    Serial.println(Wire.read(),BIN);
    
    Wire.beginTransmission(address);                             //Start communication with the gyro
    Wire.write(0x1B);                                            //GYRO_CONFIG register
    Wire.write(0x08);                                            //Set the register bits as 00001000 (500dps full scale)
    Wire.endTransmission();                                      //End the transmission

    Wire.beginTransmission(address);                             //Start communication with the gyro
    Wire.write(0x1C);                                            //ACC_CONFIG register
    Wire.write(0x11);                                            //Set the register bits as 3 (+- 16G full scale)
    Wire.endTransmission();                                      //End the transmission
    
    Wire.beginTransmission(address);                             //Start communication with the gyro (adress 1101001)
    Wire.write(0x1B);                                            //Start reading @ register 28h and auto increment with every read
    Wire.endTransmission();                                      //End the transmission
    Wire.requestFrom(address, 1);                                //Request 1 bytes from the gyro
    while(Wire.available() < 1);                                 //Wait until the 1 byte is received
    Serial.print(F("Register 0x1B is set to:"));
    Serial.println(Wire.read(),BIN);


      
  }

  void findMpu(){
  Serial.println(F(""));
    Serial.println(F("==================================================="));
    Serial.println(F("Gyro search"));
    Serial.println(F("==================================================="));
    delay(2000);
    
    Serial.println(F("Searching for MPU-6050 on address 0x68/104"));
    delay(1000);
    if(search_gyro(0x68, 0x75) == 0x68){
      Serial.println(F("MPU-6050 found on address 0x68"));
      
      gyro_address = 0x68;
      return;
    }
    

      Serial.println(F("Searching for MPU-6050 on address 0x69/105"));
      delay(1000);
      if(search_gyro(0x69, 0x75) == 0x68){
        Serial.println(F("MPU-6050 found on address 0x69"));
        gyro_address = 0x69;
      }
    
}  




void readGyro(){
 Wire.beginTransmission(gyro_address);                                   //Start communication with the gyro.
    Wire.write(0x3B);                                                       //Start reading @ register 43h and auto increment with every read.
    Wire.endTransmission();                                                 //End the transmission.
    Wire.requestFrom(gyro_address,14);     
  while(Wire.available() < 14);                                           //Wait until the 14 bytes are received.
//    acc_axis[1] = Wire.read()<<8|Wire.read();                               //Add the low and high byte to the acc_x variable.
//    acc_axis[2] = Wire.read()<<8|Wire.read();                               //Add the low and high byte to the acc_y variable.
//    acc_axis[3] = Wire.read()<<8|Wire.read();                               //Add the low and high byte to the acc_z variable.


     acc_y = (Wire.read()<<8|Wire.read())/2048;                               //Add the low and high byte to the acc_x variable.
    acc_x = (Wire.read()<<8|Wire.read())/2048;                               //Add the low and high byte to the acc_y variable.
    acc_z = (Wire.read()<<8|Wire.read())/2048;     





//acc_x = acc_axis[2]/2048;                           //Set acc_x to the correct axis that was stored in the EEPROM.
 // if(eeprom_data[29] & 0b10000000)acc_x *= -1;                              //Invert acc_x if the MSB of EEPROM bit 29 is set.
//acc_y = acc_axis[1]/2048;                           //Set acc_y to the correct axis that was stored in the EEPROM.
 // if(eeprom_data[28] & 0b10000000)acc_y *= -1;                              //Invert acc_y if the MSB of EEPROM bit 28 is set.
//acc_z = acc_axis[3]/2048;                           //Set acc_z to the correct axis that was stored in the EEPROM.
 // if(eeprom_data[30] & 0b10000000)acc_z *= -1;                              //Invert acc_z if the MSB of EEPROM bit 30 is set.


 

}


boolean detectCrash(){

  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));

     
  if(acc_total_vector >= crashGValue){
    return true;
    }
  else{  
  return false;
  }
  }

boolean detectTemp(){
  temp=analogRead(A3); //Reading the value from sensor

  temp=(temp*500)/1023; 

  
  if(temp >= dangerTempValue){
    return true;
    }
  else{
  return false;
  }
  }

void printValues(){
  Serial.print(F("Acc :"));Serial.print(acc_total_vector);Serial.print(F("  temp :"));Serial.println(temp);
  
  }
