//Rx


#include <SPI.h>    // ( Net surf for SPI pins )
#include <Ethernet.h>
#include <EthernetUdp.h>
#include<LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 5, 4, 3, 2);
//code start

int pinButton = 14;
int pinButton1 = 15;

int stateLED, stateLED1 = LOW;
int stateButton, stateButton1;
int previous, previous1 = LOW;
long time = 0;
long debounce = 100;
int codeS1 = 2; //Codes must be even
int codeS2 = 4;






//Device IP & MAC (Variable)
byte mac[] = { 0x2A, 0x00, 0x22, 0x22, 0x22, 0x44 }; //(Naming rules)
IPAddress ip(192, 168, 1, 10);                       //(IP rules according to subnet/gateway)

// -------- Do not change the section below -----------------
const unsigned int localPort = 1369;    // Process ID port (TCP/UDP Alternate port = 8888)
char remote_IP1[] = "192.168.1.20";
int remote_Port1 = 9631;
char UDP_TX_Buffer[80];
char recvdBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; // Buffer for incoming data
EthernetUDP Udp;  //Class variable (Initiates process of UDP)
// ------------------------------------------------------------




int temperature = atoi(recvdBuffer);
int flamedet = 0;

int threshold = 400;






void setup()
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Welcome");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.clear();
  lcd.print("Manless Subtration");
  lcd.setCursor(0, 2);
  lcd.print("Optical Fiber");
  delay(2000);
  //lcd.print(temperature);
  pinMode(6, OUTPUT);
  pinMode(pinButton, INPUT);
  pinMode(pinButton1, INPUT);
  pinMode(7, OUTPUT);
  Ethernet.begin(mac, ip);   // Set up the Ethernet Shield
  Udp.begin(localPort);      // Open a socket for this port
  Serial.begin(9600);        // Set up serial monitor with PC4
  delay(500);
}
char first;
int i = 0, flag = 0;
char text;



void loop()
{

  //transmit
  int recvdSize = Udp.parsePacket();
  int count, count1, flag1;
  int Pressed = 0;
  int incomingByte = 0;
  /*
    //flam detector

    int sensorvalue;
    sensorvalue=analogRead(flamedet);

    Serial.println(sensorvalue);
    delay(1);

    if(sensorvalue>threshold)
    {
    Serial.print("low");

    }
    else
    Serial.print("high");
  */

  stateButton = digitalRead(pinButton);
  if (stateButton == HIGH && previous == LOW && millis() - time > debounce) {
    if (stateLED == HIGH) {
      lcd.print("One");
      remote_send(codeS1);
      stateLED = LOW;
    } else {
      stateLED = HIGH;
      remote_send(codeS1 + 1);
      lcd.print("two");
    }
    time = millis();
  }

  previous == stateButton;


  stateButton1 = digitalRead(pinButton1);
  if (stateButton1 == HIGH && previous1 == LOW && millis() - time > debounce) {
    if (stateLED1 == HIGH) {
      remote_send(codeS2);
      stateLED1 = LOW;
    } else {
      stateLED1 = HIGH;
      remote_send(codeS2 + 1);
    }
    time = millis();
  }

  previous1 == stateButton1;







  //compare

  //strcpy (recvdBuffer,"");

  if (strcmp(recvdBuffer, "3") == 0)
  {
    lcd.clear();
    digitalWrite(6, HIGH);
    lcd.print("Relay 1 ON ");
    delay(1000);

  }
  else if (strcmp(recvdBuffer, "2") == 0)
  {
    lcd.clear();
    digitalWrite(6, LOW);
    lcd.print("Relay 1 OFF ");
    delay(1000);
  }

  else if (strcmp(recvdBuffer, "5") == 0)
  { lcd.clear();
    digitalWrite(7, HIGH);
    lcd.print("Relay 2 ON ");
    delay(1000);
  }

  else if (strcmp(recvdBuffer, "4") == 0)
  { lcd.clear();
    digitalWrite(7, LOW);
    lcd.print("Relay 2 OFF ");
    delay(1000);
  }

  else if (strcmp(recvdBuffer, "6") == 0)
  { lcd.clear();
    lcd.print("Flame Detected ");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Take necessary action");
    delay(1000);
  }

  else if (strcmp(recvdBuffer, "7") == 0)
  { lcd.clear();
    lcd.print("Flame Detected ");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Take action");
    delay(2000);
  }


  /*
    }
      first=1;
      itoa(first,UDP_TX_Buffer,10);
      Udp.beginPacket(remote_IP1,remote_Port1);
      Udp.write(UDP_TX_Buffer);
      Udp.endPacket();
      strcpy(UDP_TX_Buffer,"");
      delay(200);

  */




  if (Udp.available()) {
    IPAddress remote = Udp.remoteIP();
    Udp.read(recvdBuffer, UDP_TX_PACKET_MAX_SIZE);
    recvdBuffer[recvdSize] = '\0';
    recvdSize -= 8;               // Gets rid of the message header


    int temperature = atoi(recvdBuffer);

    // Conversion from celsius to feranheat

    int degreef = ((temperature) * 1.8000 + 32);

    lcd.clear();
    lcd.print("Temp (C)=> ");
    lcd.print(temperature);
    lcd.setCursor(0, 2);
    lcd.print("Temp (F)=> ");
    lcd.print(degreef);
    Serial.println(temperature);

  }
}
//Serial.print("Temp = > ");
///Serial.println(recvdBuffer);
/*if (flag == 1)
  {
  //  Serial.print((i++) % 100);
  // Serial.print("> ");
  //   Serial.print(" Control Message  >>  ASCII : ");
  //  Serial.print(recvdBuffer);
  //   Serial.print("  TEXT : ");
  Serial.print(temperature);
  // text = atoi(recvdBuffer);
  //Serial.println(text);

  flag = 0;
  }
  else
  {
  if (atoi(recvdBuffer) == 1)
  {
    flag = 1;
  }
  else
  {
  //  Serial.print((i++) % 100);
  //   Serial.print("> ");
  //    Serial.print("Control Code : ");
  //     Serial.println(recvdBuffer);  // Prints received data to serial
  }
  }
  }
  }
*/

void remote_send(int data)
{
  itoa(data, UDP_TX_Buffer, 10);
  Udp.beginPacket(remote_IP1, remote_Port1);
  Udp.write(UDP_TX_Buffer);
  Udp.endPacket();
  delay(200);

}
