// Demonstrates usage of the new udpServer feature.
//You can register the same function to multiple ports, and multiple functions to the same port.
//
// 2013-4-7 Brian Lee <cybexsoft@hotmail.com>


//*************************************************************
//
// IT DOESN'T WORK ON STM32 YET (IS NOT PORTED YET).
//
//*************************************************************
#include <EtherCard_STM.h>
#include <IPAddress.h>

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 192,168,1,135 };
// gateway ip address
static byte gwip[] = { 192,168,1,1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x70,0x69,0x69,0x2B,0x30,0x3A };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

//callback that prints received packets to the Serial1 port
void udpSerialPrint(uint16_t dest_port, uint8_t src_ip[80], uint16_t src_port, const char *data, uint16_t len){
  IPAddress src(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);

  Serial1.print("dest_port: ");
  Serial1.println(dest_port);
  Serial1.print("src_port: ");
  Serial1.println(src_port);


  Serial1.print("src_ip: ");
  ether.printIp(src_ip);
  Serial1.println("data: ");
  Serial1.println(data);
}

void setup(){
  Serial1.begin(115200);
  Serial1.println(F("\nRX Started]"));

  if (ether.begin(sizeof Ethernet::buffer, mymac,PB12) == 0)
    Serial1.println(F("Failed to access Ethernet controller"));
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial1.println(F("DHCP failed"));
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

//  //register udpSerial1Print() to port 1337
//  ether.udpServerListenOnPort(&udpSerialPrint, 1234);
//
//  //register udpSerial1Print() to port 42.
//  ether.udpServerListenOnPort(&udpSerialPrint, 4321);
}

void loop(){
  //register udpSerial1Print() to port 1337
  ether.udpServerListenOnPort(&udpSerialPrint, 8888);

  //register udpSerial1Print() to port 42.
  ether.udpServerListenOnPort(&udpSerialPrint, 8888);
  //this must be called for ethercard functions to work.
  ether.packetLoop(ether.packetReceive());
}


/*
//Processing sketch to send test UDP packets.

import hypermedia.net.*;

 UDP udp;  // define the UDP object


 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true );     // <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message
 }

 void draw()
 {
 }

 void keyPressed() {
 String ip       = "192.168.0.200";  // the remote IP address
 int port        = 1337;    // the destination port

 udp.send("Greetings via UDP!", ip, port );   // the message to send

 }

 void receive( byte[] data ) {       // <-- default handler
 //void receive( byte[] data, String ip, int port ) {  // <-- extended handler

 for(int i=0; i < data.length; i++)
 print(char(data[i]));
 println();
 }
*/
