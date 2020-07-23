#include <SPI.h>
#include <UIPEthernet.h>
#include <itoa.h>
#include <HCSR04.h>
#include <DS1302.h>
#include <EEPROM.h>
#include <SD.h>


uint16_t Status;
uint16_t Data; //Untuk storage
uint16_t DataWrite; // untuk penulisan
uint16_t AddressWrite = 5; // Alamat


/*
   Parsing Data Variable for UDP

*/
int recvdSize;
String dataIn2;
String dt2[10];
int i2; // Counter data parser UDP
boolean parsing2 = false;

/*
   Parsing Data Variable for Bluetooth
*/
String dataIn;
String dt[10];
int i; // Counter data parser Serial Bluetooth
boolean parsing = false;
int flag_menu = 0;
int flag_sub_menu = 0;
int flag_sub2_menu = 0;


/*********************************************************************************************************/
namespace {
const int kCePin   = PB11;  // Chip Enable
const int kIoPin   = PB10;  // Input/Output
const int kSclkPin = PB1;  // Serial1 Clock

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

uint8_t setjam, setmnt, setdtk, setthn, setbln, settgl, sethr;

String dayAsString(const uint8_t day) {
  switch (day) {
    case 1: return "Minggu";
    case 2: return "Senin";
    case 3: return "Selasa";
    case 4: return "Rabu";
    case 5: return "Kamis";
    case 6: return "Jumat";
    case 7: return "Sabtu";
  }
  return "(unknown day)";
}
char buf[50];
}

/*********************************************************************************************************/


/*************************************System Config********************************************************/
char ID[] = "PVJ202";
//byte mac[] = {0x2A, 0x00, 0x22, 0x22, 0x21, 0x21};
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
/*********************************************************************************************************/

char count_num[100];
int real_count;


/*********************************************************************************************************/
/*
  HCSR04(trigger, echo, temperature, distance)

  trigger     - trigger pin
  echo        - echo pin
  temperature - ambient temperature, in C
  distance    - maximun measuring distance, in cm
*/
HCSR04 ultrasonicSensor_01(PA1, PA0, 20, 400);
//HCSR04 ultrasonicSensor_01(PB4, PB5, 20, 400);

float distance1;
long duration1;

uint16_t pp_counter = 0;
uint16_t total_visitor = 0;
uint16_t mall_capacity = 1000;
uint16_t remain_visitor = 0;
float capacity_procent = 0;
uint16_t pp_in1, pp_in2, pp_in3, pp_in4, pp_in5;

uint16_t pp_out1, pp_out2;

uint16_t gate1 = 1, gate2 = 2, gate3 = 3, gate4 = 1, gate5 = 1, gate6 = 2, gate7 = 1;

uint16_t current_state1 = 0;
uint16_t current_state2 = 0;
uint16_t state_sensor1 = 0;
uint16_t state_sensor2 = 0;
uint16_t previous_state1 = 0;
uint16_t flag_counter = 0;
uint16_t state_io = 0;

float calibrate1 = 250;
uint16_t waiting_pp = 1000;
/*********************************************************************************************************/

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial1.println("Server Start!");
  //  Ethernet.init(PA4);
  Ethernet.begin(mac, ip);  // Set up the Ethernet Shield

  Udp.begin(localPort);
  Serial1.print("Start TC PC IP: ");
  Serial1.println(Ethernet.localIP());

  ultrasonicSensor_01.begin();
  rtc.writeProtect(false);
  rtc.halt(false);
  pp_in1=EEPROM.read(AddressWrite);
  Serial1.print("isi data: ");
  Serial1.println(pp_in1);
}

void loop() {
  read_data_serial();
  counter();
  printTime();
  udp_send();
}

