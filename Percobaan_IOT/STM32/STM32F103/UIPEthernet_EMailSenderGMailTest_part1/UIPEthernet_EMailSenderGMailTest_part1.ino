//
//
///*
// * EMailSender library for Arduino, esp8266 and esp32
// * Simple esp8266 Gmail send example
// *
// * https://www.mischianti.org
// *
// */
//
//#include "Arduino.h"
//#include <SPI.h>
//#include <UIPEthernet.h>
//#include <SD.h>
//
//#include <EMailSender.h>
//
//
//
//
//uint8_t connection_state = 0;
//uint16_t reconnect_interval = 10000;
//
//EMailSender emailSend("ypn89ypn@gmail.com", "nugraha19101989");
//byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEA};
//IPAddress ip(192, 168, 1, 179);
//EthernetServer server(80);
//
//void setup()
//{
//    Serial11.begin(115200);
//    Serial11.println("testing:");
//    Ethernet.begin(mac, ip);
//
//    
//
//    EMailSender::EMailMessage message;
//    message.subject = "Soggetto";
//    message.message = "Ciao come stai<br>io bene.<br>www.mischianti.org";
//
//    EMailSender::Response resp = emailSend.send("yayanpnugraha@gmail.com", message);
//
//    Serial11.println("Sending status: ");
//    server.begin();
//    Serial11.println(resp.status);
//    Serial11.println(resp.code);
//    Serial11.println(resp.desc);
//}
//
//void loop()
//{
//
//}


#include "Arduino.h"
#include <SPI.h>
#include <UIPEthernet.h>
#include <SD.h>

#include <EMailSender.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//  EMailSender(const char* email_login, const char* email_password, const char* email_from, const char* smtp_server, uint16_t smtp_port, bool isSecure = false);
EMailSender emailSend("ypn89ypn@gmail.com", "nugraha19101989");

void printDirectory(File dir, int numTabs);

//The setup function is called once at startup of the sketch
void setup()
{
    Serial1.begin(115200);

    delay(2000);

    Serial1.println("Starting!");

    Serial1.print("Initializing SD card...");

    if (!SD.begin(4)) {
      Serial1.println("initialization failed!");
      while (1);
    }
    Serial1.println("initialization done.");

    File root = SD.open("/");

    printDirectory(root, 0);

    Serial1.println("done!");

//  File myFile = SD.open("/TEST.TXT", "r");
//    if(myFile) {
//      myFile.seek(0);
//      DEBUG_PRINTLN(F("OK"));
//    myFile.close();
//    }
//    else {
//      DEBUG_PRINTLN(F("KO"));
//    }
//
//
    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) {
      Serial1.println("Failed to configure Ethernet using DHCP");
      while(1);
    }
    Serial1.print("IP address ");
    Serial1.println(Ethernet.localIP());


    EMailSender::EMailMessage message;
    message.subject = "Soggetto";
    message.message = "Ciao come stai<br>io bene.<br>www.mischianti.org";
    message.mime = MIME_TEXT_PLAIN;

//    Two file
//    EMailSender::FileDescriptior fileDescriptor[2];
//    fileDescriptor[1].filename = F("test.txt");
//    fileDescriptor[1].url = F("/test.txt");
//    fileDescriptor[1].storageType = EMailSender::EMAIL_STORAGE_TYPE_SD;
//
//    fileDescriptor[0].filename = F("logo.jpg");
//    fileDescriptor[0].url = F("/logo.jpg");
//    fileDescriptor[0].mime = "image/jpg";
//    fileDescriptor[0].encode64 = true;
//    fileDescriptor[0].storageType = EMailSender::EMAIL_STORAGE_TYPE_SD;
//
//    EMailSender::Attachments attachs = {2, fileDescriptor};

//    One file
    EMailSender::FileDescriptior fileDescriptor[1];
    fileDescriptor[0].filename = F("test.txt");
    fileDescriptor[0].url = F("/test.txt");
    fileDescriptor[0].mime = MIME_TEXT_PLAIN;
    fileDescriptor[0].storageType = EMailSender::EMAIL_STORAGE_TYPE_SD;

    // Pay attention base64 encoding is quite slow
//  EMailSender::FileDescriptior fileDescriptor[2];
//  fileDescriptor[0].filename = F("logo.jpg");
//  fileDescriptor[0].url = F("/logo.jpg");
//  fileDescriptor[0].mime = "image/jpg";
//  fileDescriptor[0].encode64 = false;
//  fileDescriptor[0].storageType = EMailSender::EMAIL_STORAGE_TYPE_SD;

    EMailSender::Attachments attachs = {1, fileDescriptor};

    EMailSender::Response resp = emailSend.send("yayanpnugraha@gmail.com", message, attachs);

    Serial1.println("Sending status: ");

    Serial1.println(resp.status);
    Serial1.println(resp.code);
    Serial1.println(resp.desc);

    File root2 = SD.open("/");

    printDirectory(root2, 0);

    Serial1.println("done!");
    SD.end();
}

void loop()
{

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
