/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "Kandatz Testing"
#define APPSK  "12345678"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

// Untuk Bagian control relay
uint8_t sprayer1pin = 14;
uint8_t sprayer2pin = 12;

// Untuk status Button
bool sprayer1status = LOW;
bool sprayer2status = LOW;

// untuk pembacaan sensor ultrasonic
//const int trigP1 = 16;  //SD3 Or GPIO-10 of nodemcu
//const int echoP1 = 5;  //SD2 Or GPIO-9 of nodemcu
const int trigP1 = 4;  //SD3 Or GPIO-10 of nodemcu
const int echoP1 = 0;  //SD2 Or GPIO-9 of nodemcu
long duration1;
int distance1;

const int trigP2 = 16;  //SD3 Or GPIO-10 of nodemcu
const int echoP2 = 5;  //SD2 Or GPIO-9 of nodemcu
long duration2;
int distance2;

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handle_OnConnect() {
  sprayer1status = LOW;
  sprayer2status = LOW;
  Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(sprayer1status,sprayer2status)); 
}

void handle_sprayer1on() {
  sprayer1status = HIGH;
  Serial.println("GPIO7 Status: ON");
  server.send(200, "text/html", SendHTML(true,sprayer2status)); 
}

void handle_sprayer1off() {
  sprayer1status = LOW;
  Serial.println("GPIO7 Status: OFF");
  server.send(200, "text/html", SendHTML(false,sprayer2status)); 
}

void handle_sprayer2on() {
  sprayer2status = HIGH;
  Serial.println("GPIO6 Status: ON");
  server.send(200, "text/html", SendHTML(sprayer1status,true)); 
}

void handle_sprayer2off() {
  sprayer2status = LOW;
  Serial.println("GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(sprayer1status,false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void ultrasonic_read(){
  digitalWrite(trigP1, LOW);   // Makes trigPin low
  delayMicroseconds(2);       // 2 micro second delay 
  digitalWrite(trigP1, HIGH);  // tigPin high
  delayMicroseconds(10);      // trigPin high for 10 micro seconds
  digitalWrite(trigP1, LOW);   // trigPin low
  duration1 = pulseIn(echoP1, HIGH);   //Read echo pin, time in microseconds
  distance1= duration1*0.034/2;        //Calculating actual/real distance

  digitalWrite(trigP2, LOW);   // Makes trigPin low
  delayMicroseconds(2);       // 2 micro second delay 
  digitalWrite(trigP2, HIGH);  // tigPin high
  delayMicroseconds(10);      // trigPin high for 10 micro seconds
  digitalWrite(trigP2, LOW);   // trigPin low
  duration2 = pulseIn(echoP2, HIGH);   //Read echo pin, time in microseconds
  distance2= duration2*0.034/2;        //Calculating actual/real distance
  
  Serial.print("Distance1 = ");        //Output distance on arduino serial monitor 
  Serial.print(distance1);  
  Serial.print("\t\tDistance2 = ");        //Output distance on arduino serial monitor 
  Serial.println(distance2);  
}

String SendHTML(uint8_t sprayer1stat,uint8_t sprayer2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>sprayer Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Covid-19 Sprayer Automatic System</h1>\n";
  ptr +="<h3>Terminal Control for maintenance relay motor sprayer</h3>\n";
  
   if(sprayer1stat)
  {ptr +="<p>Auto Spray Status: ON</p><a class=\"button button-off\" href=\"/sprayer1off\">OFF</a>\n";}
  else
  {ptr +="<p>Auto Spray Status: OFF</p><a class=\"button button-on\" href=\"/sprayer1on\">ON</a>\n";}

  if(sprayer2stat)
  {ptr +="<p>Spray Tester Status: ON</p><a class=\"button button-off\" href=\"/sprayer2off\">OFF</a>\n";}
  else
  {ptr +="<p>Spray Tester Status: OFF</p><a class=\"button button-on\" href=\"/sprayer2on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  pinMode(A0, INPUT); 
  pinMode(sprayer1pin, OUTPUT);
  pinMode(sprayer2pin, OUTPUT);
  pinMode(trigP1, OUTPUT);  // Sets the trigPin1 as an Output
  pinMode(echoP1, INPUT);   // Sets the echoPin1 as an Input
  
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress Ip(192, 168, 1, 1);
  IPAddress NMask(255, 255, 255, 0);


  WiFi.softAPConfig(Ip, Ip, NMask);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
//  server.on("/", handleRoot);
  server.on("/", handle_OnConnect);
  server.on("/sprayer1on", handle_sprayer1on);
  server.on("/sprayer1off", handle_sprayer1off);
  server.on("/sprayer2on", handle_sprayer2on);
  server.on("/sprayer2off", handle_sprayer2off);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {
  server.handleClient();
  if(sprayer1status)
  {digitalWrite(sprayer1pin, HIGH);
   ultrasonic_read();
  }
  else
  {digitalWrite(sprayer1pin, LOW);}
  
  if(sprayer2status)
  {digitalWrite(sprayer2pin, HIGH);}
  else
  {digitalWrite(sprayer2pin, LOW);}  

//  Serial.println("testing");
//  delay(100);
}
