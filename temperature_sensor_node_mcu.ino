#include <SoftwareSerial.h>
SoftwareSerial mySerial(12,13); // RX, TX

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

//////////////////////////////////////// ALL DECLARATIONS for MODBUS/////////////////////////////////////
unsigned char qry[8]={0x01,0x04,0x04,0x20,0x00,0x01};
unsigned char block[7]={0x20,0x21,0x22};
int count=0,i,m,j,k,ss=0,z;

unsigned char rx[30],c,d,lol;
char* dummy;
unsigned int check_sum,calc_checksum,transmit_CRC;
int t,t1,t2,t3;
int outputpin= A0;


///////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////// ALL DECLARATIONS for CLOUD //////////////////////////////
const char* host = "api.thingsio.ai";                                 // OR host = devapi2.thethingscloud.com
//const char* login_url = "/api/sessions";
const char* post_url = "/devices/deviceData";       // OR /api/v2/thingscloud2/_table/data_ac//unfrm rsrc lctr
const char* time_server = "www.convert-unix-time.com";             //this is to convert timestamp
const int httpPort = 80;

char ses_id[600];
char timestamp[10];

WiFiClient client;


//////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////CRC calculation function////////////////////////////////////////
int ModRTU_CRC(unsigned char* buf, int len)//compute the modbus rtu crc
{
  int crc = 0xFFFF,temp = 0,pos,i;
  
  for ( pos = 0; pos < len; pos++) 
  {
    crc ^= (int)buf[pos];          // XOR byte into least sig. byte of crc

    for ( i = 8; i != 0; i--)       // Loop over each bit
    {    
      if ((crc & 0x0001) != 0)      // If the LSB is set
      {      
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
    temp = 0x00FF & crc;             //Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    crc = temp<<8 |crc>>8;

    return crc;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void configModeCallback (WiFiManager *myWiFiManager) 
{
  Serial.println("Entered config mode");             //*-*-*-*-*-*-*-*-*-*-*-*-*-*if control enters this function then net is not connected
  Serial.println(WiFi.softAPIP());                  // "WiFi.softAPIP() is for AP" , "WiFi.localIP() is for STA",
                                                                
  Serial.println(myWiFiManager->getConfigPortalSSID());             //if you used auto generated SSID, print it
}
/////////////////////////////////////// TIMESTAMP CALCULATION function///////////////////////////////////////
int GiveMeTimestamp()
{
  unsigned long timeout = millis();

  while (client.available() == 0)
  {
    if (millis() - timeout > 50000)
    {
      client.stop();
      return 0;
    }
  }

while (client.available())
      {
        String line = client.readStringUntil('\r');                    //indexOf() is a funtion to search for smthng , it returns -1 if not found
        int pos = line.indexOf("\"timestamp\"");                       //search for "\"timestamp\"" from beginning of response got and copy all data after that , it'll be your timestamp
        if (pos >= 0)                                                     
        {
          int j = 0;
          for(j=0;j<10;j++)
          {
            timestamp[j] = line[pos + 12 + j];
          }
        }
      }
}  
////////////////////////////////////////////////////////////////////////////////////////////////////////




void setup() 
{
  Serial.begin(115200);     //(19200,SERIAL_8E1) - data size = 8 bits , parity = Even , stop bit =  1bit
mySerial.begin(115200);

 //WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
   WiFiManager wifiManager;
  
  //wifiManager.resetSettings();    //reset settings - for testing

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
                                                                                                    
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP" and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("ThingsCloud","Thingscloud@1"))                   //wifiManager.autoConnect("AP-NAME", "AP-PASSWORD"); (OR) wifiManager.autoConnect("AP-NAME"); only ID no password (OR) wifiManager.autoConnect(); this will generate a ID by itself
  {
    Serial.println("failed to connect and hit timeout");     //control comes here after long time of creating Access point "NodeMCU" by NodeMCU and still it has not connected
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

  //if you come here you have connected to the WiFi
  Serial.println("connected...yeey :");
 
}

void loop() 

{
  int analogValue = analogRead(outputpin);
  for(z=0;z<7;z++)
  {
  qry[3]=block[z];                                    //bcoz for the energy meter queries for difft parameters, only 4th byte is changing
  transmit_CRC = ModRTU_CRC(qry,6);
  qry[6]=transmit_CRC>>8;
  qry[7]=transmit_CRC & 0x00FF;
  
  
/////////////////////////////////////// SEND THE QUERY AND RECEIVE THE RESPONSE AND VALIDATE THE CHECKSUM///////////////////////  
 for(i=0;i<8;i++)
     {
        mySerial.write(qry[i]);     //Serial.write(query,8);  //this is only allowed for serial not software serial
        Serial.write(qry[i]);
     }

  mySerial.flush();
  Serial.flush();
  delay(2);   
  Serial.println("");
       

  if(mySerial.available()>0)                             //its like loop , it wont do any other work untill all these chars which are available are printed
     {
        count=mySerial.available();      
        //Serial.print(count,HEX);
        m=count;
        Serial.print("The response from meter in HEX is: ");
           for(i=0;i<count;i++)
              {
                rx[i]=mySerial.read();
                Serial.print(rx[i],HEX);
              }
          
         Serial.println("");
                                                  //try this to convert from any form to any other long n = strtol(&rx[3],NULL,10) ;


       }   

  t=rx[3]<<8 | rx[4];                             //cascade all required bytes from the response data and print it as "unsigned int"
  Serial.print("The actual data in decimal is: ");            //decimal bcoz mm is declared as unsigned int
  Serial.println(t);

switch(qry[3])
{
case 0x20:
       // mm = (mm*1135)>>15;
        t1=(analogValue/1024.0) * 3300;
        Serial.print("temp: ");
        //Serial.println(t1);
        break;  
case 0x21:
       // mm = (mm*50)>>15;
        t2=t1/10;;
        Serial.print("celc: ");
        Serial.println(t2);
        break;   
case 0x22:
        //mm = (mm*453)>>15;
        t3=((t2 * 9)/5 + 32);
        //t3=(t1-32)*.5556;
        Serial.print("fhrn: ");
        Serial.println(t3);
        break;       
//case 0x23:
        //mm = (mm*15)>>15;
       // mm4=mm;
       // Serial.print("ipv: ");//inst phse vtn
        //Serial.println(mm4);
       // break;           
//case 0x24:
       // mm = (mm*1135*50)>>15;
        //mm5=mm;
        //Serial.print("input_power: ");
        //Serial.println(mm5);
        //break;   
//case 0x25:
       // mm = (mm*453*15)>>15;
        //mm6=mm;
        //Serial.print("output_power: ");
       // Serial.println(mm6);
       // break; 
//case 0x26:
       // mm = (mm*15)>>15;
        //mm7=mm;
        //Serial.print("kwh: ");
        //Serial.println(mm7);
        //break;     

}
          
  check_sum = (rx[count-2]<<8) | rx[count-1];
  calc_checksum = ModRTU_CRC(rx,(count-2));

  Serial.print("received checksum: ");
  Serial.println(check_sum,HEX);

  Serial.print("calculated checksum: ");
  Serial.println(calc_checksum,HEX);

    if(check_sum == calc_checksum)
       Serial.println("CRC Matched");  
    else
       Serial.println("CRC Mismatch");

     
       delay(1000);              //between each query a delay of 1seconds
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.print("connecting to ");
  Serial.println(host);                          //defined upside :- host = devapi2.thethingscloud.com or 139.59.26.117

///////////////////////////////////// TIMESTAMP CODE SNIPPET /////////////////////////
Serial.println("inside get timestamp\n");
  if (!client.connect(time_server, httpPort)) 
  {
    return;                                                        //*-*-*-*-*-*-*-*-*-*
  }

  client.println("GET /api?timestamp=now HTTP/1.1");                            //Whats this part doing, i didnt get
  client.println("Host: www.convert-unix-time.com");
  client.println("Cache-Control: no-cache");
  client.println("Postman-Token: ea3c18c6-09ba-d049-ccf3-369a22a284b8");
  client.println();\
 

GiveMeTimestamp();                        //it'll call the function which will get the timestamp response from the server
//.Serial.println("timestamp receieved");
Serial.println(timestamp);
///////////////////////////////////////////////////////////////////////////////

// Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort))            //port = 80
  {
    Serial.println("connection failed");                  // *-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-
    return;
  }
  
//String login_data = "{\"email\":\"" + String(user_name) + "\",\"password\":\"" + String(pwd) + "\"}";                              //i think some quote problem, how can you print ""

//Serial.print("login data :");
//Serial.println(login_data);

//////////////////////////////////////Login to the thingscloud account ////////////////////////  
  
  client.print("POST ");
 // client.print(login_url);                            //login_url = "/api/v2/user/session"
  client.println(" HTTP/1.1");
  client.print("host: ");
  client.println(host);                               //host = devapi2.thethingscloud.com
  client.println("Content-Type: application/json");
  client.println("Cache-Control: no-cache");
  client.print("Content-Length: ");
 // client.println(login_data.length());
  client.println();
 // client.println(login_data);                         //login_data = "{\"email\":\"" + String(user_name) + "\",\"password\":\"" + String(pwd) + "\"}"; 

/////////////////////////logged in and now receive the response //////////////////////////

  unsigned long timeout = millis();
  while (client.available() == 0) 
  {
    if (millis() - timeout > 50000) 
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Response From Server");
  while(client.available())                                                 //what all you get from the server print them all thru this loop
  {
    String line = client.readStringUntil('\r');
    Serial.println(line);                               
        int pos = line.indexOf("token");
         //Serial.println(pos);          //indexOf() is a funtion to search for smthng , it returns -1 if not found
          
        if (pos >= 0)
        {
         int j = 0;
          while (line[pos + 8 + j] != 34)         //why 34?? , bcoz ASCII value of " is 34 . and "line[pos + 8 + j]" is considering characters not position.
          {
            ses_id[j] = line[pos + 8 + j];
            j = j + 1;
          }
          ses_id[j + 1] = '\0';

         }
 }
  Serial.println("***********************************************");
  Serial.println(ses_id);
  Serial.println("***********************************************");
  Serial.println(timestamp);
  Serial.println("***********************************************");
  Serial.println();
  Serial.println("closing connection");

  Serial.println("inside ThingsCloudPost");

 
  String PostValue = "{\"device_id\": 201840, \"slave_id\": 2";
         PostValue = PostValue + ",\"dts\":" +timestamp;
   PostValue = PostValue +",\"data\":{\"celc\":" + t2 +",\"fahr\":" + t3 +"}"+"}";
  
  
  Serial.println(PostValue);

//////////////////////////////// try to connect to server again and POST the data on the cloud //////////////////////////
  if (!client.connect(host, httpPort)) 
  {
    return;
  }

  client.print("POST ");
  client.print(post_url);               /// POST api/device-datas         HTTP/1.1     Host: baas.thethingscloud.com      Content-Type: application/json        Cache-Control: no-cache
  //client.println(" HTTP/1.1");           //Authorization:          Content-Length: 
  client.print("Host: ");
  client.println(host);
  client.println("Content-Type: application/json");
  client.println("Cache-Control: no-cache");
  client.print("Authorization:Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.IjViMjBhN2UyODQ2YjFhNDBiMmIxYTFlZiI.5xoErxmyV5sCc3JYBHwpwXnfH6Mk7S9XVVWqvwDB9Cs ");
  client.println(ses_id);
  client.print("Content-Length: ");
  client.println(PostValue.length());
  client.println();
  client.println(PostValue);
//////////////////////////////////POSTING the data on to the cloud is done and now get the response form cloud server//////////////////
  Serial.println("Response From Server");
  while(client.available())
  {
    
    String line2 = client.readStringUntil('\r');
     
    Serial.print(line2);  
    
  }
    
Serial.println("//////////////////////    THE END     /////////////////////");
client.stop();

delay(3000);
}




