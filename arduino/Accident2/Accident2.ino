#include <DFRobot_sim808.h>

  //make sure that the baud rate of SIM900 is 9600!
  //you can use the AT Command(AT+IPR=9600) to set it through SerialDebug

#define PIN_TX 10
#define PIN_RX 11

SoftwareSerial mySerial(PIN_TX,PIN_RX);
DFRobot_SIM808 sim808(&mySerial);//Connect RX,TX,PWR,


  

//  char http_cmd[] = "GET http://us-central1-accident-444a8.cloudfunctions.net/webApi/api/v1/locations/0713245678&10.2&11.3 HTTP/1.0\r\n\r\n";
  char buffer[512];

  

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
// sendData();

  }

  void loop(){

     sendGPSData();
     delay(2000);
  
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
   apiTemp += String(sim808.GPSdata.lat,6);
   apiTemp += "&";
   apiTemp += String(sim808.GPSdata.lon,6);
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
    Serial.println("waiting to fetch...");
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


