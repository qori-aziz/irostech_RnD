#include <SPI.h>
#include <UIPEthernet.h>
#include <itoa.h>

//-------- system config -----------------
char ID[] = "123456";
byte mac[] = { 0xDE, 0xAB, 0xBE, 0xEF, 0xFE, 0xAD };
IPAddress ip(192, 168, 1, 202);
const unsigned int localPort = 8888;
char remote_IP[] = "192.168.1.201";
int remote_Port = 8888;

char UDP_TX_Buffer[80]; // For future use...
//
char recvdBuffer[900];
//char UDP_RX_Buffer[80]; // For future use...
EthernetUDP Udp;
//-----------------------------------------

int reading;
float tempC;
int recvdSize;
char count_num[100];
int real_count;

void setup() {
  // put your setup code here, to run once:
//  Serial1.begin(115200);
Serial1.begin(9600);
  Serial1.println("Server Start!");
  Ethernet.begin(mac, ip);  // Set up the Ethernet Shield
  Udp.begin(localPort);
  Serial1.print("Start TC PC IP: ");
  Serial1.println(Ethernet.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  // put your main code here, to run repeatedly:
  //  recvdSize = Udp.parsePacket();

  reading = analogRead(PA2);
  tempC = (float)reading / 9.31;
  Serial1.print("analog: ");
  Serial1.println(tempC);
  real_count++;
  delay(400);

  itoa(tempC, UDP_TX_Buffer, 10);
  itoa(real_count, count_num, 10);
  Serial1.print("isi buffer: ");
  Serial1.println(UDP_TX_Buffer);

  Udp.beginPacket(remote_IP, remote_Port);
  //Udp.beginPacket("192.168.1.201", 8888);
  //Udp.println("testing");
  //Udp.endPacket();
  //    Udp.write(">ID:");
  //    Udp.write(ID);
  //    Udp.write(">ct:");
  //    Udp.write(count_num);
  //    Udp.write(";data1:");
  //    Udp.write(UDP_TX_Buffer);
  //    Udp.write(";data2:");
  //    Udp.write(UDP_TX_Buffer);
  //    Udp.write(";\n");

  Udp.write("*");
  Udp.write(ID);
  Udp.write(",");
  Udp.write(count_num);
  Udp.write(",");
  Udp.write(UDP_TX_Buffer);
  Udp.write(",");
  Udp.write(UDP_TX_Buffer);
  Udp.write("#\n");

  Udp.endPacket();

  strcpy(UDP_TX_Buffer, "");

  //    delay(200);
}
