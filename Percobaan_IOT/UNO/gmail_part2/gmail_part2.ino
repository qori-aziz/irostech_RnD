/*
   This demo sketch will fail at the Gmail login unless your Google account has
   set the following option:

       Allow less secure apps: OFF

   which can be found in your Google account under Security. You will get an email
   warning from Google that the universe will implode if you allow this.

   see https://www.base64encode.org/ for encoding / decoding
*/
//
//////#include <base64.h>
//////#include <ESP8266WiFi.h>
////#include "Config.h"
//
//#include <SPI.h>
//#include <Ethernet.h>
//
//byte mac[] = {
//  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
//};
//IPAddress ip(192, 168, 1, 177);
//IPAddress myDns(192, 168, 1, 1);
//
///*
//   const char* _ssid = "Greypuss";
//   const char* _password = "mypassword";
//   const char* _GMailServer = "smtp.gmail.com";
//   const char* _mailUser = "mygmailaddress@gmail.com";
//   const char* _mailPassword = "my Google password";
//*/
//
//
//const char* fingerprint = "289509731da223e5218031c38108dc5d014e829b"; // For smtp.gmail.com
//
//EthernetClient client;
//
//// Forward declarations of functions (only required in Eclipse IDE)
//byte response();
//byte sendEmail();
//
//void setup() {
//  Serial.begin(115200);
//  delay(10);
//  Serial.println("testing started");
//
//
//  // Connect to WiFi network
//  Ethernet.begin(mac, ip, myDns);
//  Serial.println("");
//  Serial.println("My IP address: ");
//  Serial.println(Ethernet.localIP());
//
//  delay(1000);
//  sendEmail();
//}
//
//void loop() {
//  //Nothing to do here. Never send email in a loop. You will get blacklisted.
//}
//
//// Function send a secure email via Gmail
//byte sendEmail()
//{
////  client.setFingerprint(fingerprint); // not available in axTLS::WiFiClientSecure 4.2.2
//  // port 465=SSL 567=TLS; 587 not available with library 4.2.2
//  // this all needs Google security downgrading:
//  // https://myaccount.google.com/lesssecureapps?utm_source=google-account&utm_medium=web
//
//  /*
//    Gmail exposes port 465 for SMTP over SSL and port 587 for SMTP with STARTTLS.
//    The difference between these two is that SMTP over SSL first establishes a secure
//    SSL/TLS connection and conducts SMTP over that connection, and SMTP with STARTTLS
//    starts with unencrypted SMTP and then switches to SSL/TLS.
//    See https://stackoverflow.com/questions/17281669/using-smtp-gmail-and-starttls
//  */
//  Serial.println("Attempting to connect to GMAIL server");
//  if (client.connect("smtp.gmail.com", 465) == 1) {
//    Serial.println(F("Connected"));
//  } else {
//    Serial.print(F("Connection failed:"));
//    return 0;
//  }
//  if (!response())
//    return 0;
//
//  Serial.println(F("Sending Extended Hello"));
//  client.println("EHLO gmail.com");
//  if (!response())
//    return 0;
//
//  // We're not using port 567 in this demo
//  //Serial.println(F("STARTTLS"));
//  //if (!response())
//  //  return 0;
//  //Serial.println(F("Sending EHLO"));
//  //client.println("EHLO gmail.com");
//  //if (!response())
//  //  return 0;
//
//  Serial.println(F("Sending auth login"));
//  client.println("auth login");
//  if (!response())
//    return 0;
//
//  Serial.println(F("Sending User"));
//  // Change to your base64, ASCII encoded user
//  client.println("eXBuODl5cG5AZ21haWwuY29t");
//  if (!response())
//    return 0;
//
//  Serial.println(F("Sending Password"));
//  // change to your base64, ASCII encoded password
//  client.println("bnVncmFoYTE5MTAxOTg5");
//  if (!response())
//    return 0;
//
//  Serial.println(F("Sending From"));
//  // your email address (sender) - MUST include angle brackets
//  client.println(F("MAIL FROM: <ypn89ypn@gmail.com>"));
//  if (!response())
//    return 0;
//
//  // change to recipient address - MUST include angle brackets
//  Serial.println(F("Sending To"));
//  client.println(F("RCPT To: <yayanpnugraha@gmail.com>"));
//  // Repeat above line for EACH recipient
//  if (!response())
//    return 0;
//
//  Serial.println(F("Sending DATA"));
//  client.println(F("DATA"));
//  if (!response())
//    return 0;
//
//  Serial.println(F("Sending email"));
//  // recipient address (include option display name if you want)
//  client.println(F("To: Home Alone Group<ypn89ypn@gmail.com>"));
//
//  // change to your address
//  client.println(F("From: HomeAlone@gmail.com"));
//  client.println(F("Subject: Your Arduino\r\n"));
//  client.println(F("This email was sent securely via an encrypted mail link.\n"));
//  client.println(F("In the last hour there was: 8 activities detected. Please check all is well."));
//  client.println(F("This email will NOT be repeated for this hour.\n"));
//  client.println(F("This email was sent from an unmonitored email account - please do not reply."));
//  client.println(F("Love and kisses from Dougle and Benny. They wrote this sketch."));
//
//  // IMPORTANT you must send a complete line containing just a "." to end the conversation
//  // So the PREVIOUS line to this one must be a prinln not just a print
//  client.println(F("."));
//  if (!response())
//    return 0;
//
//  Serial.println(F("Sending QUIT"));
//  client.println(F("QUIT"));
//  if (!response())
//    return 0;
//
//  client.stop();
//  Serial.println(F("Disconnected"));
//  return 1;
//}
//
//// Check response from SMTP server
//byte response()
//{
//  // Wait for a response for up to X seconds
//  int loopCount = 0;
//  while (!client.available()) {
//    delay(1);
//    loopCount++;
//    // if nothing received for 10 seconds, timeout
//    if (loopCount > 30000) {
//      client.stop();
//      Serial.println(F("\r\nTimeout"));
//      return 0;
//    }
//  }
//
//  // Take a snapshot of the response code
//  byte respCode = client.peek();
//  while (client.available())
//  {
//    Serial.write(client.read());
//  }
//
//  if (respCode >= '4')
//  {
//    Serial.print("Failed in eRcv with response: ");
//    Serial.print(respCode);
//    return 0;
//  }
//  return 1;
//}


