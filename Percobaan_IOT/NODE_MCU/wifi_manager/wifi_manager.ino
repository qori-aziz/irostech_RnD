#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

//wlan9c6857

WiFiServer server(80);

float temp_celsius = 0;
float temp_fahrenheit = 0;

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output15State = "off";
String output13State = "off";

// Assign output variables to GPIO pins
const int output15 = 15;
const int output13 = 13;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(output15, OUTPUT);
    pinMode(output13, OUTPUT);
    // Set outputs to LOW
    digitalWrite(output15, LOW);
    digitalWrite(output13, LOW);
    
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    wifiManager.autoConnect("Kandatz WiFi Manager");
    Serial.println("connected :)");
    if (WiFi.status() == WL_CONNECTED)
        Serial.print("connected IP:)");
        Serial.println(WiFi.localIP());
        server.begin();
}     


void loop() { 

temp_celsius = (analogRead(A0) * 330.0) / 1023.0;   // To convert analog values to Celsius We have 3.3 V on our board and we know that output voltage of LM35 varies by 10 mV to every degree Celsius rise/fall. So , (A0*3300/10)/1023 = celsius
  temp_fahrenheit = temp_celsius * 1.8 + 32.0;
  temp_celsius = (analogRead(A0) * 330.0) / 1023.0;   // To convert analog values to Celsius We have 3.3 V on our board and we know that output voltage of LM35 varies by 10 mV to every degree Celsius rise/fall. So , (A0*3300/10)/1023 = celsius
  temp_fahrenheit = temp_celsius * 1.8 + 32.0;

  //print on terminal
  Serial.print("  Temperature = ");
  Serial.print(temp_celsius);
  Serial.print(" Celsius, ");
  Serial.print(temp_fahrenheit);
  Serial.println(" Fahrenheit");

  WiFiClient client = server.available();   // Listen for incoming clients
  
  if (client) {                             // If a new client connects,
    Serial.println("New Client -_-");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println("Refresh: 2");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /15/on") >= 0) {
              Serial.println("GPIO 15 on");
              output15State = "on";
              digitalWrite(output15, HIGH);
            } else if (header.indexOf("GET /15/off") >= 0) {
              Serial.println("GPIO 15 off");
              output15State = "off";
              digitalWrite(output15, LOW);
            } else if (header.indexOf("GET /13/on") >= 0) {
              Serial.println("GPIO 13 on");
              output13State = "on";
              digitalWrite(output13, HIGH);
            } else if (header.indexOf("GET /13/off") >= 0) {
              Serial.println("GPIO 13 off");
              output13State = "off";
              digitalWrite(output13, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Smart Home Control</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 15  
            client.println("<p>GPIO 15 - State " + output15State + "</p>");
            // If the output15State is off, it displays the ON button       
            if (output15State=="off") {
              client.println("<p><a href=\"/15/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/15/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 13  
            client.println("<p>GPIO 13 - State " + output13State + "</p>");
            // If the output13State is off, it displays the ON button       
            if (output13State=="off") {
              client.println("<p><a href=\"/13/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/13/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Digital Thermometer</strong></span></p>");
            client.print("<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: large;'>Temperature (*C)= ");
            client.println(temp_celsius);
            client.print("<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: large;'>Temperature (F) = ");
            client.println(temp_fahrenheit);
            client.print("</p>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
//     Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
 delay(1000); 

}
