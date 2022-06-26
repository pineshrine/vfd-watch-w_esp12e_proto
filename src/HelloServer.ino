#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "RTClib.h"
#include "NTPClient.h"
#include <WiFiUdp.h>
#include "ShiftRegister74HC595.h"

//595
//ShiftRegister74HC595<1> sr(5, 6, 7);
ShiftRegister74HC595<1> sr(14, 12, 13);


// rtc
RTC_DS1307 rtc;

//wifi temp
const char* ssid = "matsu_11"; 
const char* password = "password";

//ESP8266WebServer server(80);

//const int led = 2;
//const int coron = 8;
const int coron = 15;
const int tc4511_le = 0;

//ntp
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,32400);

// void handleRoot() {
//   //digitalWrite(led, 1);
//   //DateTime time = rtc.now();
//   //timeClient.update();
//   char* ntpepoch = "";
//   char* rtcepoch = "";
//   ltoa((signed long)(timeClient.getEpochTime()),ntpepoch,10);
//   //String rtcepoch = time.timestamp(DateTime::TIMESTAMP_FULL);
//   //ltoa((long)time.unixtime(),rtcepoch,10);
//   String message = "RTC:";
//   //message += rtcepoch;
//   message += "\nNTP:";
//   message += ntpepoch;
//   message += "\n\n";
//   server.send(200, "text/plain", message);
//   //digitalWrite(led, 1);
// }

// void handleNotFound(){
//   //digitalWrite(led, 1);
//   String message = "File Not Found\n\n";
//   message += "URI: ";
//   message += server.uri();
//   message += "\nMethod: ";
//   message += (server.method() == HTTP_GET)?"GET":"POST";
//   message += "\nArguments: ";
//   message += server.args();
//   message += "\n";
//   for (uint8_t i=0; i<server.args(); i++){
//     message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//   }
//   server.send(404, "text/plain", message);
// }



void setup(void){
  //pinMode(led, OUTPUT);
  pinMode(coron, OUTPUT);
  pinMode(tc4511_le, OUTPUT);
  //digitalWrite(led, 0);
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
  Serial.print("IP address: ");
  delay(500);
  Serial.println(WiFi.localIP());

  // if (MDNS.begin("esp8266",WiFi.localIP())) {
  //   Serial.println("MDNS responder started");
  // }

  // server.on("/", handleRoot);

  // server.on("/inline", [](){
  //   server.send(200, "text/plain", "this works as well");
  // });

  // server.onNotFound(handleNotFound);

  // server.begin();
  // Serial.println("HTTP server started");
  // MDNS.addService("http", "tcp", 80);

  timeClient.begin();
  timeClient.update();
  delay(500);
  timeClient.update();
  delay(500);
  Serial.println(timeClient.getFormattedTime());
  //unsigned long ntpepoch = timeClient.getEpochTime();

  //rtc.adjust((uint32_t)ntpepoch);
  //Serial.println("rtc synced from ntp.");

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

}

void disp_clock(void){

  DateTime time = rtc.now();
  String ntptime = timeClient.getFormattedTime();
  //int hhmm = time.hour() * 100 + time.minute();
  // int hh = time.hour();
  // int mm = time.minute();
  // int blink = time.second() % 2;
  String hhs = "";
  String hms = "";
  String hss = "";
  hhs = ntptime.substring(0,2);
  hms = ntptime.substring(3,5);
  hss = ntptime.substring(6,8);
  int hh = hhs.toInt();
  int mm = hms.toInt();
  int blink = hss.toInt() % 2;
  
  int usb[4];
  usb[0] = hh/10;
  usb[1] = hh%10;
  usb[2]= mm/10;
  usb[3] = mm%10;
  //Serial.print(usb[0]);Serial.print(usb[1]);Serial.print(usb[2]);Serial.print(usb[3]);Serial.print("\r");
  Serial.println(hh);Serial.println(mm);Serial.println(hss.toInt());
  //Serial.println();
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
  //MDNS.update();
  // server.handleClient();
  disp_clock();
  //delay(10);
}
