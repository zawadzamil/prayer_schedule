#include <Arduino.h>
#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino
#include <EEPROM.h>
#include "WiFi.h"
#include <WiFiUdp.h>
#include <time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <typeinfo>



#define successpin 2
#define failedpin 17
#define matchpin 16

#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20        /* Time ESP32 will go to sleep (in seconds) */



// create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

BluetoothSerial ESP_BT; //Object for Bluetooth
String buffer_in;
unsigned long previousMillis = 0; 
byte val;       
int addr = 0;
byte indS=0;
byte indP=0;
byte indL=0;
String stream;
byte len=0;
String temp;
String temp2;
String temp3;
unsigned int interval=40000;
String city;

int GMTOffset = 21600;  //Replace with your GMT Offset in seconds
int daylightOffset = 0;  // Replace with your daylight savings offset in seconds
 

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}




boolean check_wifiUpdate()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
   

    Serial.println("40 Seconds Over");

    return true;
  }

    else if (ESP_BT.available()) //Check if we receive anything from Bluetooth
  {interval=50000;
    buffer_in = ESP_BT.readStringUntil('\n'); //Read what we recevive 
    //Serial.println("Received:"); Serial.println(buffer_in);
 delay(20);
 

 if(buffer_in.charAt(0)=='S')
{
 for(int i=0;i<buffer_in.length();i++)
 {val=(byte)(buffer_in.charAt(i));
 //Serial.println("val "+val);

  EEPROM.write(addr, val);
  //Serial.println(val);
   addr++;
}
//Serial.print("New ");
//Serial.print(buffer_in);
EEPROM.write(addr, 10);
addr++;
EEPROM.commit();     
   ESP_BT.println("SSID Stored");
  }

 else if(buffer_in.charAt(0)=='P')
{
 for(int i=0;i<buffer_in.length();i++)
 {val=(byte)(buffer_in.charAt(i));
// Serial.println("val "+val);
  EEPROM.write(addr, val);
//  Serial.println(val);
  addr++;
  
}
//Serial.print("New ");
//Serial.print(buffer_in);
EEPROM.write(addr, 10);
addr++;
EEPROM.commit();  
ESP_BT.println("Password Stored"); 

  } 
  else if(buffer_in.charAt(0)=='C')
{
 for(int i=0;i<buffer_in.length();i++)
 {val=(byte)(buffer_in.charAt(i));
 //Serial.println("val "+val);

  EEPROM.write(addr, val);
  //Serial.println(val);
   addr++;
}
//Serial.print("New ");
////Serial.print(buffer_in);
EEPROM.write(addr, 10);
addr++;
EEPROM.commit();  
ESP_BT.println("Location Stored");
return true;
  }
  
 
  return false;
  }
else
{ return false;
  }
}
  


StaticJsonDocument<1536> doc;
void setup() {
EEPROM.begin(50);
Serial.begin(9600); //Start Serial monitor in 9600
pinMode(successpin,OUTPUT);
pinMode(failedpin,OUTPUT);
pinMode(matchpin,OUTPUT);

// initialize OLED display with I2C address 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }
  delay(2000);
  oled.clearDisplay(); // clear display

  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0,0);       // set position to display
  oled.println("Connecting"); // set text
  oled.display();

Serial.println("Bluetooth Device is Ready to Pair");

Serial.println("Waiting For Wifi Updates 40 seconds");
ESP_BT.begin("ESP32_BLUETOOTH"); //Name of your Bluetooth Signal


// Wakeup timer set after deep sleep
 esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
 

while(!check_wifiUpdate()==true)
{
}


    
Serial.println("The Stored Wifi credetial are : ");
for(int i=0;i<50;i++)
 {val=EEPROM.read(i);
 stream+=(char)val;
if((val==10) && (indS==0))
{
  indS=i;
//Serial.println("indS"+(String)i);
}
  else if(val==10 && indP==0)
  {
    indP=i;
   
//Serial.println("indP"+(String)i);
  }
    else if(val==10 && indL==0)
  {
    indL=i;
    break;
  }
    
}


// Serial.println(stream);
// Serial.println("Stream Ended");
 temp=stream.substring(0,indS);
 temp=temp.substring(5,indS);


