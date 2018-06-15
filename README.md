## Getting started with ThingsIO.AI

1. Register the account in http://thingsio.ai/#/register.
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_m1f8581dd.png "Register the account")
2. Create a project:
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_m4d897fd5.png "Create a project")
3. Write the name of the project:
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_m33ab5c79.png "Write the name of the project")
4. Now, you will be on the project dashboard of your account. Click on the new project option:
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_15ba7b54.png "Project Dashboard")
5. Click on the add a new device option:
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_15ba7b54.png "New Device")
6. Enter the device name and click on the create and configure device:
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_7c55cf30.png "Device Name")
7. You will be on your device dashboard:
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_m78b16072.png "device dashboard")

* Track your device’s special parameters here. This can be set in device configuration (default to null).
* You can see your real time and special parameter’s graph.
* You can see your all data points.
* You can see here your last 5 data points.
* You can see here all the list of created graphs.

8. Go in sample device code options and click on the CC3200. You will get the sample code from there and paste into your CCS.
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_67e2f411.png "device dashboard")
9. Click on the “Send trial data” to send a trial data to the server:
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_m3b9bdbc7.png "Send trial data")
10. Now, click on the configuration device option.
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_41da8116.png "configuration device")
11. You can set the special parameters (this is tracked on the device dashboard) & transform it accordingly and add new parameters (Parameters are updated automatically as you send them from your device).
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_2a97afea.png "special parameters")
12. Click on the update device option:
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_m59124fa6.png "update device option")
13. You can also set your device geographical location.
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_6b115ebe.png "device geographical location")
14. To create a graph for your device click on the “create graph option”.
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_2902f544.png "create graph option")
15. Click on the preview option. You will see your created graph and click save changes.
![alt text](https://s3.ap-south-1.amazonaws.com/thingsio-tuts/tutorial_html_m2c1996f2.png "graph") 
 

## Platform: Windows
## TEMPERATURE SENSOR WITH NODE MCU
## IDE: ARDUINO
## CHIP: ESP8266 MOD WiFi  MODULE
1.LIBRARIES INCLUDED
a)  ```#include <SoftwareSerial.h>```
Install arduino IDE 
Go to sketch>include library>library manager>search software serial>install
Checkout the  link provided for the detailed video on how to setup Arduino tool chain and libary for ESP8266:  https://youtu.be/ftO-_nfBBZk
b) ```#include <ESP8266WiFi.h> ```
(https://github.com/esp8266/Arduino  )
c) ``` #include <DNSServer.h>```
Go to sketch>include library>library manager>search DNSserver>install
d)``` #include <ESP8266WebServer.h>```
Go to sketch>include library>library manager>search >install
e)``` #include "WiFiManager.h"   ``` 
(https://github.com/tzapu/WiFiManager)
      2. CODE MODIFICATIONS
a) define the host 
       		    ```const char* host = "api.thingsio.ai";```
b) define the post url
       	```	    const char* post_url = "/devices/deviceData";```
       3. TEMPERATURE SENSING AND CONVERSION
    
``` 
switch(qry[3])
          {
          1) case 0x20:
         t1=(analogValue/1024.0) * 3300;      (analogValue = analogRead(outputpin),
         (outputpin= A0);
          Serial.print("temp: ");
          Serial.println(t1);
          break;  
       2) case 0x21:
         do the following  to obtain the Celsius value,
          t2=t1/10; 
          Serial.print("celc: ");
          Serial.println(t2);
          break;   
       3) case 0x22:
          t3=((t2 * 9)/5 + 32);
         celcius to Fahrenheit conversion
         Serial.print("fhrn: ");
         Serial.println(t3);
         break;
// *You can modify the code as per the requirements and the sensor used

         provide the device id as well as the slave id
         String PostValue = "{\"device_id\": 201840, \"slave_id\": 2";
         Post the values 
         PostValue = PostValue + ",\"dts\":" +timestamp;
         PostValue = PostValue +",\"data\":{\"celc\":" + t2 +",\"fahr\":" + t3 +"}"+"}";
//Celc  temperature in celcius
//Fahrtemperature in Fahrenheit
```


 