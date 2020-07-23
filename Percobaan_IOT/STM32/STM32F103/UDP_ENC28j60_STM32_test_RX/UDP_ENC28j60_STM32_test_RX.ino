#include <EtherCard_STM.h>
#include <SPI.h>
#include <EthernetUdp.h>

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192, 168, 1, 120 };
// gateway ip address
static byte gwip[] = { 192, 168, 1, 1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer


// -------- Do not change the section below -----------------
unsigned int localPort = 1369;    // Process ID port (TCP/UDP Alternate port = 8888)
char remote_IP1[]="192.168.1.130";
int remote_Port1 = 9631;
char UDP_TX_Buffer[80];
char recvdBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; // Buffer for incoming data
EthernetUDP Udp;  //Class variable (Initiates process of UDP)
// ------------------------------------------------------------



const char page[] PROGMEM =
"HTTP/1.0 503 Service Unavailable\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
  "<head><title>"
    "Service Temporarily Unavailable"
  "</title></head>"
  "<body>"
    "<h3>Bro AYo Cari Sarapan</h3>"
    "<p><em>"
      "Sarapan Nasi Kuning.<br />"
      "Beli Galon Aqua."
    "</em></p>"
  "</body>"
"</html>"
;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);
  delay(10);
  Serial1.println("\nRX Computer]");

  if (ether.begin(sizeof Ethernet::buffer, mymac, PA4) == 0)
    Serial1.println( "Failed to access Ethernet controller");
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial1.println("DHCP failed");
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  Udp.begin(localPort);
}

  int recvdSize;

void loop() {
  // put your main code here, to run repeatedly:
  // if there's data available, read a packet
  recvdSize = Udp.parsePacket();
  
  if (Udp.available()) {
    IPAddress remote = Udp.remoteIP();
    Udp.read(recvdBuffer, UDP_TX_PACKET_MAX_SIZE);
    recvdBuffer[recvdSize] = '\0';
    recvdSize -= 8;               // Gets rid of the message header


    int temperature = atoi(recvdBuffer);

    // Conversion from celsius to feranheat

    int degreef = ((temperature) * 1.8000 + 32);

    Serial1.println(temperature);

  }else{
    Serial1.println("no data");
  }
  
    
  

  /*
  if (ether.packetLoop(ether.packetReceive())) {
    memcpy_P(ether.tcpOffset(), page, sizeof page);
    ether.httpServerReply(sizeof page - 1);
  }*/
}
