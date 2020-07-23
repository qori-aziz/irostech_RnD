///*
// * EMailSender library for Arduino, esp8266 and esp32
// * Arduino Mega and UIPEthernet send example with attach
// * this example is not tested for all, I can't find a provider
// * that manage attach without SSL and TLS
// *
// * Pay attention you must set in the library
// * #define DEFAULT_EMAIL_NETWORK_TYPE_ARDUINO 	NETWORK_ENC28J60
// * for UIPEthernet
// *
// * #define DEFAULT_EMAIL_NETWORK_TYPE_ARDUINO 	NETWORK_W5100
// * for standard Ethernet
// *
// * The base64 encoding of the image is slow, so be patient
// *
// *
// * https://www.mischianti.org
// *
// */
//
//#include "Arduino.h"
//#include <SPI.h>
////#include <UIPEthernet.h>
//#include <SD.h>
//#include <Ethernet.h>
//
//#include <EMailSender.h>
//
//// Enter a MAC address for your controller below.
//// Newer Ethernet shields have a MAC address printed on a sticker on the shield
//byte mac[] = { 0xDF, 0xAA, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress ip(192, 168, 1, 177);
////EthernetServer server(80);
//
////	EMailSender(const char* email_login, const char* email_password, const char* email_from, const char* smtp_server, uint16_t smtp_port, bool isSecure = false);
//EMailSender emailSend("prima.nugraha.nusantara@gmail.com", "nugraha19101989");
//
//void printDirectory(File dir, int numTabs);
//
////The setup function is called once at startup of the sketch
//void setup()
//{
//    Serial.begin(115200);
//
//    delay(2000);
//
//    Serial.println("Starting!");
//    Ethernet.init(10);  // Most Arduino shields
////    Serial.print("Initializing SD card...");
////
////    if (!SD.begin(4)) {
////      Serial.println("initialization failed!");
////      while (1);
////    }
////    Serial.println("initialization done.");
////
////    File root = SD.open("/");
////
////    printDirectory(root, 0);
//
//    Serial.println("done!");
//
//    Ethernet.begin(mac, ip);
//
//    delay(2000);
//    // start the server
////  server.begin();
//  Serial.print("server is at ");
//  Serial.println(Ethernet.localIP());
//
////	File myFile = SD.open("/TEST.TXT", "r");
////	  if(myFile) {
////		  myFile.seek(0);
////		  DEBUG_PRINTLN(F("OK"));
////		myFile.close();
////	  }
////	  else {
////		  DEBUG_PRINTLN(F("KO"));
////	  }
////
////
////    // start the Ethernet connection:
////    if (Ethernet.begin(mac) == 0) {
////      Serial.println("Failed to configure Ethernet using DHCP");
////      while(1);
////    }
////    Serial.print("IP address ");
////    Serial.println(Ethernet.localIP());
//
//
//    EMailSender::EMailMessage message;
//    message.subject = "Soggetto";
//    message.message = "Ciao come stai<br>io bene.<br>www.mischianti.org";
//    message.mime = MIME_TEXT_PLAIN;
//
//// 		Two file
////    EMailSender::FileDescriptior fileDescriptor[2];
////    fileDescriptor[1].filename = F("test.txt");
////    fileDescriptor[1].url = F("/test.txt");
////    fileDescriptor[1].storageType = EMailSender::EMAIL_STORAGE_TYPE_SD;
////
////    fileDescriptor[0].filename = F("logo.jpg");
////    fileDescriptor[0].url = F("/logo.jpg");
////    fileDescriptor[0].mime = "image/jpg";
////    fileDescriptor[0].encode64 = true;
////    fileDescriptor[0].storageType = EMailSender::EMAIL_STORAGE_TYPE_SD;
////
////    EMailSender::Attachments attachs = {2, fileDescriptor};
//
//// 		One file
////    EMailSender::FileDescriptior fileDescriptor[1];
////    fileDescriptor[0].filename = F("test.txt");
////    fileDescriptor[0].url = F("/test.txt");
////    fileDescriptor[0].mime = MIME_TEXT_PLAIN;
////    fileDescriptor[0].storageType = EMailSender::EMAIL_STORAGE_TYPE_SD;
//
//    // Pay attention base64 encoding is quite slow
////	EMailSender::FileDescriptior fileDescriptor[2];
////	fileDescriptor[0].filename = F("logo.jpg");
////	fileDescriptor[0].url = F("/logo.jpg");
////	fileDescriptor[0].mime = "image/jpg";
////	fileDescriptor[0].encode64 = false;
////	fileDescriptor[0].storageType = EMailSender::EMAIL_STORAGE_TYPE_SD;
//
////    EMailSender::Attachments attachs = {1, fileDescriptor};
//
////    EMailSender::Response resp = emailSend.send("yayanpnugraha@gmail.com", message, attachs);
//  EMailSender::Response resp = emailSend.send("ypn89ypn@gmail.com", message);
//
//    Serial.println("Sending status: ");
//
//    Serial.println(resp.status);
//    Serial.println(resp.code);
//    Serial.println(resp.desc);
//
//    //File root2 = SD.open("/");
//
//    //printDirectory(root2, 0);
//
////    Serial.println("done sent!");
////    SD.end();
//}
//
//void loop()
//{
//
//}
//
//void printDirectory(File dir, int numTabs) {
//  while (true) {
//
//    File entry =  dir.openNextFile();
//    if (! entry) {
//      // no more files
//      break;
//    }
//    for (uint8_t i = 0; i < numTabs; i++) {
//      Serial.print('\t');
//    }
//    Serial.print(entry.name());
//    if (entry.isDirectory()) {
//      Serial.println("/");
//      printDirectory(entry, numTabs + 1);
//    } else {
//      // files have sizes, directories do not
//      Serial.print("\t\t");
//      Serial.println(entry.size(), DEC);
//    }
//    entry.close();
//  }
//}
//



