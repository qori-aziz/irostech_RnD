#include <ESP8266WiFi.h>
//const char* ssid = "fh_6397a8";
//const char* password = "wlan9c6857";
const char* ssid = "ypn_network";
const char* password = "1234567890";
float temp_celsius = 0;
float temp_fahrenheit = 0;
float distance1;
long duration1;
float distance2;
long duration2;

long int pp_counter = 0;
long int total_visitor=0;
long int mall_capacity = 1000;
float capacity_procent=0;
long int pp_in1=0,pp_in2=0,pp_in3=0,pp_in4=0,pp_in5=0;

long int pp_out1, pp_out2;

int gate1=1, gate2=2, gate3=3, gate4=1, gate5=1, gate6=2, gate7=1;

int current_state1 = 0;
int previous_state1 = 0;
long flag_counter =0;

float calibrate1 = 150;
float calibrate2 = 150;


const int trigP1 = 14;  //GPIO-5 of nodemcu
const int echoP1 = 12;  //GPIO-6 of nodemcu
const int trigP2 = 13;  //GPIO-7 of nodemcu
const int echoP2 = 15;  //GPIO-8 of nodemcu

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

//  if(distance1 < calibrate1 && distance2 >= calibrate2){ // sensor1 = 1 & sensor2 = 0
//    current_state1 = 0; 
//  }else if(distance1 < calibrate1 && distance2 < calibrate2){ // sensor1 = 1 & sensor2 = 1
//    flag_counter = 1; 
//  }
//  if(distance1 >= calibrate1 && distance2 < calibrate2){ // sensor1 = 0 & sensor2 = 1
//    current_state1 = 2; 
//  }
//  if(distance1 >= calibrate1 && distance2 >= calibrate2){ // sensor1 = 0 & sensor2 = 0
//    current_state1 = 3;
//    flag_counter = 0; 
//  }

  if(distance1 < calibrate1){ // sensor1 = 1 & sensor2 = 0
    current_state1 = 0; 
  }else if(distance1 >= calibrate1){ // sensor1 = 0 & sensor2 = 0
    current_state1 = 3;

  }

  if(previous_state1 != current_state1 ){
//    if(current_state1 == 2 & flag_counter == 1){pp_in1++; total_visitor++;}
//    if(current_state1 == 0 & flag_counter == 1){pp_in1--;pp_out1++;}

    if(current_state1 == 0){pp_in1++; total_visitor++;}
//    if(current_state1 == 0){pp_in1--;pp_out1++;}
    
  previous_state1 = current_state1;
  }

  pp_counter = pp_in1 + pp_in2 + pp_in3;
  capacity_procent =(float) pp_counter / mall_capacity *100;
  
  
  
  
  Serial.print("Distance1 = ");        //Output distance on arduino serial monitor 
  Serial.print(distance1);  
  Serial.print("\tDistance2 = ");        //Output distance on arduino serial monitor 
  Serial.println(distance2);
  Serial.print("Counter: ");
  Serial.println(pp_counter);
   Serial.print("Counter procent: ");
  Serial.println(capacity_procent);  

  

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
//client.println("Connection: close");  // the connection will be closed after completion of the response
client.println("Refresh: 0.5");  // update the page after 10 sec
client.println();
client.println("<!DOCTYPE HTML>");
client.println("<html>");

client.println("<body><h1>Visitor Monitoring System</h1>");
client.println("<table border=\"2\" width=\"656\" cellpadding=\"10\"><tbody><tr><td>");


client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Pengunjung Miko Mall</strong></span></p>");
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Dalam Mall= ");
client.println(pp_counter);

client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Estimasi Pengunjung (%)= ");
client.println(capacity_procent);

client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Masuk = ");
client.println(pp_in1);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Keluar= ");
client.println(pp_out1);
// Mall Capacity
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Kapasitas Pengunjung Mall= ");
client.println(mall_capacity);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Pengunjung Hari ini = ");
client.println(total_visitor);

client.print("</p>");

client.println("<table border=\"2\" width=\"656\" cellpadding=\"10\"><tbody><tr><td>");

// Floor 1
client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Pengunjung Miko Mall Floor 1 Gate In 1</strong></span></p>");
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Masuk = ");
client.println(pp_in1);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Keluar= ");
client.println(pp_out1);

client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Pengunjung Miko Mall Floor 1 Gate In 2</strong></span></p>");
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Masuk = ");
client.println(pp_in2);
//client.println(distance1);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Keluar= ");
client.println(pp_out1);
//client.println(distance2);

client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Pengunjung Miko Mall Floor 1 Gate In 3</strong></span></p>");
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Masuk = ");
client.println(pp_in3);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Keluar= ");
client.println(pp_out1);

client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Pengunjung Miko Mall Floor 1 Gate Out 1</strong></span></p>");
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Masuk = ");
client.println(pp_in2);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Keluar= ");
client.println(pp_out1);

// Floor 2


client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Pengunjung Miko Mall Floor 2 Gate In 1</strong></span></p>");
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Masuk = ");
client.println(pp_in4);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Keluar= ");
client.println(pp_out2);

client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Pengunjung Miko Mall Floor 2 Gate In 2</strong></span></p>");
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Masuk = ");
client.println(pp_in5);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Keluar= ");
client.println(pp_out2);

client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Pengunjung Miko Mall Floor 2 Gate Out 1</strong></span></p>");
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Masuk = ");
client.println(pp_in4);
client.print("<p style='text-align: left;'><span style='color: #0000ff;'><strong style='font-size: large;'>Jumlah Pengunjung Keluar= ");
client.println(pp_out2);




client.print("</p>");
client.println("</html>");
delay(10);
}
