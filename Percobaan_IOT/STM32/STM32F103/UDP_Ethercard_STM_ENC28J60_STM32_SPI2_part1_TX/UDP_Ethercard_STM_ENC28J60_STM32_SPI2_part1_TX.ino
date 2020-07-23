// This is a demo of the RBBB running as webserver with the Ether Card
// 2010-05-28 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
//
//
//-----------------------------------------------------------------
//  Ported to STM32F103 by Vassilis Serasidis on 21 May 2015
//  Home:  http://www.serasidis.gr
//  email: avrsite@yahoo.gr
//
// PIN Connections (Using STM32F103):
//
// ENC28J60 -  STM32F103
//   VCC    -    3.3V
//   GND    -    GND
//   SCK    -    Pin PB13
//   SO     -    Pin PB14
//   SI     -    Pin PB15
//   CS     -    Pin PA8
//-----------------------------------------------------------------
//
//

#include <EtherCard_STM.h>
#include <SPI.h>
#include <UIPEthernet.h>

#define STATIC 1 
EthernetUDP Udp;
const unsigned int localPort = 8888;
char remote_IP[] = "192.168.1.130";
int remote_Port = 8888;

char UDP_TX_Buffer[80]; // For future use...
//
char recvdBuffer[900];


#define STATIC 1

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x7A, 0xFA, 0xFE, 0x2E, 0xFB, 0x3F };
static byte myip[] = { 192, 168, 1, 135 };

byte buffer[500];
//BufferFiller bfill;

void setup () {
  Serial1.begin(115200);
  Serial1.println("started");
  delay(10);
  if (ether.begin(sizeof buffer, mymac, PB12) == 0)
    Serial1.println(F("Failed to access Ethernet controller"));
  ether.staticSetup(myip);
  Udp.begin(localPort);
  //if (!ether.dhcpSetup())
  //  Serial1.println("DHCP failed");

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
}



void loop () {

//  ether.packetLoop(ether.packetReceive());
  Udp.beginPacket(remote_IP, remote_Port);
  Udp.write("test");
  Udp.endPacket();

}