//ssid2=ssid;
temp2=stream.substring(indS+1,indP);
temp2=temp2.substring(5,indP-indS);

//pass2=city;
temp3=stream.substring(indP+1,indL);
temp3=temp3.substring(5,indL-indP);



  


int i=temp.length();
int j=temp2.length();
int k=temp3.length();
char ssid[i];
char pass[j]; 
char loct[k]; 
temp.toCharArray(ssid,i);
temp2.toCharArray(pass,j);
temp3.toCharArray(loct,k);
city = loct;




  Serial.println("Stored SSID");
  Serial.println(ssid);
  Serial.println("Stored PASS");
  Serial.println(pass);
  Serial.println("Stored Location");
  Serial.println(loct);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, pass);
delay(1000);
 oled.clearDisplay();
 oled.println("Connecting..");
 oled.display();

 if (WiFi.waitForConnectResult() != WL_CONNECTED) {
   oled.clearDisplay();
 oled.println("Failed!");
 oled.display();
        Serial.println("WiFi Failed");
         digitalWrite(failedpin,HIGH);
          digitalWrite(successpin,LOW);
        while(1) {
            delay(1000);
        }
    }
    else 
    {
      Serial.print("Wifi Connected to ");
      Serial.println(ssid);
       digitalWrite(failedpin,LOW);
      digitalWrite(successpin,HIGH);
     configTime(GMTOffset, daylightOffset, "pool.ntp.org","time.nist.gov");
        oled.clearDisplay();
        oled.println("Connected!");
        oled.display();
        delay(2000);
      HTTPClient http;
      String serverPath = "https://muslimsalat.com/"+city+"/daily.json?key=d2d7f0a9ac2fbc7c0ab53535a40dff96";
        // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      delay(5000);

      // Send HTTP GET request
       int httpResponseCode = http.GET();
       if (httpResponseCode>0)
        {          
          String payload = http.getString();
          DeserializationError error = deserializeJson(doc, payload);
          if (error) 
            {
              Serial.print("deserializeJson() failed: ");
              Serial.println(error.c_str());
              return;
            }

        }


        else 
        {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
      // Free resources
      http.end();

}

}




