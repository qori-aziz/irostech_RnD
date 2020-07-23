


/*
  Email client sketch for IDE v1.0.5 and w5100/w5200
  Posted 7 May 2015 by SurferTim
*/



#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>


char b64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// this must be unique
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x59, 0x67 };
// change network settings to yours
IPAddress ip( 192, 168, 1, 207 );
IPAddress gateway( 192, 168, 1, 1 );
IPAddress subnet( 255, 255, 255, 0 );

char server[] = "smtp.gmail.com";
int port = 465;

//char server[] = "mail.smtp2go.com"; 
//int port = 2525; // You can also try using Port Number 25, 8025 or 587.



File myFile;


EthernetClient client;

void setup()
{
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Ethernet.begin(mac);
  delay(2000);
  Serial.println(F("Ready. Press 'e' to send."));
  Serial.println(Ethernet.localIP());

  if (!SD.begin(4))
  {
    Serial.println("SD initialization failed!");
  }
  else
  {
    Serial.println("SD initialization done.");
  }



}

void loop()
{
  byte inChar;

  inChar = Serial.read();

  if (inChar == 'e')
  {
    if (sendEmail()) Serial.println(F("Email sent"));
    else Serial.println(F("Email failed"));
  }
}


byte sendEmail()
{
  byte thisByte = 0;
  byte respCode;

  if (client.connect(server, port) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }

  if (!eRcv()) return 0;

  Serial.println(F("Sending hello"));
  // replace 1.2.3.4 with your Arduino's ip
  client.println("EHLO 1.2.3.4");
  if (!eRcv()) return 0;

  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if (!eRcv()) return 0;

  Serial.println(F("Sending User"));
  // Change to your base64 encoded user
  client.println(F("eXBuODl5cG5AZ21haWwuY29t"));

  if (!eRcv()) return 0;

  Serial.println(F("Sending Password"));
  // change to your base64 encoded password
  client.println(F("bnVncmFoYTE5MTAxOTg5"));


  if (!eRcv()) return 0;

  // change to your email address (sender)
  Serial.println(F("Sending From"));
  client.println("MAIL From: <ypn89ypn@gmail.com>");
  if (!eRcv()) return 0;

  // change to recipient address
  Serial.println(F("Sending To"));
  client.println("RCPT To: <yayanpnugraha@gmail.com>");
  if (!eRcv()) return 0;

  Serial.println(F("Sending DATA"));
  client.println("DATA");
  if (!eRcv()) return 0;

  Serial.println(F("Sending email"));

  // change to recipient address
  client.println("<yayanpnugraha@gmail.com>");

  // change to your address
  client.println("<ypn89ypn@gmail.com>");

  client.println("Subject: Arduino email Text File"); //Arduino email test\r\n

  //client.println("This is from my Arduino!");


  client.write("MIME-Version: 1.0\r\n");
  client.write("Content-Type: Multipart/mixed; boundary=frontier\r\n\r\n");

  client.write("--frontier\r\n");
  client.write("Content-Type: text/plain\r\n\r\n");

  client.write("This is text file from my Arduino!\r\n");

  client.write("--frontier\r\n");
  client.write("Content-Type: text/plain\r\n");
  //  client.write("Content-Type: image/jpeg\r\n");
  client.write("Content-Disposition: attachment; filename=test.txt\r\n");

  // this is where you would send your file
  int clientCount = 0;



  myFile = SD.open("test.txt");
  int myint;

  if (myFile)
  {
    while (myFile.available())
    {
      myint = myFile.read();
      client.write(myint);
      Serial.write(myint);
    }
    client.print("\r\n");
    myFile.close();

  }
  else {
    Serial.println(F("File open failed"));
  }



  //  client.write("This is a test\r\n");

  client.write("--frontier--\r\n");




  client.println(".");

  if (!eRcv()) return 0;

  Serial.println(F("Sending QUIT"));
  client.println("QUIT");
  if (!eRcv()) return 0;

  client.stop();

  Serial.println(F("disconnected"));

  return 1;
}

byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();

  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
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

  client.println(F("QUIT"));

  while (!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }

  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  client.stop();

  Serial.println(F("disconnected"));
}