//zoomkat 12/26/12
//SD server test code
//open serial monitor to see what the arduino receives
//address will look like http://192.168.1.102:84 when submited
//for use with W5100 based ethernet shields

#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address
byte ip[] = { 
  192, 168, 1, 102 }; // ip in lan
byte gateway[] = { 
  192, 168, 1, 1 }; // internet access via router
byte subnet[] = { 
  255, 255, 255, 0 }; //subnet mask
EthernetServer server(84); //server port
String readString; 

//////////////////////

void setup(){

  Serial.begin(115200);

  // disable w5100 while setting up SD
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  Serial.print("Starting SD..");
  if(!SD.begin(4)) Serial.println("failed");
  else Serial.println("ok");

  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  //delay(2000);
  server.begin();
  Serial.println("Ready");
}

void loop(){
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string 
          readString += c; 
          //Serial.print(c);
        } 
        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          Serial.println(readString); //print to serial monitor for debuging 

            client.println("HTTP/1.1 200 OK"); //send new page
          //client.println("Content-Type: text/html");
          //client.println("Content-Type: image/jpeg");
          //client.println("Content-Type: image/gif");
          //client.println("Content-Type: application/x-javascript");
          client.println("Content-Type: text");

          client.println();

          //File myFile = SD.open("boom.htm");
          //File myFile = SD.open("HYPNO.JPG");
          //File myFile = SD.open("BLUEH_SL.GIF");
          //File myFile = SD.open("SERVOSLD.HTM");
          File myFile = SD.open("TEST.TXT");

          if (myFile) {

            byte clientBuf[64];
            int clientCount = 0;

            while(myFile.available())
            {
              clientBuf[clientCount] = myFile.read();
              clientCount++;

              if(clientCount > 63)
              {
                // Serial.println("Packet");
                client.write(clientBuf,64);
                clientCount = 0;
              }
            }
            //final <64 byte cleanup packet
            if(clientCount > 0) client.write(clientBuf,clientCount);            
            // close the file:
            myFile.close();
          }
          delay(1);
          //stopping client
          client.stop();
          readString="";
        }
      }
    }
  } 
}