void loop() {
  time_t rawtime = time(nullptr);
struct tm* timeinfo = localtime(&rawtime);

// Serial.print("Time: ");
// Serial.print(timeinfo->tm_hour);
// Serial.print(":");
// Serial.print(timeinfo->tm_min);
// Serial.print(":");
// Serial.println(timeinfo->tm_sec);

oled.clearDisplay();
oled.setTextSize(3);
oled.setTextColor(WHITE);
oled.setCursor(0,10);
if((timeinfo->tm_hour)<10)
oled.print("0");
oled.print(timeinfo->tm_hour);
oled.print(":");
if( timeinfo->tm_min <10)
oled.print("0");
oled.print(timeinfo->tm_min);

oled.setTextSize(2);
oled.setCursor(90,15);
oled.print(":");
if( timeinfo->tm_sec <10)
oled.print("0");
oled.print(timeinfo->tm_sec); 
oled.display();

int status_valid = doc["status_valid"];
//Serial.print(status_valid);
if(status_valid == 0)
{
    oled.setTextSize(1);
  oled.setCursor(25,40);
  oled.print("Invalid City!");
  oled.display();
  Serial.println("Wrong City! Please Enter Correnct City");



}
else{
  JsonObject items_0 = doc["items"][0];
const char* items_0_date_for = items_0["date_for"]; // "2022-2-16"
const char* items_0_fajr = items_0["fajr"]; // "5:14 am"
const char* items_0_shurooq = items_0["shurooq"]; // "6:27 am"
const char* items_0_dhuhr = items_0["dhuhr"]; // "12:12 pm"
const char* items_0_asr = items_0["asr"]; // "4:17 pm"
const char* items_0_maghrib = items_0["maghrib"]; // "5:57 pm"
const char* items_0_isha = items_0["isha"]; // "7:09 pm"

  









char *ptr;
  int i1;
  int i2;
  int i3;
   ptr = strchr(items_0_date_for, '-');
    *ptr = '\0';
    i1 = atoi(&items_0_date_for[0]);
    *ptr = '-';
    ptr++;
    
   
    i2 = atoi(ptr);
    *ptr = '-';
    
    ptr++;
    ptr++;
   
    
   // This One mainly Needed
     i3 = atoi(ptr);

    oled.setTextSize(1);
  oled.setCursor(20,40);
  oled.print("Date: ");
  if((timeinfo->tm_mday) <10 )
  oled.print("0");
  oled.print(timeinfo->tm_mday);
  oled.print("/");
  if((timeinfo->tm_mon) <10 )
  oled.print("0");
  oled.print((timeinfo->tm_mon)+1);
  oled.print("/");
  oled.print(i1);
  oled.setCursor(25,52);
  oled.print("City: ");
  oled.print(city);
  oled.display();

   
    //

     // Convert fajr char to Integer
    char *fajar ;
    int fajrHour;
    int fajrMin;
     fajar = strchr(items_0_fajr, ':');
     *fajar = '\0';
      fajrHour = atoi(&items_0_fajr[0]);
    *fajar = ':';
    fajar++;
    fajrMin = atoi(fajar);
    
   

    //

    // Convert Dhuhr char to Integer
    char *dhuhrString ;
    int dhuhrHour;
    int dhuhrMin;
     dhuhrString = strchr(items_0_dhuhr, ':');
     *dhuhrString = '\0';
      dhuhrHour = atoi(&items_0_dhuhr[0]);
    *dhuhrString = ':';
    dhuhrString++;
    dhuhrMin = atoi(dhuhrString);
    
   
    //

    // Convert Asr char to Integer
    char *asrString ;
    int asrHour;
    int asrMin;
     asrString = strchr(items_0_asr, ':');
     *asrString = '\0';
      asrHour = atoi(&items_0_asr[0]);
    *asrString = ':';
    asrString++;
    asrMin = atoi(asrString);
    //

     // Convert Maghrib char to Integer
    char *maghribString ;
    int maghribHour;
    int maghribMin;
     maghribString = strchr(items_0_maghrib, ':');
     *maghribString = '\0';
      maghribHour = atoi(&items_0_maghrib[0]);
    *maghribString = ':';
    maghribString++;
    maghribMin = atoi(maghribString);
    //

    

     // Convert Isha char to Integer
    char *ishaString ;
    int ishaHour;
    int ishaMin;
     ishaString = strchr(items_0_isha, ':');
     *ishaString = '\0';
      ishaHour = atoi(&items_0_isha[0]);
    *ishaString = ':';
    ishaString++;
    ishaMin = atoi(ishaString);

    
    //

    ////////Checking If Current time Matches With Prayer Time ///////

    if(i3 == timeinfo->tm_mday) //day matched
    {
      
       


         if( timeinfo->tm_hour == fajrHour && timeinfo->tm_min ==  fajrMin) //fajr time matched
         {
             
              digitalWrite(matchpin,HIGH);
              delay(61000);
           
         }
        else if( (timeinfo->tm_hour - 12) == dhuhrHour && timeinfo->tm_min ==  dhuhrMin) //dhuhr time matched
         {
         
            digitalWrite(matchpin,HIGH);
           delay(61000);
           
           
         }
        else if ( (timeinfo->tm_hour - 12) == asrHour && timeinfo->tm_min ==  asrMin) //asr time matched
         {
         
            digitalWrite(matchpin,HIGH);
           delay(61000);
           
         }
        else if( (timeinfo->tm_hour - 12) == maghribHour && timeinfo->tm_min == maghribMin )  //maghrib time matched
         {
           
            digitalWrite(matchpin,HIGH);
           delay(61000);

         }
        else if( (timeinfo->tm_hour - 12) == ishaHour &&  timeinfo->tm_min == ishaMin) //isha time matched
         {
          
            digitalWrite(matchpin,HIGH);
           delay(61000);

         }
         else{
            digitalWrite(matchpin,LOW);
          
         }
    }

}
 


    if(timeinfo->tm_hour == 3 && timeinfo ->tm_min == 00 && timeinfo->tm_sec == 57)
    {
        Serial.println("Going to sleep now");
      Serial.flush(); 
      esp_deep_sleep_start();
       
    }
 
delay(1000); 
  
  
}
