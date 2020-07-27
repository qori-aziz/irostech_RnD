#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <stdio.h>

ESP8266WebServer server(80);

#ifndef APSSID
#define APSSID "People Counter"
#define APPSK  "12345678"
#endif

const char* ssid = APSSID;
const char* password = APPSK;

String inputThr; int thr = 5;
String inputCap; int cap = 5;

int data[5];

String errorInfo;

void handle_OnConnect() {
  errorInfo = "Selamat Datang";
  server.send(404, "text/plain", errorInfo);
}

void handle_input() {
  if (server.argName(0) == "thr" && server.arg("thr") != "" && server.arg("thr").toFloat() <= 400 && server.arg("thr").toFloat() >= 0 ) {
    inputThr = server.arg("thr"); thr = inputThr.toInt();
    EEPROM.put(0, thr); EEPROM.commit();
    errorInfo = "Threshold set";
  }
  if (server.argName(0) == "cap" && server.arg("cap") != "" && server.arg("cap").toFloat() <= 400 && server.arg("cap").toFloat() >= 0 ) {
    inputCap = server.arg("cap"); cap = inputCap.toInt();
    EEPROM.put(10, cap); EEPROM.commit();
    errorInfo = "Person capacity set";
  }
  server.send(200, "text/html", SendHTML());
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML() {

  String ptr;

  ptr += "<!DOCTYPE html> <html>\n";

  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>People Counter</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 40px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 10px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Covid-19 People Counter System</h1>\n";

  //Tombol refresh
  ptr += "<p></p><a class=\"button button-off\" href=\"/\">Refresh</a>\n";

  //Status Parameter
  //  ptr +="<h3>Relay motor sprayer parameter</h3>\n";
  ptr += "<p style='text-align: center;'><span style='font-size: large;'><strong>Parameter Data</strong></span></p>\n";

  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>Threshold : ";
  ptr += thr;
  ptr += " (cm)\n";
  ptr += "</p>";

  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>Person Capacity : ";
  ptr += cap;
  ptr += " (cm)\n";
  ptr += "</p>";

  //pengaturan parameter
  ptr += "<p style='text-align: center;'><span style='font-size: large;'><strong>Parameter Jangkauan (Max: 400 cm)</strong></span></p>\n";

  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>";
  ptr += "<form action=\"/set\">Threshold : <input type=\"text\" name=\"thr\"> <input type=\"submit\" value=\"Ubah\"> (cm) </form><br>\n";
  ptr += "</p>";

  ptr += "<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: small;'>";
  ptr += "<form action=\"/set\">Person Capacity : <input type=\"text\" name=\"cap\"> <input type=\"submit\" value=\"Ubah\"> (person) </form><br>\n";
  ptr += "</p>";

  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;

}

IPAddress Ip(192, 168, 1, 1);
IPAddress NMask(255, 255, 255, 0);

void setup() {
  Serial.begin(57600);
  EEPROM.begin(512);
  delay(5000);

  Serial.println();
  Serial.print("Configuring access point...");

  WiFi.softAP(ssid, password);

  WiFi.softAPConfig(Ip, Ip, NMask);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handle_OnConnect);
  server.on("/set", HTTP_GET, handle_input);

  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");

  thr = EEPROM.get(0, thr);
  cap = EEPROM.get(10, cap);
}

void loop() {
  server.handleClient();
  data[0] = thr;
  data[1] = cap;

  // Serial.parseInt();

  //    Serial.print(cap);
  //    Serial.println(thr);

  serial_send();
}


/************************************Serial send to STM***************************************

   8
   /*****************************************************************************************

   Mengirim data serial thr dan cap ke stm
*/
void serial_send() {
  //Serial.write("*");
  int temp = 0;
  Serial.print("*");
  delay(200);
  if (thr >= 10) {
    temp = thr / 10;
    Serial.print(temp);
    delay(200);
    Serial.print(thr % 10);
    delay(200);
  } else if (thr >= 100) {
    temp = thr / 100;
    Serial.print(temp);
    delay(200);
    temp = thr / 10;
    Serial.print(temp);
    delay(200);
    Serial.print(thr % 10);
    delay(200);
  } else if (thr<10){
    Serial.print(thr);
    delay(200);
  }
  //  Serial.print(thr);
  //Serial.write(",");
  Serial.print(",");
  delay(200);
  //Serial.write(cap);
  //  Serial.print(cap);
  if (cap >= 10) {
    temp = cap / 10;
    Serial.print(temp);
    delay(200);
    Serial.print(thr % 10);
    delay(200);
  } else if (cap >= 100) {
    temp = cap / 100;
    Serial.print(temp);
    temp = cap / 10;
    delay(200);
    Serial.print(temp);
    delay(200);
    Serial.print(cap % 10);
    delay(200);
  } else if (cap<10){
    Serial.print(cap);
    delay(200);
  }
  //  delay(100);
  //Serial.write("#\n");
  Serial.print("#");
  delay(200);
  //Serial.print(thr);Serial.println(cap);
  //  delay(250);
  //  Serial.flush();
}
