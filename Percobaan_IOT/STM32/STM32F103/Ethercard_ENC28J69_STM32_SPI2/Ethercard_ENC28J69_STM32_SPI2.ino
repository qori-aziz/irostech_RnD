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

#define STATIC 1 

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x7A,0xFA,0xFE,0x2E,0xFB,0x3F };
static byte myip[] = { 192,168,1,135 };

byte Ethernet::buffer[500];
BufferFiller bfill;

void setup () {
  Serial1.begin(115200);
  Serial1.println("started");
  delay(10);
  if (ether.begin(sizeof Ethernet::buffer, mymac, PB12) == 0)
    Serial1.println(F("Failed to access Ethernet controller"));
  ether.staticSetup(myip);
  
  //if (!ether.dhcpSetup())
  //  Serial1.println("DHCP failed");
  
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
}

static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='1'/>"
    "<title>RBBB server</title>" 
    "<h1>$D$D:$D$D:$D$D</h1>"),
      h/10, h%10, m/10, m%10, s/10, s%10);
  return bfill.position();
}

void loop () {
  
  if (ether.packetLoop(ether.packetReceive())){   // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
    
  }
}
