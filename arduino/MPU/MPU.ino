#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/twi.h>
#include<Wire.h>


 
byte lowByte,highByte,address,gyro_address;

unsigned long timer=0;
unsigned long loop_timer;
double gyro_pitch, gyro_roll, gyro_yaw;
double gyro_axis_cal[4];
float gyro_roll_cal, gyro_pitch_cal, gyro_yaw_cal;
int cal_int=0;
float roll_level_adjust, pitch_level_adjust;
int acc_axis[4], gyro_axis[4];
float angle_roll_acc, angle_pitch_acc, angle_pitch, angle_roll;
long acc_x, acc_y, acc_z, acc_total_vector;
boolean gyro_angles_set;
int temperature;
float pid_i_mem_roll, pid_roll_setpoint, gyro_roll_input, pid_output_roll, pid_last_roll_d_error;
float pid_i_mem_pitch, pid_pitch_setpoint, gyro_pitch_input, pid_output_pitch, pid_last_pitch_d_error;
float pid_i_mem_yaw, pid_yaw_setpoint, gyro_yaw_input, pid_output_yaw, pid_last_yaw_d_error;

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












byte search_gyro(int gyro_address, int who_am_i){
  Wire.beginTransmission(gyro_address);
  Wire.write(who_am_i);
  Wire.endTransmission();
  Wire.requestFrom(gyro_address, 1);
  timer = millis() + 100;
  while(Wire.available() < 1 && timer > millis());
  lowByte = Wire.read();
  address = gyro_address;
  return lowByte;
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
    acc_axis[1] = Wire.read()<<8|Wire.read();                               //Add the low and high byte to the acc_x variable.
    acc_axis[2] = Wire.read()<<8|Wire.read();                               //Add the low and high byte to the acc_y variable.
    acc_axis[3] = Wire.read()<<8|Wire.read();                               //Add the low and high byte to the acc_z variable.
    temperature = Wire.read()<<8|Wire.read();                               //Add the low and high byte to the temperature variable.
    gyro_axis[1] = Wire.read()<<8|Wire.read();                              //Read high and low part of the angular data.
    gyro_axis[2] = Wire.read()<<8|Wire.read();                              //Read high and low part of the angular data.
    gyro_axis[3] = Wire.read()<<8|Wire.read();                              //Read high and low part of the angular data.

  
     if(cal_int == 2000){
    gyro_axis[1] -= gyro_axis_cal[1];                                       //Only compensate after the calibration.
    gyro_axis[2] -= gyro_axis_cal[2];                                       //Only compensate after the calibration.
    gyro_axis[3] -= gyro_axis_cal[3];                                       //Only compensate after the calibration.
  }

gyro_roll = gyro_axis[2];                      //Set gyro_roll to the correct axis that was stored in the EEPROM.
 // if(eeprom_data[28] & 0b10000000)gyro_roll *= -1;                          //Invert gyro_roll if the MSB of EEPROM bit 28 is set.
gyro_pitch = gyro_axis[1];                     //Set gyro_pitch to the correct axis that was stored in the EEPROM.
 //if(eeprom_data[29] & 0b10000000)gyro_pitch *= -1;                         //Invert gyro_pitch if the MSB of EEPROM bit 29 is set.
gyro_yaw = gyro_axis[3];                       //Set gyro_yaw to the correct axis that was stored in the EEPROM.
 //if(eeprom_data[30] & 0b10000000)gyro_yaw *= -1;                           //Invert gyro_yaw if the MSB of EEPROM bit 30 is set.

//divide by 2048 for +-16g scale

acc_x = acc_axis[2]/2048;                           //Set acc_x to the correct axis that was stored in the EEPROM.
 // if(eeprom_data[29] & 0b10000000)acc_x *= -1;                              //Invert acc_x if the MSB of EEPROM bit 29 is set.
acc_y = acc_axis[1]/2048;                           //Set acc_y to the correct axis that was stored in the EEPROM.
 // if(eeprom_data[28] & 0b10000000)acc_y *= -1;                              //Invert acc_y if the MSB of EEPROM bit 28 is set.
acc_z = acc_axis[3]/2048;                           //Set acc_z to the correct axis that was stored in the EEPROM.
 // if(eeprom_data[30] & 0b10000000)acc_z *= -1;                              //Invert acc_z if the MSB of EEPROM bit 30 is set.


 

}

long crashGValue = 16;

void detectCrash(){

  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));

  Serial.println(acc_total_vector);
     
  if(acc_total_vector >= crashGValue){
    
    }
  
  }







void setup() {
  Serial.begin(57600);
  Wire.setClock(400000L);
  Serial.println(TWBR);
   if(TWBR == 12 ){
    Serial.println(F("I2C clock speed is correctly set to 400kHz."));
  }

  // put your setup code here, to run once:

findMpu();
setupMpu();
Serial.println("setup ok");

}





void loop() {
  // put your main code here, to run repeatedly:
readGyro();
detectCrash();
//Serial.print((int)gyro_roll);Serial.print(" ");Serial.print((int)gyro_pitch);Serial.print(" ");Serial.print((int)gyro_yaw);Serial.println(" ");
delay(25);
//Serial.println(acc_z/2048);
//showAngles();

}


  

