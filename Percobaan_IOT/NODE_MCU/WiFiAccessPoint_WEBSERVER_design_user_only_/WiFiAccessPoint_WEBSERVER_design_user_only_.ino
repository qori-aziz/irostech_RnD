#include "lamp.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <stdio.h>
#include <DS1302.h>

#ifndef APSSID
#define APSSID "Auto Sprayer"
#define APPSK  "1234509876"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);

String errorInfo;

// Untuk Bagian control relay
const int sprayer1pin = 0;
//uint8_t sprayer2pin = 16;

const int ledGreen = 16;
//const int ledRed = 9;

LAMP blink_lamp(ledGreen);
//LAMP alarm(ledRed);


// Untuk status Button
bool sprayer1status = LOW;
bool sprayer2status = LOW;
bool sprayer3stat;

String inputMessage1; float im1;
String inputMessage2; float im2;
String inputMessage3; float im3;
String inputMessage4; float im4;
String inputMessage5; float im5;

// untuk pembacaan sensor ultrasonic
const int trigP1 = 14;  //SD3 Or GPIO-10 of nodemcu
const int echoP1 = 12;  //SD2 Or GPIO-9 of nodemcu
long duration1;
int distance1;

const int trigP2 = 13;  //SD3 Or GPIO-10 of nodemcu
const int echoP2 = 15;  //SD2 Or GPIO-9 of nodemcu
long duration2;
int distance2;
int state;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

char buft[50]; // print waktu

uint8_t tsjam, tsmnt, tsdtk;
uint8_t tbjam, tbmnt, tbdtk;
uint8_t setjam, setmnt, setdtk;

namespace {
const int kCePin   = 2;  // Chip Enable  - D4
const int kIoPin   = 4;  // Input/Output - D2
const int kSclkPin = 5;  // Serial Clock - D1

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Minggu";
    case Time::kMonday: return "Senin";
    case Time::kTuesday: return "Selasa";
    case Time::kWednesday: return "Rabu";
    case Time::kThursday: return "Kamis";
    case Time::kFriday: return "Jumat";
    case Time::kSaturday: return "Sabtu";
  }
  return "(unknown day)";
}

void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
//  char buft[50];
  snprintf(buft, sizeof(buft), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
//  Serial.println(buft);
  delay(10);
}

}  // namespace


void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handle_OnConnect() {  
//  sprayer1status = LOW;
//  sprayer2status = LOW;
//  Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
  errorInfo = "Selamat Datang";
  server.send(200, "text/html", SendHTML(sprayer1status,sprayer2status)); 
}

void handle_sprayer1on() {
  sprayer1status = HIGH;
  Serial.println("GPIO7 Status: ON");
  errorInfo = "mesin otomatis hidup succes!";
  server.send(200, "text/html", SendHTML(true,sprayer2status)); 
}

void handle_sprayer1off() {
  sprayer1status = LOW;
  Serial.println("GPIO7 Status: OFF");
  errorInfo = "mesin otomatis mati succes!";
  server.send(200, "text/html", SendHTML(false,sprayer2status)); 
}

void handle_sprayer2on() {
  sprayer2status = HIGH;
  Serial.println("GPIO6 Status: ON");
  errorInfo = "test mesin hidup succes!";
  server.send(200, "text/html", SendHTML(sprayer1status,true)); 
}

void handle_sprayer2off() {
  sprayer2status = LOW;
  Serial.println("GPIO6 Status: OFF");
  errorInfo = "test mesin mati succes!";
  server.send(200, "text/html", SendHTML(sprayer1status,false)); 
}

void handle_timeron() {
  sprayer3stat = HIGH;
  errorInfo = "mode waktu hidup succes!";
  server.send(200, "text/html", SendHTML(sprayer1status,sprayer2status)); 
}

void handle_timeroff() {
  // isi aksi berikut dg status
  sprayer3stat = LOW;
  errorInfo = "mode waktu mati succes!";
  server.send(200, "text/html", SendHTML(sprayer1status,sprayer2status)); 
}


