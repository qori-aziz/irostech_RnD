//Tx


#include <SPI.h>
#include <Ethernet.h>
//#include <EthernetUdp.h>
#include <UIPEthernet.h>
#include <itoa.h>

//PIN on board



int pinButton = 4;
int pinButton1= 5;
int flame =8;
int LED = 12;
int LED1 =13;
int stateLED,stateLED1,stateLED2 = LOW;
int stateButton,stateButton1,stateButton2;
int previous,previous1,previous2 = LOW;
long time = 0;
long debounce = 100;



//-------- system config -----------------
byte mac[] = {0x2A, 0x00, 0x22, 0x22, 0x22, 0x22};
IPAddress ip(192, 168, 1, 20);
const unsigned int localPort = 9631;

char remote_IP[] = "192.168.1.10";
int remote_Port = 1369;

char UDP_TX_Buffer[80]; // For future use...

char recvdBuffer[UDP_TX_PACKET_MAX_SIZE + 1];
//char UDP_RX_Buffer[80]; // For future use...
EthernetUDP Udp;
//-----------------------------------------

int analogPin = 3;
int firstpin=2;
int reading;
float tempC;

void setup()
{

  Ethernet.begin(mac, ip);  // Set up the Ethernet Shield
  Udp.begin(localPort);     // Open a socket for this port
  Serial.begin(9600);       // Set up serial monitor with PC
  //pinMode(13, OUTPUT);
// analogReference(INTERNAL);
 
pinMode(6,OUTPUT);
pinMode(7,OUTPUT);
pinMode(8,INPUT);
   pinMode(pinButton, INPUT);
   pinMode(pinButton1,INPUT);
  pinMode(LED, OUTPUT);
  pinMode(LED1,OUTPUT);
 

  delay(1000);


}

int newS1, oldS1 = 0, newS2, oldS2 = 0, newS3, oldS3 = 0, newS4, oldS4 = 0;

int sData_code = 1;
int codeS1 = 2; //Codes must be even
int codeS2 = 4;
int codeS3 = 6;
int codeS4 = 8;


void loop()
{
int recvdSize = Udp.parsePacket();

reading = analogRead(analogPin);
tempC = reading / 9.31;
Serial.println(tempC);
delay(1000);

 itoa(tempC, UDP_TX_Buffer, 10); 

   Udp.beginPacket(remote_IP, remote_Port);

   Udp.write(UDP_TX_Buffer);

   Udp.endPacket();

   strcpy(UDP_TX_Buffer,"");

   delay(200);


stateButton = digitalRead(pinButton); 
  if(stateButton == HIGH && previous == LOW && millis() - time > debounce) {
    if(stateLED == HIGH){
      remote_Send(codeS1);
      stateLED = LOW;
    } else {
       stateLED = HIGH;
       remote_Send(codeS1 + 1);
    }
    time = millis();
  }
  digitalWrite(LED, stateLED);
  previous == stateButton;


  stateButton1 = digitalRead(pinButton1); 
  if(stateButton1 == HIGH && previous1 == LOW && millis() - time > debounce) {
    if(stateLED1 == HIGH){
      remote_Send(codeS2);
      stateLED1 = LOW;
    } else {
       stateLED1 = HIGH;
       remote_Send(codeS2 + 1);
    }
    time = millis();
  }
  digitalWrite(LED1, stateLED);
  previous1 == stateButton1;



stateButton2 = digitalRead(flame); 
  if(stateButton2 == HIGH && previous2 == LOW && millis() - time > debounce) {
    if(stateLED2 == HIGH){
      remote_Send(codeS3);
      stateLED2 = LOW;
    } else {
       stateLED2 = HIGH;
       remote_Send(codeS3 + 1);
    }
    time = millis();
  }
  previous == stateButton;





if(strcmp(recvdBuffer,"2")==0)
{

 digitalWrite(6,HIGH);
 delay(500);
 
}

else if(strcmp(recvdBuffer,"3")==0)
{
  digitalWrite(6,LOW);
  delay(500);
}

/* 
//Conversion of celcius to fernhit

  float val = analogRead(analogPin);
  int degreeF = (val*(9/5)+32);
  char text;

  Serial.println(degreeF);  

  itoa(degreeF, UDP_TX_Buffer, 10); 

   Udp.beginPacket(remote_IP, remote_Port);

   Udp.write(UDP_TX_Buffer);

   Udp.endPacket();

   strcpy(UDP_TX_Buffer,"");

   delay(200);


if(digitalRead(firstpin)==HIGH)
{
  Udp.beginPacket(remote_IP, remote_Port);
 strcpy(UDP_TX_Buffer,"First"); 
 Udp.write(UDP_TX_Buffer);
  Udp.endPacket();
  strcpy(UDP_TX_Buffer,"");
}

*/
   //Receive data

   
   
  if (Udp.available()) {
    IPAddress remote = Udp.remoteIP();
    Udp.read(recvdBuffer, UDP_TX_PACKET_MAX_SIZE);
    recvdBuffer[recvdSize] = '\0';
    recvdSize -= 8; 

 
    Serial.println(recvdBuffer);
  

  }
}
  
  /*
  if (digitalRead(control) == HIGH)
  {
    read_inputs();

    if (newS1 != oldS1)
    {
      if (newS1 == 1)
        remote_Send(codeS1);
     else if(newS1==0)
        remote_Send(codeS1 + 1);

      delay(200);
    }
    oldS1=newS1;

    if (newS2 != oldS2)
    {
      if (newS2 == 1)
        remote_Send(codeS2);
  /*    else
        remote_Send(codeS2 + 1);

      delay(200);
    }

    if (newS3 != oldS3)
    {
      if (newS3 == 1)
        remote_Send(codeS3);
  /*    else
        remote_Send(codeS3 + 1);

      delay(200);
    }

    if (newS4 != oldS4)
    {
      if (newS4 == 1)
        remote_Send(codeS4);
  /*    else
        remote_Send(codeS4 + 1);

      delay(200);
    }



  }
  else
  {
    Serial.println("E");
    delay(2000);
    //Future purpose

  }

}

*/


void remote_Send(int data)
{
  itoa(data, UDP_TX_Buffer, 10);
  Udp.beginPacket(remote_IP, remote_Port); // REMOTE IP/Port
  Udp.write(UDP_TX_Buffer);                         //Byte/string
  Udp.endPacket();
  delay(200);
}

/*void read_inputs()
{
  if (Serial.available())
  {
    char sData = Serial.read();
    remote_Send(sData_code);
    delay(50);
    remote_Send(sData);
  }
  oldS1 = newS1;
  if (digitalRead(s1) == HIGH)
    newS1 = 1 ;
  else
    newS1 = 0 ;

  oldS2 = newS2;
  if (digitalRead(s2) == HIGH)
    newS2 = 1 ;
  else
    newS2 = 0 ;

  oldS3 = newS3;
  if (digitalRead(s3) == HIGH)
    newS3 = 1 ;
  else
    newS3 = 0 ;

  oldS4 = newS4;
  if (digitalRead(s4) == HIGH)
    newS4 = 1 ;
  else
    newS4 = 0 ;

}
*/
