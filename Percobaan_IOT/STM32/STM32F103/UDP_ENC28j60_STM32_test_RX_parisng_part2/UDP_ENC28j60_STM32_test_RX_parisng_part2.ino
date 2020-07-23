#include <SPI.h>
#include <UIPEthernet.h>
#include <itoa.h>

int recvdSize;
String dataIn;
String dt[10];
int i;
boolean parsing = false;

// -------- Do not change the section below -----------------
byte mac[] = {0x2A, 0x00, 0x22, 0x22, 0x22, 0x24};
IPAddress ip(192, 168, 1, 201);
unsigned int localPort = 8888;    // Process ID port (TCP/UDP Alternate port = 8888)
char remote_IP1[] = "192.168.1.202";
int remote_Port1 = 8888;
char UDP_TX_Buffer[80];
char recvdBuffer[900]; // Buffer for incoming data
EthernetUDP Udp;  //Class variable (Initiates process of UDP)
// ------------------------------------------------------------


void setup() {
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Serial1.begin(9600);
  dataIn = "";
  Serial1.println("Client Start!");
  Ethernet.begin(mac, ip);  // Set up the Ethernet Shield
  Udp.begin(localPort);
  Serial1.print("Start TC PC IP: ");
  Serial1.println(Ethernet.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  recvdSize = Udp.parsePacket();
  if (recvdSize) {
    IPAddress remote = Udp.remoteIP();
    Udp.read(recvdBuffer, 900);  // 900 are bufers for received storage
    recvdBuffer[recvdSize] = '\0';
    recvdSize -= 8;               // Gets rid of the message header
    Serial1.println((String)recvdBuffer);
/*
    dataIn += recvdBuffer;
    parsing = true;
    if (parsing)
    {
      parsingData();
      Serial1.print("data 1 : ");
      Serial1.print(dt[0]);
      Serial1.print("\n");
      Serial1.print("data 2 : ");
      Serial1.print(dt[1].toInt());
      Serial1.print("\n");
      Serial1.print("data 3 : ");
      Serial1.print(dt[2].toInt());
      Serial1.print("\n");
      Serial1.print("data 4 : ");
      Serial1.print(dt[3].toInt());
      Serial1.print("\n");
      parsing = false;
      dataIn = "";
    }*/

  }
}

void parsingData()
{
  int j = 0;
  //kirim data yang telah diterima sebelumnya
  Serial1.print("data masuk : ");
  Serial1.print(dataIn);
  Serial1.print("\n");
  //inisialisasi variabel, (reset isi variabel)
  dt[j] = "";
  //proses parsing data
  for (i = 1; i < dataIn.length(); i++)
  {
    //pengecekan tiap karakter dengan karakter (#) dan (,)
    if ((dataIn[i] == '#') || (dataIn[i] == ','))
    {
      //increment variabel j, digunakan untuk merubah index array penampung
      j++;
      dt[j] = ""; //inisialisasi variabel array dt[j]
    }
    else
    {
      //proses tampung data saat pengecekan karakter selesai.
      dt[j] = dt[j] + dataIn[i];
    }
  }
  //kirim data hasil parsing
  //      Serial1.print("data 1 : ");
  //      Serial1.print(dt[0]);
  //      Serial1.print("\n");
  //      Serial1.print("data 2 : ");
  //      Serial1.print(dt[1].toInt());
  //      Serial1.print("\n");
  //      Serial1.print("data 3 : ");
  //      Serial1.print(dt[2].toInt());
  //      Serial1.print("\n");
  //      Serial1.print("data 4 : ");
  //      Serial1.print(dt[3].toInt());
  //      Serial1.print("\n");
  //      Serial1.print("data 5 : ");
  //      Serial1.print(dt[4].toInt());
  //      Serial1.print("\n\n");
}
