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
#include <SD.h>
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEA};
#endif

String buf;
int count_;

File myFile;

IPAddress ip(192, 168, 1, 179);

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

  if (!SD.begin(PA3)) {
    Serial1.println("initialization failed!");
    while (1);
  }
  Serial1.println("initialization done.");
}


void loop() {
  count_++;
  server_pp();
  mmc_card();
//  Serial1.println(count_);
//  Serial1.println(buf);
  buf = "";

}

void mmc_card() {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    //    Serial1.print("Writing to test.txt...");
    myFile.println(count_);
    // close the file:
    myFile.close();
    //    Serial1.println("done.");
  } else {
    //     if the file didn't open, print an error:
    Serial1.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    //    Serial1.println("test.txt:");
     
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
//      Serial1.write(myFile.read());
      buf = (String)myFile.read();
    }
    // close the file:
    myFile.close();
    
  } else {
    // if the file didn't open, print an error:
    //    Serial1.println("error opening test.txt");
  }
}

void server_pp () {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial1.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //        Serial1.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 10");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("basis data: ");
          client.println(count_);
          client.println("\n");
          client.println("bacanan sdcard: ");
          client.println(buf);
          
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