void handle_input(){
  // Untuk Parameter Sensor
  if(server.argName(0) == "input1" && server.arg("input1") != "" && server.arg("input1").toFloat() <=400 ){
    inputMessage1 = server.arg("input1"); im1 = inputMessage1.toFloat();
    EEPROM.put(0,im1); EEPROM.commit();
    errorInfo = "Pengaturan Sensor 1 sukses!";
  }
  if(server.argName(0) == "input2" && server.arg("input2") != "" && server.arg("input2").toFloat() <=400 ){
    inputMessage2 = server.arg("input2"); im2 = inputMessage2.toFloat();
    EEPROM.put(10,im2); EEPROM.commit();
    errorInfo = "Pengaturan Sensor 2 sukses!";
  }
  if(server.argName(0) == "input3" && server.arg("input3") != "" && server.arg("input3").toFloat() <=400 ){
    inputMessage3 = server.arg("input3"); im3 = inputMessage3.toFloat();
    EEPROM.put(20,im3); EEPROM.commit();
    errorInfo = "Pengaturan Sensor 3 sukses!";
  }
  if(server.argName(0) == "input4" && server.arg("input4") != "" && server.arg("input4").toFloat() <=400 ){
    inputMessage4 = server.arg("input4"); im4 = inputMessage4.toFloat();
    EEPROM.put(30,im4); EEPROM.commit();
    errorInfo = "Pengaturan Sensor 4 sukses!";
  }

  // Untuk Parameter Waktu
  if(server.argName(0) == "input5" && server.arg("input5") != "" && server.arg("input5").toFloat() <=3600 ){
    inputMessage5 = server.arg("input5"); im5 = inputMessage5.toFloat();
    EEPROM.put(40,im5); EEPROM.commit();
    errorInfo = "Pengaturan timer semprot sukses!";
  }

  
  // Untuk parameter waktu operasional
  //mulai
  if(server.argName(0) == "tsjam" && server.arg("tsjam") != "" && server.arg("tsjam").toFloat() <=24 ){
    tsjam = server.arg("tsjam").toFloat();
    EEPROM.put(50,tsjam); EEPROM.commit();
    
  }
  else{errorInfo = "Pengaturan timer semprot Gagal!";}
  if(server.argName(0) == "tsmenit" && server.arg("tsmenit") != "" && server.arg("tsmenit").toFloat() <=60 ){
    tsmnt = server.arg("tsmenit").toFloat();
    EEPROM.put(55,tsmnt); EEPROM.commit();
    
  }
  else{errorInfo = "Pengaturan timer semprot Gagal!";}
  if(server.argName(0) == "tsdetik" && server.arg("tsdetik") != "" && server.arg("tsdetik").toFloat() <=60 ){
    tsdtk = server.arg("tsdetik").toFloat();
    EEPROM.put(60,tsdtk); EEPROM.commit();
    
  }
  else{errorInfo = "Pengaturan timer semprot Gagal!";}

  //akhir
  if(server.argName(0) == "tbjam" && server.arg("tbjam") != "" && server.arg("tbjam").toFloat() <=24 ){
    tbjam = server.arg("tbjam").toFloat();
    EEPROM.put(65,tbjam); EEPROM.commit();
    
  }
  else{errorInfo = "Pengaturan timer semprot Gagal!";}
  if(server.argName(0) == "tbmenit" && server.arg("tbmenit") != "" && server.arg("tbmenit").toFloat() <=60 ){
    tbmnt = server.arg("tbmenit").toFloat();
    EEPROM.put(70,tsmnt); EEPROM.commit();
    
  }
  else{errorInfo = "Pengaturan timer semprot Gagal!";}
  if(server.argName(0) == "tbdetik" && server.arg("tbdetik") != "" && server.arg("tbdetik").toFloat() <=60 ){
    tbdtk = server.arg("tbdetik").toFloat();
    EEPROM.put(75,tbdtk); EEPROM.commit();
  errorInfo = "Pengaturan timer semprot Sukses!";  
  }
  else{errorInfo = "Pengaturan timer semprot Gagal!";}
  

  
  
  server.send(200, "text/html", SendHTML(sprayer1status,sprayer2status));
 
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
  
//  Serial.print("Distance1 = ");        //Output distance on arduino serial monitor 
//  Serial.print(distance1);  
//  Serial.print("\t\tDistance2 = ");        //Output distance on arduino serial monitor 
//  Serial.println(distance2);  
}

