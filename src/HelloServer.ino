#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "RTClib.h"
#include "NTPClient.h"
#include <WiFiUdp.h>
#include "ShiftRegister74HC595.h"

//595
ShiftRegister74HC595<1> sr(14, 12, 13);


// rtc
RTC_DS1307 rtc;

//wifi temp
const char* ssid = "matsu_11"; 
const char* password = "password";

const int coron = 15;
const int tc4511_le = 0;

//ntp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,32400);

void setup(void){
  pinMode(coron, OUTPUT);
  pinMode(tc4511_le, OUTPUT);
  digitalWrite(coron, 0);
  digitalWrite(tc4511_le, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < 10; i++)
    {
      delay(500);
      Serial.print(".");
    }
    break;
  }
  Serial.println("");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.println("IP address: ");
  delay(500);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    if (! rtc.isrunning()) {
      Serial.println("RTC is NOT running, let's set the time!");
      // When time needs to be set on a new device, or after a power loss, the
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(WiFi.localIP());
    timeClient.begin();
    timeClient.update();
    delay(500);
    if(timeClient.isTimeSet())
    {
      delay(500);
      Serial.println(timeClient.getFormattedTime());
      rtc.adjust(DateTime(timeClient.getEpochTime()));
    }else{
      Serial.println("unable to get ntp date.");
    }
  }else{
    timeClient.end();
  }
  

}

void disp_clock(void){

  DateTime time = rtc.now();
  int hh = time.hour();
  int mm = time.minute();
  int blink = time.second() % 2;
  
  int usb[4];
  usb[0] = hh/10;
  usb[1] = hh%10;
  usb[2]= mm/10;
  usb[3] = mm%10;
  //Serial.println(hh);Serial.println(mm);Serial.println(hss.toInt());
  Serial.print(time.timestamp()+"\r");
  for (size_t i = 0; i < 4; i++)
  {
    sr.setNoUpdate(4,LOW);sr.setNoUpdate(5,LOW);sr.setNoUpdate(6,LOW);sr.setNoUpdate(7,LOW);
    sr.setNoUpdate(i+4,HIGH);
    if(usb[i] & 0b0001){
      sr.setNoUpdate(0,HIGH);
    }else{
      sr.setNoUpdate(0,LOW);
    }
    if(usb[i] & 0b0010){
      sr.setNoUpdate(1,HIGH);
    }else{
      sr.setNoUpdate(1,LOW);
    }
    if(usb[i] & 0b0100){
      sr.setNoUpdate(2,HIGH);
    }else{
      sr.setNoUpdate(2,LOW);
    }
    if(usb[i] & 0b1000){
      sr.setNoUpdate(3,HIGH);
    }else{
      sr.setNoUpdate(3,LOW);
    }
    if (blink)
    {
      digitalWrite(coron, 1);
    }else{
      digitalWrite(coron, 0);
    }
    digitalWrite(tc4511_le, 0);
    sr.updateRegisters();
    digitalWrite(tc4511_le, 1);
    delay(1);
  }
  
}

void loop(void){
  disp_clock();
}
