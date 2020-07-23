/*
  Web Server

  A simple web server that shows the value of the analog input pins.
  using an Arduino Wiznet Ethernet shield.

  Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13
   Analog inputs attached to pins A0 through A5 (optional)

  created 18 Dec 2009
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe
  modified 12 Aug 2013
  by Soohwan Kim
  Modified 18 Aug 2015
  by Vassilis Serasidis

  =========================================================
  Ported to STM32F103 on 18 Aug 2015 by Vassilis Serasidis

  <---- Pinout ---->
  W5x00 <--> STM32F103
  SS    <-->  PA4 <-->  BOARD_SPI1_NSS_PIN
  SCK   <-->  PA5 <-->  BOARD_SPI1_SCK_PIN
  MISO  <-->  PA6 <-->  BOARD_SPI1_MISO_PIN
  MOSI  <-->  PA7 <-->  BOARD_SPI1_MOSI_PIN
  =========================================================


*/

#include <SPI.h>
#include <UIPEthernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xDE, 0xAA, 0xBE, 0xEF, 0xFE, 0xF1};
#endif
IPAddress ip(192, 168, 1, 202);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // Open Serial1 communications and wait for port to open:
  Serial1.begin(115200);
  while (!Serial1) {
    ; // wait for Serial1 port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
#if defined(WIZ550io_WITH_MACADDRESS)
  Ethernet.begin(ip);
#else
  Ethernet.begin(mac, ip);
#endif
  server.begin();
  Serial1.print("server is at ");
  Serial1.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial1.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial1.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          int sensorReading = analogRead(0);


          client.print("<p><center>") ;
          client.print(sensorReading);
          client.print(",");

          client.print(sensorReading);
          client.print(",");

          client.print(sensorReading);
          client.print(",");

          client.print(sensorReading);
          client.print(",");

          client.print(sensorReading);
          client.print(",</p>");

          // output the value of each analog input pin
          //          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
          //            int sensorReading = analogRead(analogChannel);
          //            client.print("analog input ");
          //            client.print(analogChannel);
          //            client.print(" is ");
          //            client.print(sensorReading);
          //            client.println("<br />");
          //          }
          client.print("<h1 style=\"text-align: center;\">Real-time People Counter.</h1>");
          client.print("<p>&nbsp;</p>");
          client.print("<p>&nbsp;</p>");
          client.print("<center>") ;
          client.print("<table style=\"width: 450px; height: 69px; background-color: #dcdcdc;\" border=\"5\">");

          // Probe 01
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>T S 01_CT</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");

          if (sensorReading == 127.00)
          {
            client.println("Error");
            Serial.println("Error getting temperature ");
          }
          else
          {
            client.print(sensorReading);
            client.print(" - avg ");
            client.print(sensorReading);
            client.print(" C");
          }
          client.print("</strong></span></td></tr>");

          // Probe 02
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>T S 02_CT</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");

          if (sensorReading == 128.00)
          {
            client.println("Error");
            Serial.println("Error getting temperature ");
          }
          else
          {
            client.print(sensorReading);
            client.print(" - avg ");
            client.print(sensorReading);
            client.print(" C");
          }
          client.print("</strong></span></td></tr>");


          // Probe 03
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>T S 03_CT</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");

          if (sensorReading == 129.00)
          {
            client.println("Error");
            Serial.println("Error getting temperature ");
          }
          else
          {
            client.print(sensorReading);
            client.print(" - avg ");
            client.print(sensorReading);
            client.print(" C");
          }
          client.print("</strong></span></td></tr>");

          // Probe 04
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>T S 04_CT</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");

          if (sensorReading == 129.00)
          {
            client.println("Error");
            Serial.println("Error getting temperature ");
          }
          else
          {
            client.print(sensorReading);
            client.print(" - avg ");
            client.print(sensorReading);
            client.print(" C");
          }
          client.print("</strong></span></td></tr>");

          // Probe 05
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>T S 05_CT</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");

          if (sensorReading == 120.00)
          {
            client.println("Error");
            Serial.println("Error getting temperature ");
          }
          else
          {
            client.print(sensorReading);
            client.print(" - avg ");
            client.print(sensorReading);
            client.print(" C");
          }
          client.print("</strong></span></td></tr>");

          client.print("</tbody></table></center>");
          client.print("<h5 style=\"text-align: center;\">(C) Dan Padure 2014 DS18B20 webserver v0.42</h5>");

          client.print("<p>&nbsp;</p>");
          client.print("<p>&nbsp;</p>");
          client.print("<center>") ;
          client.print("<table style=\"width: 450px; height: 69px; background-color: #dcdcdc;\" border=\"5\">");

          // Probe 01
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>T S 01_CT</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");

          if (sensorReading == 127.00)
          {
            client.println("Error");
            Serial.println("Error getting temperature ");
          }
          else
          {
            client.print(sensorReading);
            client.print(" - avg ");
            client.print(sensorReading);
            client.print(" C");
          }
          client.print("</strong></span></td></tr>");



          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial1.println("client disonnected");
  }
}