String SendHTML(uint8_t sprayer1stat,uint8_t sprayer2stat){
  
  
  String ptr;

  ptr += "<!DOCTYPE html> <html>\n";
  
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>sprayer Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 40px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 10px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Covid-19 Sprayer Automatic System</h1>\n";

  //Tampilan Waktu
  ptr +="<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>waktu saat ini: ";
  ptr += buft;
  ptr += "\n";
  ptr += "</p>";

  //Tampilan Waktu
  ptr +="<p style='text-align: center;'><span style='color: #FF0000;'><strong style='font-size: small;'>Info: ";
  ptr += errorInfo;
  ptr += "\n";
  ptr += "</p>";

  //Tombol Aktifasi
  if(sprayer1stat)
  {ptr +="<p>Auto Spray Status: ON</p><a class=\"button button-off\" href=\"/sprayer1off\">OFF</a>\n";}
  else
  {ptr +="<p>Auto Spray Status: OFF</p><a class=\"button button-on\" href=\"/sprayer1on\">ON</a>\n";}

  if(sprayer2stat)
  {ptr +="<p>Spray Tester Status: ON</p><a class=\"button button-off\" href=\"/sprayer2off\">OFF</a>\n";}
  else
  {ptr +="<p>Spray Tester Status: OFF</p><a class=\"button button-on\" href=\"/sprayer2on\">ON</a>\n";}

  

  //Tombol refresh
  ptr +="<p></p><a class=\"button button-off\" href=\"/\">Refresh</a>\n";

  //Status Parameter
//  ptr +="<h3>Relay motor sprayer parameter</h3>\n";
  ptr +="<p style='text-align: center;'><span style='font-size: large;'><strong>Parameter Data</strong></span></p>\n";

  ptr +="<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>Waktu Penyemprotan ";
  ptr += 06;
  ptr += " :";
  ptr += 59;
  ptr += " :";
  ptr += 59;
  ptr += " sampai ";
  ptr += 20;
  ptr += " :";
  ptr += 59;
  ptr += " :";
  ptr += 59;
  ptr += "\n";
  ptr += "</p>";
  
  ptr +="<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>Durasi Penyemprotan = ";
  ptr += im5;
  ptr += " (detik)\n";
  ptr += "</p>";

  
  
  ptr +="<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>Jangkauan Sensor 1 = ";
  ptr += im1;
  ptr += " (cm)\n";
  ptr += "</p>";

  ptr +="<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>Jangkauan Sensor 2 = ";
  ptr += im2;
  ptr += " (cm)\n";
  ptr += "</p>";

  ptr +="<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>Jangkauan Sensor 3 = ";
  ptr += im3;
  ptr += " (cm)\n";
  ptr += "</p>";

  ptr +="<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>Jangkauan Sensor 4 = ";
  ptr += im4;
  ptr += " (cm)\n";
  ptr += "</p>";

  


  //pengaturan parameter
  ptr +="<p style='text-align: center;'><span style='font-size: large;'><strong>Parameter Jangkauan (Max: 400 cm)</strong></span></p>\n";
  
  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>";
  ptr += "<form action=\"/get\">Sensor 1: <input type=\"text\" name=\"input1\"> <input type=\"submit\" value=\"Ubah\"> (cm) </form><br>\n";
  ptr += "</p>";

  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>";
  ptr += "<form action=\"/get\">Sensor 2: <input type=\"text\" name=\"input2\"> <input type=\"submit\" value=\"Ubah\"> (cm) </form><br>\n";
  ptr += "</p>";

  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>";
  ptr += "<form action=\"/get\">Sensor 3: <input type=\"text\" name=\"input3\"> <input type=\"submit\" value=\"Ubah\"> (cm) </form><br>\n";
  ptr += "</p>";

  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>";
  ptr += "<form action=\"/get\">Sensor 4: <input type=\"text\" name=\"input4\"> <input type=\"submit\" value=\"Ubah\"> (cm) </form><br>\n";
  ptr += "</p>";

  ptr +="<p style='text-align: center;'><span style='font-size: large;'><strong>Durasi Penyemprotan (max: 3600 detik)</strong></span></p>\n";
  
  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>";
  ptr += "<form action=\"/get\"> Waktu : <input type=\"text\" name=\"input5\"> <input type=\"submit\" value=\"Ubah\"> (detik) </form><br>\n";
  ptr += "</p>";

  //Pengaturan parameter Waktu Semprot
  ptr +="<p style='text-align: center;'><span style='font-size: large;'><strong>Pengaturan Waktu Penyemprotan </strong></span></p>\n";

  ptr += "<form action=\"/get\">Start<br> Jam:<input type=\"text\" name=\"tsjam\" value=\"07\"><br>Menit:<input type=\"text\" name=\"tsmenit\" value=\"00\"><br> Detik:<input type=\"text\" name=\"tsdetik\" value=\"00\"><br><br> ";
  ptr += "Berakhir <br> Jam:<input type=\"text\" name=\"tbjam\" value=\"07\"><br>Menit:<input type=\"text\" name=\"tbmenit\" value=\"00\"><br> Detik:<input type=\"text\" name=\"tbdetik\" value=\"00\">";
  ptr += "<br><input type=\"submit\" value=\"Atur\"></form>";

  if(sprayer3stat)
  {ptr +="<p>Spray Timer Status: ON</p><a class=\"button button-off\" href=\"/timeroff\">OFF</a>\n";}
  else
  {ptr +="<p>Spray Timer Status: OFF</p><a class=\"button button-on\" href=\"/timeron\">ON</a>\n";}

  
  
  //reset waktu
  ptr +="<p style='text-align: center;'><span style='font-size: large;'><strong>Reset Jam Komputer </strong></span></p>\n";
  
   //Tampilan Waktu
  ptr +="<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>waktu saat ini: ";
  ptr += buft;
  ptr += "\n";
  ptr += "</p>";
  
  ptr += "<form action=\"/get\">Jam:<input type=\"text\" name=\"tsetjam\" value=\"07\"><br>Menit:<input type=\"text\" name=\"tsetmenit\" value=\"00\"><br> Detik:<input type=\"text\" name=\"tsetdetik\" value=\"00\"><br><br> ";
  ptr += "<br><input type=\"submit\" value=\"Atur\"></form>";

  
  
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  
  
  
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  
  pinMode(A0, INPUT); 
  pinMode(sprayer1pin, OUTPUT);
//  pinMode(sprayer2pin, OUTPUT);
  
  pinMode(trigP1, OUTPUT);  // Sets the trigPin1 as an Output1
  pinMode(echoP1, INPUT);   // Sets the echoPin1 as an Input1
  pinMode(trigP2, OUTPUT);  // Sets the trigPin1 as an Output2
  pinMode(echoP2, INPUT);   // Sets the echoPin1 as an Input2
  
  
  rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  //Time t(2020, 04, 19, 20, 33, 50, Time::kSunday); // aktifkan untuk update

  // Set the time and date on the chip.
  //rtc.time(t); //aktifkan untuk st up rtc

  
  
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

  server.on("/", handle_OnConnect);
  server.on("/sprayer1on", handle_sprayer1on);
  server.on("/sprayer1off", handle_sprayer1off);
  server.on("/sprayer2on", handle_sprayer2on);
  server.on("/sprayer2off", handle_sprayer2off);
  server.on("/get", HTTP_GET, handle_input);
  server.on("/timeron", handle_timeron);
  server.on("/timeroff", handle_timeroff);
  
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
  printTime();
  im1 = EEPROM.get(0,im1);
  im2 = EEPROM.get(10,im2);
  im3 = EEPROM.get(20,im3);
  im4 = EEPROM.get(30,im4);
  im5 = EEPROM.get(40,im5);

  uint8_t tsjam, tsmnt, tsdtk;
  uint8_t tbjam, tbmnt, tbdtk;
  uint8_t setjam, setmnt, setdtk;

  tsjam = EEPROM.get(50,tsjam);
}

