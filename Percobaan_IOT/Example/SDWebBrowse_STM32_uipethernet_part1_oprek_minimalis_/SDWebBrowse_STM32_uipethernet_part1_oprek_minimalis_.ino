/*
   SDWebBrowse.ino

   This sketch uses the microSD card slot on the a WIZ5500 Ethernet shield
   to serve up files over a very minimal browsing interface

   Some code is from Bill Greiman's SdFatLib examples, some is from the
   Arduino Ethernet WebServer example and the rest is from Limor Fried
   (Adafruit) so its probably under GPL

   Tutorial is at https://learn.adafruit.com/arduino-ethernet-sd-card/serving-files-over-ethernet

*/

#include <SPI.h>
#include <SD.h>
//#include <Ethernet.h>
#include <UIPEthernet.h>

/************ ETHERNET STUFF ************/
byte mac[] = { 0xAE, 0xEA, 0xAE, 0xEF, 0xFE, 0xAF };
byte ip[] = { 192, 168, 0, 211 };                // change if necessary
EthernetServer server(80);

/************ SDCARD STUFF ************/
#define SDCARD_CS PA3
File root;

//#if defined(ESP8266)
//  // default for ESPressif
//  #define WIZ_CS 15
//#elif defined(ESP32)
//  #define WIZ_CS 33
//#elif defined(ARDUINO_STM32_FEATHER)
//  // default for WICED
//  #define WIZ_CS PB4
//#elif defined(TEENSYDUINO)
//  #define WIZ_CS 10
//#elif defined(ARDUINO_FEATHER52)
//  #define WIZ_CS 11
//#else   // default for 328p, 32u4 and m0
//  #define WIZ_CS 10
//#endif

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

void error_P(const char* str) {
  Serial1.print(F("error: "));
  Serial1.println(str);

  while (1);
}

void setup() {
  Serial1.begin(9600);
  Serial1.println("mcu started!");
  while (!Serial1){
    ;      // For 32u4 based microcontrollers like 32u4 Adalogger Feather
  }

  //Serial1.print(F("Free RAM: ")); Serial1.println(FreeRam());

  //  Ethernet.init(PA4);
  if (!SD.begin(SDCARD_CS)) {
    error("card.init failed!");
  }

  root = SD.open("/");
  printDirectory(root, 0);

  //  // Recursive list of all directories
  //  Serial1.println(F("Files found in all dirs:"));
  //  printDirectory(root, 0);
  //
  //  Serial1.println();
  //  Serial1.println(F("Done"));

  // Debugging complete, we start the server!
  Serial1.println(F("Initializing ENC82J60"));
  Ethernet.init(PA4);
  // give the ethernet module time to boot up
  delay(1000);
  // start the Ethernet connection
  // Use the fixed IP specified. If you want to use DHCP first
  //   then switch the Ethernet.begin statements
  //Ethernet.begin(mac, ip);
#if defined(WIZ550io_WITH_MACADDRESS)
  Ethernet.begin(ip);
#else
  Ethernet.begin(mac, ip);
#endif
  server.begin();
  Serial1.print("server is at ");
  Serial1.println(Ethernet.localIP());


  // try to congifure using DHCP address instead of IP:
  //  Ethernet.begin(mac);

  // print the Ethernet board/shield's IP address to Serial1 monitor
  Serial1.print(F("My IP address: "));
  Serial1.println(Ethernet.localIP());

  server.begin();
}