/*********************************************************************************************************/
/*
  Counter Algorithm
*/
void counter() {
  distance1 = ultrasonicSensor_01.getDistance();
  if (distance1 <= calibrate1) {
    current_state1 = 1;
  } else {
    current_state1 = 0;
  }
  state_io = 2;
  if (previous_state1 != current_state1) {

    if (current_state1 == 1) {
      if (state_io == 1) {
        pp_in1 += 1;
        DataWrite = pp_in1;

        total_visitor++;
      } else if (state_io == 2) {
        pp_out1 -= 1;
      }

      //      Serial1.print(F("PP: "));
      //      Serial1.print(pp_counter);
      //
      //      Serial1.print(F("\tS1: "));
      //      Serial1.print(distance1);
      //      Serial1.print(F("\tcapasity: "));
      //      Serial1.println(capacity_procent);
      //      DataWrite = pp_in1;

      delay(waiting_pp);
    }
    previous_state1 = current_state1;
  }
  Serial1.print("epprom: ");
  Serial1.println((String)pp_counter);
  EEPROM.write(AddressWrite, pp_counter);
  pp_counter = pp_in1 + pp_in2 + pp_in3 - pp_out1;
  remain_visitor = mall_capacity - pp_in1;
  capacity_procent = (float) pp_counter / mall_capacity * 100;
}
/*********************************************************************************************************/


/*********************************************************************************************************/
/*
  RTC
*/
void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.

  snprintf(buf, sizeof(buf), "Tanggal: %s, %04d-%02d-%02d \n Jam: %02d.%02d.%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to Serial1 so we can see the time.
  //  Serial1.println(buf);
}
/*********************************************************************************************************/


/*********************************************************************************************************/
/*
  UDP Send
*/
void udp_send() {
  // put your main code here, to run repeatedly:
  // put your main code here, to run repeatedly:
  //  recvdSize = Udp.parsePacket();

 

  Udp.beginPacket(remote_IP, remote_Port);

  //    Udp.write(">ID:");
  //    Udp.write(ID);
  //    Udp.write(">ct:");
  //    Udp.write(count_num);
  //    Udp.write(";data1:");
  //    Udp.write(UDP_TX_Buffer);
  //    Udp.write(";data2:");
  //    Udp.write(UDP_TX_Buffer);
  //    Udp.write(";\n");

//  Udp.write("*");
//  Udp.write(ID);
//
//  Udp.write(",");
//  Udp.write(count_num); // counter number of send
//
//  Udp.write(",");
//  Udp.write(buf); // date counting
//
//  Udp.write(",");
//  Udp.write(pp_counter); // data people counter
//  Udp.write(",");
//  Udp.write(current_state1);
//  Udp.write("#\n");

Udp.print("*");
  Udp.print(ID);

  Udp.print(",");
  Udp.print(count_num); // counter number of send

  Udp.print(",");
  Udp.print((String)buf); // date counting

  Udp.print(",");
  Udp.print(pp_counter); // data people counter
  Udp.print(",");
  Udp.print(current_state1);
  Udp.print("#\n");

  Udp.endPacket();

  Serial1.print("ID: ");
  Serial1.print(ID);
  Serial1.print("\t");
  Serial1.print("counter: ");
  Serial1.print(count_num);
  Serial1.print("\t");
  //  Serial1.print("tanggal: ");
  Serial1.print(buf);
  Serial1.print("\t");
  Serial1.print("PP: ");
  Serial1.print(pp_counter);
  Serial1.print("\n");
  //
  strcpy(UDP_TX_Buffer, "");

  //    delay(200);
}
/*********************************************************************************************************/




/*********************************************************************************************************/

/*********************************************************************************************************/
/*
  Read Data Serial
*/
void read_data_serial() {
  if (Serial1.available() > 0)
  {
    char inChar = (char)Serial1.read();
    dataIn += inChar;
    if (inChar == '\n') {
      parsing = true;
    }
  }
  if (parsing)
  {
    parsingData();
    parsing = false;
    dataIn = "";
  }
}
/*********************************************************************************************************/


