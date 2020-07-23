#include <ESP8266WiFi.h>
//const char* ssid = "fh_6397a8";
//const char* password = "wlan9c6857";
const char* ssid = "iRoschool";
const char* password = "wlan3e1a3f";
float temp_celsius = 0;
float temp_fahrenheit = 0;
float distance1;
long duration1;
float distance2;
long duration2;

long pp_counter;
int current_state1 = 0;
int previous_state1 = 0;


const int trigP1 = 14;  //SD3 Or GPIO-10 of nodemcu
const int echoP1 = 12;  //SD2 Or GPIO-9 of nodemcu
const int trigP2 = 13;  //SD3 Or GPIO-10 of nodemcu
const int echoP2 = 15;  //SD2 Or GPIO-9 of nodemcu

WiFiServer server(80);
void setup() {
 Serial.begin(115200);
 pinMode(trigP1, OUTPUT);  // Sets the trigPin1 as an Output1
 pinMode(echoP1, INPUT);   // Sets the echoPin1 as an Input1
 pinMode(trigP2, OUTPUT);  // Sets the trigPin1 as an Output2
 pinMode(echoP2, INPUT);   // Sets the echoPin1 as an Input2
  
  pinMode(A0, INPUT);   
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi is connected");
server.begin();
Serial.println("Server started");
Serial.println(WiFi.localIP());


}
void loop() {
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

  if(distance2<5){
    current_state1 = 1; 
  }else{
    current_state1 = 0;
  }

  if(previous_state1 != current_state1 ){
    if(current_state1 == 1){pp_counter++;}
//  pp_counter++;
  previous_state1 = current_state1;
  delay(50);
  }

//  if(current_state1 == 0){previous_state1 == 0;}
  
  
  
  
  Serial.print("Distance1 = ");        //Output distance on arduino serial monitor 
  Serial.print(distance1);  
  Serial.print("\tDistance2 = ");        //Output distance on arduino serial monitor 
  Serial.println(distance2);
  Serial.print("Counter: ");
  Serial.println(pp_counter);  

  

temp_celsius = (analogRead(A0) * 330.0) / 1023.0;   // To convert analog values to Celsius We have 3.3 V on our board and we know that output voltage of LM35 varies by 10 mV to every degree Celsius rise/fall. So , (A0*3300/10)/1023 = celsius
temp_fahrenheit = temp_celsius * 1.8 + 32.0;
//Serial.print("  Temperature = ");
//Serial.print(temp_celsius);
//Serial.print(" Celsius, ");
//Serial.print(temp_fahrenheit);
//Serial.println(" Fahrenheit");
WiFiClient client = server.available();
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");  // the connection will be closed after completion of the response
client.println("Refresh: 1");  // update the page after 10 sec
client.println();
client.println("<!DOCTYPE HTML>");
client.println("<html>");

client.println("<body><h1>ESP8266 Weather Web Server</h1>");
client.println("<table border=\"2\" width=\"456\" cellpadding=\"10\"><tbody><tr><td>");

client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Digital Thermometer</strong></span></p>");
client.print("<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: large;'>Temperature (*C)= ");
client.println(distance1);
client.print("<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: large;'>Temperature (F) = ");
client.println(distance2);
client.print("</p>");
client.println("</html>");
delay(1000);
}