void ListFiles(EthernetClient client, uint8_t flags, File dir) {
  client.println("<ul>");
  while (true) {
    File entry = dir.openNextFile();

    // done if past last used entry
    if (! entry) {
      // no more files
      break;
    }

    // print any indent spaces
    client.print("<li><a href=\"");
    client.print(entry.name());
    if (entry.isDirectory()) {
      client.println("/");
    }
    client.print("\">");

    // print file name with possible blank fill
    client.print(entry.name());
    if (entry.isDirectory()) {
      client.println("/");
    }

    client.print("</a>");
    /*
        // print modify date/time if requested
        if (flags & LS_DATE) {
           dir.printFatDate(p.lastWriteDate);
           client.print(' ');
           dir.printFatTime(p.lastWriteTime);
        }
        // print size if requested
        if (!DIR_IS_SUBDIR(&p) && (flags & LS_SIZE)) {
          client.print(' ');
          client.print(p.fileSize);
        }
    */
    client.println("</li>");
    entry.close();
  }
  client.println("</ul>");
}

// How big our line buffer should be. 100 is plenty!
#define BUFSIZ 100

void loop()
{
  //  Ethernet.init(PA4);
  char clientline[BUFSIZ];
  char name[17];
  int index = 0;

  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;

    // reset the input buffer
    index = 0;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial1.print(F("clientline: "));
        Serial1.println(c);

        // If it isn't a new line, add the character to the buffer
        if (c != '\n' && c != '\r') {
          clientline[index] = c;
          index++;
          // are we too big for the buffer? start tossing out data
          if (index >= BUFSIZ)
            index = BUFSIZ - 1;

          // continue to read more data!
          continue;
        }

        // got a \n or \r new line, which means the string is done
        clientline[index] = 0;

        // Print it out for debugging
        Serial1.print(F("clientline Serial: "));
        Serial1.println(clientline);

        // Look for substring such as a request to get the file
        //        if (strstr(clientline, "GET /") != 0) {
        // this time no space after the /, so a sub-file!
        char *filename;

        filename = clientline + 5; // look after the "GET /" (5 chars)  *******
        // a little trick, look for the " HTTP/1.1" string and
        //           turn the first character of the substring into a 0 to clear it out.
        (strstr(clientline, " HTTP"))[0] = 0;

        //          if (filename[strlen(filename) - 1] == '/') { // Trim a directory filename
        //            filename[strlen(filename) - 1] = 0;      //  as Open throws error with trailing /
        //          }

        Serial1.print(F("Web request for: ")); Serial1.println(filename);  // print the file we want

        File file = SD.open(filename, O_READ);
        if ( file == 0 ) {  // Opening the file with return code of 0 is an error in SDFile.open
          client.println("HTTP/1.1 404 Not Found");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<h2>File Not Found!</h2>");
          client.println("<br><h3>Couldn't open the File!</h3>");
          break;
        }

        Serial1.println("File Opened!");

        client.println("HTTP/1.1 200 OK");
        if (file.isDirectory()) {
          Serial1.println("is a directory");
          //file.close();
          client.println("Content-Type: text/html");
          client.println();
          client.print("<h2>Files in in");
          client.print(filename);
          client.println(":</h2>");
          ListFiles(client, LS_SIZE, file);
          file.close();
        }
        else { // Any non-directory clicked, server will send file to client for download
          client.println("Content-Type: application/octet-stream");
          client.println();

          char file_buffer[16];
          int avail;
          while (avail = file.available()) {
            int to_read = min(avail, 16);
            if (to_read != file.read(file_buffer, to_read)) {
              break;
            }
            // uncomment the Serial1 to debug (slow!)
            //Serial1.write((char)c);
            client.write(file_buffer, to_read);
          }
          file.close();
        }
        //        }
        //        else {
        //          // everything else is a 404
        //          client.println("HTTP/1.1 404 Not Found");
        //          client.println("Content-Type: text/html");
        //          client.println();
        //          client.println("<h2>File Not Found!</h2>");
        //        }
        break;
      }
    }
    // give the web browser time to receive the data
    delay(5000);
    client.stop();
  }
}


void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial1.print('\t');
    }
    Serial1.print(entry.name());
    if (entry.isDirectory()) {
      Serial1.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial1.print("\t\t");
      Serial1.println(entry.size(), DEC);
    }
    entry.close();
  }
}
