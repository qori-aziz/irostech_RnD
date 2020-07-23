//
//#include <SPI.h>
//#include <EtherCard_STM.h>
//
//#define STATIC 1
//static byte myip[] = { 192, 168, 1, 195 };
//static byte gwip[] = { 192, 168, 1, 1 };
//static byte dnsip[] = { 8, 8, 8, 8 };
//static byte netmask[] = { 255, 255, 255, 0 };
//static byte mymac[] = { 0x1F, 0x2E, 0x3C, 0xAD, 0x5E, 0x6F };
//byte Ethernet::buffer[700];
//static uint32_t timer;
//BufferFiller bfill;
//char website[] PROGMEM = "192.168.1.135";
//const int dstPort PROGMEM = 8888;
//
//const int srcPort PROGMEM = 8888;
//
//uint8_t ipDestinationAddress[] = { 192, 168, 1, 135 };
//
//void setup () {
//  Serial1.begin(115200);
//  Serial1.println("TX started");
//  delay(10);
//  if (ether.begin(sizeof Ethernet::buffer, mymac, PB12) == 0)
//    Serial.println( "Failed to access Ethernet controller");
//  //  //  if (!ether.dhcpSetup())
//  //  //    Serial.println("DHCP failed");
//  //
//  //  ether.staticSetup(myip, gwip);
//  ether.staticSetup(myip, gwip, dnsip, netmask);
//  //
//  //  ether.copyIp(ether.dnsip, dnsip);
//  ether.printIp("IP:  ", ether.myip);
//  ether.printIp("GW:  ", ether.gwip);
//  ether.printIp("DNS: ", ether.dnsip);
//  //
//  //  if (!ether.dnsLookup(website))
//  //    Serial.println("DNS failed");
//  //
//  ether.printIp("SRV: ", ether.hisip);
//  //
//  //  ether.parseIp(ipDestinationAddress, "192.168.1.135");
//}
//
//char textToSend[] = "test 123";
//
//static word homePage() {
//  long t = millis() / 1000;
//  word h = t / 3600;
//  byte m = (t / 60) % 60;
//  byte s = t % 60;
//  bfill = ether.tcpOffset();
//  bfill.emit_p(PSTR(
//                 "HTTP/1.0 200 OK\r\n"
//                 "Content-Type: text/html\r\n"
//                 "Pragma: no-cache\r\n"
//                 "\r\n"
//                 "<meta http-equiv='refresh' content='1'/>"
//                 "<title>RBBB server</title>"
//                 "<h1>$D$D:$D$D:$D$D</h1>"),
//               h / 10, h % 10, m / 10, m % 10, s / 10, s % 10);
//  return bfill.position();
//}
//
//void loop () {
//
//  //  ether.packetLoop(ether.packetReceive());
//  Serial1.println(textToSend);
//  //  if (millis() > timer) {
//  //    timer = millis() + 5000;
//  //static void sendUdp (char *data,uint8_t len,uint16_t sport, uint8_t *dip, uint16_t dport);
//  //  ether.sendUdp(textToSend, sizeof(textToSend), srcPort, ipDestinationAddress, dstPort );
//
//  //    word pos = ether.packetLoop(ether.packetReceive());
//  if (ether.packetLoop(ether.packetReceive())) {  // check if valid tcp data is received
//    ether.httpServerReply(homePage()); // send web page data
//
//  }
//  static byte remoteip[] = {192, 168, 1, 135};
//  static byte remoteip1[] = {255, 255, 255, 0};
//  ether.sendUdp(textToSend, sizeof(textToSend), 8888, remoteip, 8888 );
//  ether.sendUdp(textToSend, sizeof(textToSend), 8888, remoteip1, 8888 );
//  delay(1000);
//  //  }
//}


#include <EtherCard_STM.h>
#include <SPI.h>

#define STATIC 1

char textToSend[] = "test 123";
// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x7A, 0xFA, 0xFE, 0x2E, 0xFB, 0x3F };
static byte myip[] = { 192, 168, 1, 195 };

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

//static word homePage() {
//  long t = millis() / 1000;
//  word h = t / 3600;
//  byte m = (t / 60) % 60;
//  byte s = t % 60;
//  bfill = ether.tcpOffset();
//  bfill.emit_p(PSTR(
//    "HTTP/1.0 200 OK\r\n"
//    "Content-Type: text/html\r\n"
//    "Pragma: no-cache\r\n"
//    "\r\n"
//    "<meta http-equiv='refresh' content='1'/>"
//    "<title>RBBB server</title>"
//    "<h1>$D$D:$D$D:$D$D</h1>"),
//      h/10, h%10, m/10, m%10, s/10, s%10);
//  return bfill.position();
//}

void loop () {
//  ether.packetLoop(ether.packetReceive());
  static byte remoteip[] = {192, 168, 1, 135};
//  static byte remoteip1[] = {255, 255, 255, 0};
  ether.sendUdp(textToSend, sizeof(textToSend), 8888, remoteip, 8888 );
//  ether.sendUdp(textToSend, sizeof(textToSend), 8888, remoteip1, 8888 );
//  delay(1000);

}
