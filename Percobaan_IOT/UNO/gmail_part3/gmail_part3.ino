/*
   EMailSender library for Arduino, esp8266 and esp32
   Arduino Mega and UIPEthernet send example with Sendgrid provider

   Pay attention you must set in the library
   #define DEFAULT_EMAIL_NETWORK_TYPE_ARDUINO 	NETWORK_ENC28J60
   for UIPEthernet

   #define DEFAULT_EMAIL_NETWORK_TYPE_ARDUINO 	NETWORK_W5100
   for standard Ethernet


   https://www.mischianti.org

*/

#include "Arduino.h"
#include <SPI.h>
//#include <UIPEthernet.h>
#include <EMailSender.h>

//#define DEFAULT_EMAIL_NETWORK_TYPE_ARDUINO   NETWORK_W510K

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EMailSender emailSend("smtp.ypn89ypn@gmail.com", "nugraha19101989");

IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  //    while (!Serial) {}

  delay(200);

  Serial.println("Starting!");

  //    // start the Ethernet connection:
  //    if (Ethernet.begin(mac) == 0) {
  //      Serial.println("Failed to configure Ethernet using DHCP");
  //      while(1);
  //    }

  Ethernet.begin(mac, ip);
  Serial.print("IP address ");
  Serial.println(Ethernet.localIP());
  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  /*
    EMailSender::EMailMessage message;
    message.subject = "Soggetto";
    message.message = "Ciao come stai<br>io bene.<br>www.mischianti.org";

    EMailSender::Response resp = emailSend.send("yayanpnugraha@gmail.com", message);

    Serial.println("Sending status: ");

    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);
  */
}
int counter;
void loop()
{
  counter++;
  Serial.print("Trial: ");
  Serial.println(counter);
  EMailSender::EMailMessage message;
  message.subject = "Soggetto";
  message.message = "Ciao come stai<br>io bene.<br>www.mischianti.org";

  EMailSender::Response resp = emailSend.send("yayanpnugraha@gmail.com", message);

  Serial.println("Sending status: ");

  Serial.println(resp.status);
  Serial.println(resp.code);
  Serial.println(resp.desc);
  delay(10000);
}