#include <SPI.h>
#include <Ethernet.h>

// this must be unique
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xE2, 0xA3 };  
// change network settings to yours
IPAddress ip( 192, 168, 1, 100 );  
IPAddress gateway( 192, 168, 1, 1 );
IPAddress subnet( 255, 255, 255, 0 );

//char server[] = "mail.smtp2go.com"; 
//char server[] = "smtp2go.com"; //smtpcorp.com // smtpcorp.com
char server[] = "smtpcorp.com"; //smtpcorp.com // smtpcorp.com
int port = 8025; // You can also try using Port Number 25, 8025 or 587 2525.

EthernetClient client;

void setup()
{
  Serial.begin(115200);
  Serial.println("started!");
  pinMode(4,OUTPUT); //4
  digitalWrite(4,HIGH);
  
  Ethernet.begin(mac, ip); 

 if(Ethernet.begin(mac) == 0) {Serial.println("DHCP failed");}
  else {Serial.println(Ethernet.localIP());}
  
  delay(2000);
  Serial.println("Ready. Press 'e' to send.");
}

void loop()
{
  byte inChar;

  inChar = Serial.read();

  if(inChar == 'e')
  {
      if(sendEmail()) Serial.println("Email sent");
      else Serial.println("Email failed");
  }
}

byte sendEmail()
{
  byte thisByte = 0;
  byte respCode;

int rtnVal = client.connect(server,port);

 if(rtnVal != 1) {
    if(rtnVal < 0){ Serial.println("DNS fail");
    }
    else {Serial.println("Connection failed");
  } }   
  else {
    Serial.println("connected");
    return 0;
  }

  if(!eRcv()) return 0;

  Serial.println("Sending hello");
// replace 1.2.3.4 with your Arduino's ip
  client.println("EHLO 198.168.1.100");
  if(!eRcv()) return 0;

  Serial.println("Sending auth login");
  client.println("auth login");
  if(!eRcv()) return 0;

  Serial.println("Sending User");
// Change to your base64 encoded user
  client.println("cHAucmVwb3J0QGlyb3N0ZWNoLmNvbQ==");

  if(!eRcv()) return 0;

  Serial.println("Sending Password");
// change to your base64 encoded password
  client.println("dGVrbmlzaWphZ281NDMyMQ==");

  if(!eRcv()) return 0;

// change to your email address (sender)
  Serial.println("Sending From");
  client.println("MAIL From: <pp.report@irostech.com>");
  if(!eRcv()) return 0;

// change to recipient address
  Serial.println("Sending To");
  client.println("RCPT To: <ypn@irostech.com>");
  if(!eRcv()) return 0;

  Serial.println("Sending DATA");
  client.println("DATA");
  if(!eRcv()) return 0;

  Serial.println("Sending email");

// change to recipient address
  client.println("To: You <ypn@irostech.com>");

// change to your address
  client.println("From: Me <pp.report@irostech.com>");

  client.println("Subject: Arduino email test\r\n");

  client.println("This is from my Arduino!");

  client.println(".");

  if(!eRcv()) return 0;

  Serial.println("Sending QUIT");
  client.println("QUIT");
  if(!eRcv()) return 0;

  client.stop();

  Serial.println("disconnected");

  return 1;
}

byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while(!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if(loopCount > 30000) {
      client.stop();
      Serial.println("\r\nTimeout");
      return 0;
    }
  }

  respCode = client.peek();

  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }

  if(respCode >= '4')
  {
    efail();
    return 0;  
  }

  return 1;
}


void efail()
{
  byte thisByte = 0;
  int loopCount = 0;

  client.println("QUIT");

  while(!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if(loopCount > 30000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }

  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }

  client.stop();

  Serial.println(F("disconnected"));
}
