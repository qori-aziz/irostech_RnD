#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

EthernetUDP Udp;
byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x5C, 0x18};
IPAddress ip(192, 168, 1, 202);
unsigned int localport = 1234;

IPAddress remoteIP(192, 168, 1, 201);
unsigned int remotePort = 8888;

void setup() {
  Serial.begin(9600);
  Serial.println("****************");
  Serial.println("Puerto Serial OK");
  Ethernet.begin(mac, ip);
  Serial.print("IP : ");
  Serial.println(Ethernet.localIP());
  Udp.begin(localport);
}

void loop() {
  Udp.beginPacket(remoteIP, remotePort);
  Udp.println("PVJ202,127,141,1,Selasa:2020-07-12;09.50.01#");
   Serial.println("*PVJ202,128,40,1#");
  Udp.endPacket();

  delay(1000);

  Udp.beginPacket(remoteIP, remotePort);
  Udp.println("PVJ211,128,942,1, Selasa:2020-07-12;09.51.02#");
  Udp.endPacket();

  delay(1000);

  Udp.beginPacket(remoteIP, remotePort);
  Udp.println("PVJ212,129,343,1,Selasa:2020-07-12;09.52.03#");
  Udp.endPacket();

  delay(1000);
}
