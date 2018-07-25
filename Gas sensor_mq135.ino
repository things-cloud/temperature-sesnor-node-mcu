#include <SoftwareSerial.h>
SoftwareSerial mySerial(12, 13); // RX, TX

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

int count=0,i,m,j,k;

int outputpin= A0;
int s1, s2; //mq135 sensor is used here




//////////////////////////////////////// ALL DECLARATIONS for CLOUD //////////////////////////////
const char* host = "api.thingsio.ai";                                 // OR host = devapi2.thethingscloud.com
const char* post_url = "/devices/deviceData";       // OR /api/v2/thingscloud2/_table/data_ac
const char* time_server = "www.convert-unix-time.com";             //this is to convert timestamp
const int httpPort = 80;

char timestamp[10];

WiFiClient client;
  


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
  //Serial.begin(115200);     //(19200,SERIAL_8E1) - data size = 8 bits , parity = Even , stop bit =  1bit
mySerial.begin(9600);

  Serial.begin(9600);      // sets the serial port to 9600

  pinMode( 0, INPUT);







  WiFiManager wifiManager;
 
  wifiManager.setAPCallback(configModeCallback);
                                                                                                    
  if(!wifiManager.autoConnect("abcd","*****"))                   //wifiManager.autoConnect("AP-NAME", "AP-PASSWORD"); (OR) wifiManager.autoConnect("AP-NAME"); only ID no password (OR) wifiManager.autoConnect(); this will generate a ID by itself
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
{
 
  
/////////////////////////////////////// SEND THE QUERY AND RECEIVE THE RESPONSE///////////////////////  


  s1 = analogRead(0);       // read analog input pin 0

  s2 = digitalRead(0); //digtl

  Serial.println(s1,DEC);  // prints the value read

  Serial.println(s2,DEC);

  delay(2000);                        // wait 100ms for next reading

}

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
  client.println();

GiveMeTimestamp();                        //it'll call the function which will get the timestamp response from the server
Serial.println("timestamp receieved");
Serial.println(timestamp);
///////////////////////////////////////////////////////////////////////////////

// Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort))            //host = devapi2.thethingscloud.com , port = 80
  {
    Serial.println("connection failed");                  // *-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-
    return;
  }
  
  client.print("Content-Length: ");
  client.println(); 

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
  
  Serial.println("***********************************************");
  Serial.println();
  Serial.println("closing connection");

  Serial.println("inside ThingsCloudPost");

 
  String PostValue = "{\"device_id\": 201869, \"slave_id\": 2";
         PostValue = PostValue + ",\"dts\":" +timestamp;
   PostValue = PostValue +",\"data\":{\"decs\":" + s1 +",\"decd\":" + s2+"}"+"}";
  
  
  Serial.println(PostValue);

//////////////////////////////// try to connect to server again and POST the data on the cloud //////////////////////////
  if (!client.connect(host, httpPort)) 
  {
    return;
  }

  client.print("POST ");
  client.print(post_url);               /// POST api/device-datas         HTTP/1.1     Host: baas.thethingscloud.com      Content-Type: application/json        Cache-Control: no-cache
  client.println(" HTTP/1.1");           //Authorization:          Content-Length: 
  client.print("Host: ");
  client.println(host);
  client.println("Content-Type: application/json");
  client.println("Cache-Control: no-cache");
  client.print("Authorization:Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.IjViMjM4NjE1YTNhN2NiNTA5ZTA0NDYwMCI.n5O_tRfqBbfzRfonK7CU8W8s-apj0ZHeTKU5T6OzXqA ");
  client.println();
  client.print("Content-Length: ");
  client.println(PostValue.length());
  client.println();
  client.println(PostValue);
  
//////////////////////////////////POSTING the data on to the cloud is done and now get the response form cloud server//////////////////
  while (client.available() == 0) 
  {
    if (millis() - timeout > 50000) 
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  Serial.println("Response From Server");
  while(client.available())
  { 
    String line2 = client.readStringUntil('\r');   
    Serial.print(line2);
    
  }
    
Serial.println("//////////////////////    THE END     /////////////////////");
delay(3000);
}
