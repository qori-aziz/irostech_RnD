#include <UIPEthernet.h>
#include <SPI.h>
#include <EthernetUdp.h>
#include <itoa.h>

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192, 168, 1, 130 };
// gateway ip address
static byte gwip[] = { 192, 168, 1, 1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

const unsigned int localPort = 9631;

char remote_IP[] = "192.168.1.120";
int remote_Port = 1369;

char UDP_TX_Buffer[80]; // For future use...

char recvdBuffer[UDP_TX_PACKET_MAX_SIZE + 1];
//char UDP_RX_Buffer[80]; // For future use...
EthernetUDP Udp;

int reading;
float tempC;

void setup() {
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Serial1.begin(115200);
  delay(10);
  Serial1.println("\nTX Computer]");

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
//  recvdSize = Udp.parsePacket();

  reading = analogRead(PA0);
  tempC = reading / 9.31;
  Serial1.print("analog: ");
  Serial1.println((float)tempC);
  delay(1000);

    itoa(tempC, UDP_TX_Buffer, 10);
  
    Udp.beginPacket(remote_IP, remote_Port);
  
    Udp.write(UDP_TX_Buffer);
  
    Udp.endPacket();
//  
    strcpy(UDP_TX_Buffer, "");
  
    delay(200);


}