void parsingData()
{
  int j = 0;
  //kirim data yang telah diterima sebelumnya
  //inisialisasi variabel, (reset isi variabel)
  dt[j] = "";
  //proses parsing data
  for (i = 0; i < dataIn.length(); i++)
  {
    //pengecekan tiap karakter dengan karakter (#) dan (,)
    if ((dataIn[i] == '\n') || (dataIn[i] == ','))
      //    if ((dataIn[i] == ','))
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
  /*
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
    Serial1.print("data 5 : ");
    Serial1.print(dt[4].toInt());
    Serial1.print("\n\n");
  */

  /*********************************************************************************************************/
  //Menu >0 Help
  if (dt[0].toInt() == 0) {
    flag_menu = 0;
    flag_sub_menu = 0;
    flag_sub2_menu = 0;
    Serial1.println("**************************************");
    Serial1.println("+++People Counter versi one way direction+++");
    Serial1.println("+++Copyright @2020 by Irostech Group+++");
    Serial1.println("**************************************");
    Serial1.println("Choose your setting use e.g. '1A'");
    Serial1.println("**************************************");
    Serial1.println("0. Help");
    Serial1.println("1. Set up static ip");
    Serial1.println("2. Set Up Threshold Object");
    Serial1.println("**************************************");
  }
  /*********************************************************************************************************/


  /*********************************************************************************************************/
  //Menu >1 Help
  if (dt[0].toInt() == 1) {
    flag_menu = 1;
  }
  //Menu >2 Help
  if (dt[0].toInt() == 2) {
    flag_menu = 2;
  }
  /*********************************************************************************************************/


  /*********************************************************************************************************/
  // Pilihan menu untuk Help 1
  if (flag_menu == 1) {
    flag_sub_menu = 1;
    Serial1.println("**************************************");
    Serial1.println("Chooese your setting use e.g. '1A' ");
    Serial1.println("**************************************");
    Serial1.println("1A. Define Static Local IP");
    Serial1.println("1B. Define Static Remote IP");
    Serial1.println("1C. Define Static Local Port");
    Serial1.println("**************************************");
    flag_menu = 0;
  }

  // Pilihan "Eksekusi" Menu untuk Help 1
  if (flag_sub_menu == 1) {
    //1A. Define Static Local IP
    if (dt[0] == "1A" ) {
      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("**************************************");
      Serial1.println("Set Your Local IP e.g. 192,168,1,123 (sparete comma)");
      Serial1.println("**************************************");
      //      flag_sub_menu = 0;
      flag_menu = 0;
      flag_sub2_menu = 1;
    }
    if ( flag_sub2_menu == 1 & dt[0] == "192" ) {

      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.print("-->Your Setting Local IP is: ");
      Serial1.print(dt[0].toInt());
      Serial1.print(".");
      Serial1.print(dt[1].toInt());
      Serial1.print(".");
      Serial1.print(dt[2].toInt());
      Serial1.print(".");
      Serial1.print(dt[3].toInt());
      Serial1.print("\n");

    }
    //1B. Define Static Remote IP
    if (dt[0] == "1B" ) {
      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("**************************************");
      Serial1.println("Set Your Remote IP e.g. 192,168,1,123 (sparete comma)");
      Serial1.println("**************************************");
      //      flag_sub_menu = 0;
      //Kayanya harusnya disini masukin IP
      flag_menu = 0;
      flag_sub2_menu = 2;
    }
    if ( flag_sub2_menu == 2 & dt[0] == "192" ) {

      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.print("-->Your Setting Remote IP is: ");
      Serial1.print(dt[0].toInt());
      Serial1.print(".");
      Serial1.print(dt[1].toInt());
      Serial1.print(".");
      Serial1.print(dt[2].toInt());
      Serial1.print(".");
      Serial1.print(dt[3].toInt());
      Serial1.print("\n");

    }

    //1C. Define Static Local Port
    if (dt[0] == "1C" ) {
      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("**************************************");
      Serial1.println("Set Your Static Port e.g. 8888");
      Serial1.println("**************************************");
      //      flag_sub_menu = 0;
      flag_menu = 0;
      flag_sub2_menu = 3;
    }
    if ( flag_sub2_menu == 3 & dt[0].toInt() >= 0) {

      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.print("-->Your Port Setting is: ");
      Serial1.print(dt[0].toInt());

    }

  }
  /*********************************************************************************************************/


  /*********************************************************************************************************/
  // Pilihan menu untuk Help 2
  if (flag_menu == 2) {
    flag_sub_menu = 3;
    Serial1.println("**************************************");
    Serial1.println("Chooese your Threshold detected Object in 'cm' use e.g. '125' ");
    Serial1.println("**************************************");
    flag_menu = 0;
  }

  // Pilihan "Eksekusi" Menu untuk Help 1
  if (flag_sub_menu == 3 & dt[0].toInt() >= 5) {

    Serial1.print("\n");
    Serial1.print("\n");
    Serial1.print("--> Your Set Up Threshold is: ");
    Serial1.print(dt[0].toInt());
    Serial1.println("cm");
  } else if (flag_sub_menu == 3 & dt[0].toInt() < 5) {
    Serial1.println("please make threshold more than 5 cm!");
  }
  /*********************************************************************************************************/

 
  /*********************************************************************************************************/

}