void loop() {
  server.handleClient();
  currentTime = millis();
  printTime();
  
  if(sprayer1status){
    ultrasonic_read();
    if(  distance1 <= im1 | distance2 <= im2){
        state = 1;
//      digitalWrite(sprayer1pin, HIGH);
//      digitalWrite(sprayer2pin, HIGH);
      
      
    }else{
//      digitalWrite(sprayer1pin, LOW);
//      digitalWrite(sprayer2pin, LOW);
    }
  }
  else{
    if(sprayer2status){
      digitalWrite(sprayer1pin, LOW);
       blink_lamp.blink(75);
//      digitalWrite(sprayer2pin, HIGH);
    
    }else{
      digitalWrite(sprayer1pin, HIGH);
//      digitalWrite(sprayer2pin, LOW);
    }
  }
  
  if(currentTime - previousTime <= im5*1000 & state ==1) {
    digitalWrite(sprayer1pin, LOW);
    blink_lamp.blink(150);
//    digitalWrite(sprayer2pin, HIGH);       
       
       //Serial.println(currentTime);
       //Serial.println(previousTime);
//       Serial.println(currentTime - previousTime);
       
   }else{
    state = 2;
    blink_lamp.blink(1000);
    previousTime = currentTime;
    digitalWrite(sprayer1pin, HIGH);
//    digitalWrite(sprayer2pin, LOW);
   }
  
  
  
//  blink_lamp.blink(850);
//  alarm.blink(500);
  
  

}
