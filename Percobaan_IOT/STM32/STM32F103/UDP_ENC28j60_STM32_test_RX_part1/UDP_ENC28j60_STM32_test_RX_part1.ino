#include <SPI.h>
#include <UIPEthernet.h>
#include <itoa.h>

int recvdSize;

// -------- Do not change the section below -----------------
byte mac[] = {0x2A, 0x00, 0x22, 0x22, 0x22, 0x24};
IPAddress ip(192, 168, 1, 130);
unsigned int localPort = 8888;    // Process ID port (TCP/UDP Alternate port = 8888)
char remote_IP1[] = "192.168.1.120";
int remote_Port1 = 8888;
char UDP_TX_Buffer[80];
char recvdBuffer[900]; // Buffer for incoming data
EthernetUDP Udp;  //Class variable (Initiates process of UDP)
// ------------------------------------------------------------


void setup() {
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Serial1.begin(115200);
  Serial1.println("Client Start!");
  Ethernet.begin(mac, ip);  // Set up the Ethernet Shield
  Udp.begin(localPort);
  Serial1.print("Start TC PC IP: ");
  Serial1.println(Ethernet.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  recvdSize = Udp.parsePacket();
  //  Serial1.println(Udp.available());
  //  if (Udp.available()) {
  if (recvdSize) {
    IPAddress remote = Udp.remoteIP();
    Serial1.print("Sender: ");
    Serial1.println(Udp.remoteIP());
    //IPAddress remote = "192.168.1.120";
    Udp.read(recvdBuffer, 900);  // 900 are bufers for received storage
    recvdBuffer[recvdSize] = '\0';
    recvdSize -= 8;               // Gets rid of the message header
    Serial1.println((String)recvdBuffer);

    int temperature = atoi(recvdBuffer);

    // Conversion from celsius to feranheat

    int degreef = ((temperature) * 1.8000 + 32);

//    Serial1.println(temperature);

  } else {
    //Serial1.println("no data");
  }
}
