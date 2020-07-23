
/*
   People Counter versi one way direction
   0. Help
   1. Set up static ip: local, remort, port
   2. Set Up Server / Client
   3. Set Up Threshold Object
   4. Set Up RTC
   5. Set Up Email and Password
   6. Start
   7. Stop
   8. Reset
   9. Status / info
   10. Change Mac Address
*/

#include <HCSR04.h>
#include <SPI.h>
#include <UIPEthernet.h>
#include <DS1302.h>
#include <itoa.h>
#define SPI1_NSS_PIN PA3
#include <EEPROM.h>
#include <SD.h>

/***************************************RTC**************************************************************/
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
char tanggal[8];
}

/*********************************************************************************************************/


/*********************************************************************************************************/
//byte mac[] = {0xDE, 0xAA, 0xBE, 0xFF, 0xFE, 0xF1};
//byte mac[] = {0x2A, 0x00, 0x22, 0x22, 0x22, 0x24};
//1
//byte mac[] = { 0xDE, 0xAB, 0xBE, 0xBF, 0xCE, 0xAD };
//IPAddress ip(192, 168, 1, 101);
//2
//byte mac[] = { 0xDE, 0xBB, 0xBE, 0xCF, 0xDE, 0xAD };
//IPAddress ip(192, 168, 1, 102);
//3
//byte mac[] = { 0xDE, 0xCB, 0xBE, 0xDF, 0xEE, 0xAD };
//IPAddress ip(192, 168, 1, 103);
//4
//byte mac[] = { 0xDE, 0xDB, 0xBE, 0xEF, 0xFE, 0xAD };
//IPAddress ip(192, 168, 1, 104);


byte mac[] = { 0xFE, 0xEA, 0xAE, 0xEF, 0xFE, 0xAD };
byte ip[] = { 192, 168, 1, 201 };
EthernetServer server(80);
unsigned int localPort = 8888;    // Process ID port (TCP/UDP Alternate port = 8888)
char remote_IP1[] = "192.168.1.202";
int remote_Port1 = 8888;
char UDP_TX_Buffer[80];
char recvdBuffer[200]; // Buffer for incoming data
EthernetUDP Udp;  //Class variable (Initiates process of UDP)
/*********************************************************************************************************/

/*********************************************************************************************************/
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

uint16_t pp_counter;
uint16_t total_visitor;
uint16_t total_visitor_daily;
uint16_t total_out;
uint16_t total_in;
uint16_t mall_capacity = 30000;
uint16_t remain_visitor = 0;
float capacity_procent = 0;
uint16_t pp_in1, pp_in2, pp_in3, pp_in4, pp_in5;

uint16_t pp_out1, pp_out2;

uint16_t address_gate_in1 = 5;

int gate1 = 1, gate2 = 2, gate3 = 3, gate4 = 1, gate5 = 1, gate6 = 2, gate7 = 1;

int current_state1 = 0;
int current_state2 = 0;
int state_sensor1 = 0;
int state_sensor2 = 0;
int previous_state1 = 0;
int flag_counter = 0;
int state_io = 0;



float calibrate1 = 250;
uint16_t waiting_pp = 700;
/*********************************************************************************************************/

/*********************************************************************************************************/
/*
  SD Card
*/
#define SDCARD_CS PA3
File root;
#define error(s) error_P(PSTR(s))
char clientline[BUFSIZ];
char name[17];
int indeks = 0;
boolean current_line_is_blank = true;
char c;
int number = 1;
/*********************************************************************************************************/

void error_P(const char* str) {
  Serial1.print(F("error: "));
  Serial1.println(str);

  while (1);
}

void setup() {
  Serial1.begin(9600);
  Serial1.println("mcu started!");
  dataIn = "";
  dataIn2 = "";
  ultrasonicSensor_01.begin(); //set trigger as output & echo pin as input
  Ethernet.init(PA4);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial1.print("server is at ");
  Serial1.println(Ethernet.localIP());

  rtc.writeProtect(false);
  rtc.halt(false);

  Udp.begin(localPort);
  //  Time t(2020, 7, 18, 12, 59, 59, 7); // aktifkan untuk update

  //  Time t(setthn, setbln, settgl, setjam, setmnt, setdtk, sethr); // aktifkan untuk update

  // Set the time and date on the chip.
  //  rtc.time(t);
  //pinMode(PA3, OUTPUT);

  //Membaca pp_in1 dari EEPROM self (server)
  //EEPROM.read(address_gate_in1, &pp_in1);
  pp_in1 = EEPROM.read(address_gate_in1);
  //  Ethernet.init(PA4);
  if (!SD.begin(SDCARD_CS)) {
    error("card.init failed!");
  }

  root = SD.open("/");
  printDirectory(root, 0);

}

void ListFiles(EthernetClient client, uint8_t flags, File dir) {
  client.println("<ul>");
  while (true) {
    File entry = dir.openNextFile();

    // done if past last used entry
    if (! entry) {
      // no more files
      break;
    }

    // print any indent spaces
    client.print("<li><a href=\"");
    client.print(entry.name());
    if (entry.isDirectory()) {
      client.println("/");
    }
    client.print("\">");

    // print file name with possible blank fill
    client.print(entry.name());
    if (entry.isDirectory()) {
      client.println("/");
    }

    client.print("</a>");
    /*
        // print modify date/time if requested
        if (flags & LS_DATE) {
           dir.printFatDate(p.lastWriteDate);
           client.print(' ');
           dir.printFatTime(p.lastWriteTime);
        }
        // print size if requested
        if (!DIR_IS_SUBDIR(&p) && (flags & LS_SIZE)) {
          client.print(' ');
          client.print(p.fileSize);
        }
    */
    client.println("</li>");
    entry.close();
  }
  client.println("</ul>");
}

// How big our line buffer should be. 100 is plenty!
#define BUFSIZ 20

int new_indeks;  // indeks arraty number dari client get
String new_text_buf; // string cilent get
String new_text_buf2;
String dt_http[20]; // hasil parsing
int j_array; // array penampung
void loop() { 
  read_data_serial(); // Parsing Data Worked
  counter();
  printTime();
  dataReceiveUDP();
  //Serial1.println(dt2[0]);
  calculation();
  server_pp();
  write_csv();
}
/*******************************Penulisan csv di SD card******************************
  Brief:Menulis ke dalam csv dengan format sesuai tanggal, mengembalikan variabel
  i yang menandakan baris

  Write csv disini dipanggil dengan parsing Data ethernet, jika didapat 1 maka baru di ubah
  /*************************************************************************************/

void write_csv() {
  //inisialisasi file (buka file atau buat baru sesuai tanggal)
  Serial.print(dt2[3]);
  if (dt2[3].toInt() == 1) {
    Serial.print("masuk1");
    printTime();
    String namafile = String(tanggal) + String(".csv");
    File myFile = SD.open(namafile, FILE_WRITE);

    //Jika dapat dibuka, write
    if (myFile) {
      Serial1.print("Writing to csv...");

      //simpan urutan
      myFile.print(String(number));
      //Ke kolom selanjutnya
      myFile.print(",");

      //simpan data angka
      myFile.print(String(pp_in1));
      myFile.print(",");

      myFile.print(String(pp_in2));
      myFile.print(",");

      myFile.print(String(pp_out1));
      myFile.print(",");

      myFile.print(String(pp_out2));
      myFile.print(",");

      myFile.print(String(total_out));
      myFile.print(",");

      myFile.println(String(total_in));
      // close the file:
      myFile.close();
      Serial1.println("done.");
      number = number + 1;
    } else {
      // if the file didn't open, print an error:
      Serial1.println("error opening csv");
      number = number;
    }

  }
}


//*************************************************************************************

//*************************************************************************************

void sd_card_printed() {
  //  indeks = 0;
  //  // If it isn't a new line, add the character to the buffer
  //  if (c != '\n' && c != '\r') {
  //    clientline[indeks] = c;
  //    indeks++;
  //    // are we too big for the buffer? start tossing out data
  //    if (indeks >= BUFSIZ)
  //      indeks = BUFSIZ - 1;
  //
  //    // continue to read more data!
  //    //          continue;
  //  }
  //  // got a \n or \r new line, which means the string is done
  //  clientline[indeks] = 0;
  //  // Print it out for debugging
  //  Serial1.println(clientline);



}

//void printDirectory(File dir, int numTabs) {
//   while(true) {
//     File entry =  dir.openNextFile();
//     if (! entry) {
//       // no more files
//       break;
//     }
//     for (uint8_t i=0; i<numTabs; i++) {
//       Serial1.print('\t');
//     }
//     Serial1.print(entry.name());
//     if (entry.isDirectory()) {
//       Serial1.println("/");
//       printDirectory(entry, numTabs+1);
//     } else {
//       // files have sizes, directories do not
//       Serial1.print("\t\t");
//       Serial1.println(entry.size(), DEC);
//     }
//     entry.close();
//   }
//}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial1.print('\t');
    }
    Serial1.print(entry.name());
    if (entry.isDirectory()) {
      Serial1.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial1.print("\t\t");
      Serial1.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void calculation() {

  total_out = pp_out1 + pp_out2;
  total_in = pp_in1 + pp_in2;
  total_visitor = total_in - total_out;
  total_visitor_daily = total_in;
  remain_visitor = mall_capacity - total_visitor;
  capacity_procent = (float) pp_counter / mall_capacity * 100;


}


/*********************************************************************************************************/
/*
  UDP Receiver Data
*/
void dataReceiveUDP() {
  // put your main code here, to run repeatedly:
  recvdSize = Udp.parsePacket();
  if (recvdSize) {
    IPAddress remote = Udp.remoteIP();
    Udp.read(recvdBuffer, 200);  // 900 are bufers for received storage
    recvdBuffer[recvdSize] = '\0';
    recvdSize -= 8;               // Gets rid of the message header
    //    Serial1.println((String)recvdBuffer);


    dataIn2 += recvdBuffer;
    parsing2 = true;
    if (parsing2)
    {
      parsingData2();

      if (dt2[0] == "PVJ202") {
        pp_out1 = dt2[2].toInt();
        //        delay(50);
        //        Serial1.print("pp_out1: ");
                Serial1.println(pp_out1);
      } else if (dt2[0] == "PVJ211") {
        pp_in2 = dt2[2].toInt();
        //        delay(50);
        //        Serial1.print("pp_in2: ");
        //        Serial1.println(pp_in2);
      } else if (dt2[0] == "PVJ212") {
        pp_out2 = dt2[2].toInt();
        //        delay(50);
        //        Serial1.print("pp_out2: ");
        //        Serial1.println(pp_out2);
      }



      //      Serial1.print("data 1 : "); // ID
      //      Serial1.print(dt2[0]);
      //      Serial1.print("\n");
      //      Serial1.print("data 2 : "); // counter
      //      Serial1.print(dt2[1].toInt());
      //      Serial1.print("\n");
      //      Serial1.print("data 2 : "); // pp_counter
      //      Serial1.print(dt2[2].toInt());
      //      Serial1.print("\n");
      //      Serial1.print("data 3 : "); // flag
      //      Serial1.print(dt2[3].toInt());
      //      Serial1.print("\n");

      parsing2 = false;
      dataIn2 = "";
      //      recvdBuffer[200] = "";
    }

  }
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
  /*
    snprintf(buf, sizeof(buf), "Tanggal: %s, %04d-%02d-%02d \n Jam: %02d.%02d.%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec); */
  snprintf(buf, sizeof(buf), "Tanggal: %s, %04d-%02d-%02d \n Jam: %02d.%02d.%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
  snprintf(tanggal, sizeof(tanggal), "%02d-%02d-%04d", t.date, t.mon, t.yr);

  // Print the formatted string to Serial1 so we can see the time.
  //  Serial1.println(buf);
}
/*********************************************************************************************************/



/*********************************************************************************************************/
/*
  Server HTTP
*/
void server_pp() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    //    Serial1.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    indeks = 0;
    new_text_buf = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //                Serial1.print(F("clientline: "));

        if (c != '\n' && c != '\r') {

          new_text_buf += c;

        }
        char filesname[50];

        //        Serial1.println(new_text_buf.substring(5, new_text_buf.indexOf('H')));

        if (client.read() == -1) {
          (new_text_buf.substring(5, new_text_buf.indexOf('H'))).toCharArray(filesname, 50);
          File file = SD.open(filesname, O_READ);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/octet-stream");
          client.println();

          char file_buffer[16];
          int avail;
          while (avail = file.available()) {
            int to_read = min(avail, 16);
            if (to_read != file.read(file_buffer, to_read)) {
              break;
            }
            // uncomment the Serial1 to debug (slow!)
            //Serial1.write((char)c);
            client.write(file_buffer, to_read);
          }
          file.close();
          break;
        }









        //Serial1.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYzPE HTML>");
          client.println("<html>");
          int sensorReading = analogRead(0);

          //           akan diisi calendar
          client.print("<p><center>") ;
          client.print(buf);
          client.print("</p>");



          client.print("<h1 style=\"text-align: center;\">Real-time People Counter.</h1>");
          //          client.print("<p>&nbsp;</p>");
          //          client.print("<p>&nbsp;</p>");
          client.print("<center>") ;
          client.print("<table style=\"width: 550px; height: 100px; background-color: #dcdcdc;\" border=\"5\">");

          // Probe 01
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Pengunjung Masuk</strong></span></center></td>");
          client.print("<td><center><span style=\"color: blue;\"><strong>");
          client.print(total_visitor);
          client.print("</strong></span></td></tr>");

          // Probe 02
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Pengunjung Keluar</strong></span></center></td>");
          client.print("<td><center><span style=\"color: green;\"><strong>");
          client.print(total_out);
          client.print("</strong></span></td></tr>");

          // Probe 03
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Sisa Kapasitas Gedung</strong></span></center></td>");
          client.print("<td><center><span style=\"color: red;\"><strong>");
          client.print(remain_visitor);
          client.print("</strong></span></td></tr>");


          // Probe 03
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Gate In 1</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");
          client.print(pp_counter);
          client.print("</strong></span></td></tr>");


          // Probe 04
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Gate In 2</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");
          client.print(pp_in2);
          client.print("</strong></span></td></tr>");

          // Probe 05
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Gate Out 1</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");
          client.print(pp_out1);
          client.print("</strong></span></td></tr>");

          // Probe 06
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Gate Out 2</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");
          client.print(pp_out2);
          client.print("</strong></span></td></tr>");

          // Probe 03
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Kapasitas Pengunjung</strong></span></center></td>");
          client.print("<td><center><span style=\"color: black;\"><strong>");
          client.print(mall_capacity);
          client.print("</strong></span></td></tr>");

          client.print("</tbody></table></center>");
          client.print("<h1 style=\"text-align: center;\">Visitor Report. </h1>");

          //          client.print("<p>&nbsp;</p>");
          //          client.print("<p>&nbsp;</p>");
          client.print("<center>") ;
          client.print("<table style=\"width: 450px; height: 69px; background-color: #dcdcdc;\" border=\"5\">");

          // Probe 01
          client.print("<tbody><tr><td><center><span style=\"color: black;\"><strong>Pengunjung Hari Ini</strong></span></center></td>");
          client.print("<td><center><span style=\"color: blue;\"><strong>");
          client.print(total_visitor_daily);
          client.print("</strong></span></td></tr>");
          client.print("</tbody></table></center>");

          //sd_card_printed()



          // Print it out for debugging
          char *filename;
          //          Serial1.println(F("clientline: "));
          //          Serial1.println(client.read());
          File file = SD.open(filename, O_READ);

          client.print(filename);
          //          Serial1.println(F("filename: "));
          //          Serial1.println(filename);
          client.print("<h2>Data Logger People Counter 30 Days");
          client.println(":</h2>");
          ListFiles(client, LS_SIZE, file);
          file.close();






          client.print("<h5 style=\"text-align: center;\">(C) Irostech @2020 People Counter V1.3 </h5>");




          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
        //        break;
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    //    Serial1.println("client disonnected");
  }
}

/*********************************************************************************************************/

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
      pp_in1 += 1;
      pp_counter++;
      //      if (state_io == 1) {
      //        pp_in1 += 1;
      //        pp_counter++;
      ////        total_visitor++;
      //      } else if (state_io == 2) {
      //        pp_out1 -= 1;
      //      }


      Serial1.print(F("PP: "));
      Serial1.print(pp_in1);

      Serial1.print(F("\tS1: "));
      Serial1.print(total_visitor);
      Serial1.print(F("\tcap: "));
      Serial1.print(remain_visitor);
      Serial1.print(F("\tcapasity: "));
      Serial1.println(capacity_procent);


      delay(waiting_pp);
    }
    previous_state1 = current_state1;
    EEPROM.write(address_gate_in1, pp_in1);
  }

  //  pp_counter = pp_in1;
  //  remain_visitor = mall_capacity - pp_in1;
  //  capacity_procent = (float) pp_counter / mall_capacity * 100;
}
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

/*********************************************************************************************************/
/*
  Parsing Data
*/
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
    Serial1.println("1. Set up static ip: local, remort, port");
    Serial1.println("2. Set Up Server / Client");
    Serial1.println("3. Set Up Threshold Object");
    Serial1.println("4. Set Up RTC");
    Serial1.println("5. Set Up Email and Password");
    Serial1.println("6. Start");
    Serial1.println("7. Stop");
    Serial1.println("8. Reset");
    Serial1.println("9. Status / info");
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
  //Menu >3 Help
  if (dt[0].toInt() == 3) {
    flag_menu = 3;
  }
  //Menu >4 Help
  if (dt[0].toInt() == 4) {
    flag_menu = 4;
  }
  //Menu >5 Help
  if (dt[0].toInt() == 5) {
    flag_menu = 5;
  }
  //Menu >6 Help
  if (dt[0].toInt() == 6) {
    flag_menu = 6;
  }
  //Menu >7 Help
  if (dt[0].toInt() == 7) {
    flag_menu = 7;
  }
  //Menu >8 Help
  if (dt[0].toInt() == 8) {
    flag_menu = 8;
  }
  //Menu >9 Help
  if (dt[0].toInt() == 9) {
    flag_menu = 9;
  }
  //Menu >10 Help
  if (dt[0].toInt() == 10) {
    flag_menu = 10;
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
    flag_sub_menu = 2;
    Serial1.println("**************************************");
    Serial1.println("Chooese your setting Server / Client use e.g. '2A' ");
    Serial1.println("2A. Server");
    Serial1.println("2B. Client");
    Serial1.println("**************************************");
    flag_menu = 0;
  }

  // Pilihan "Eksekusi" Menu untuk Help 1
  if (flag_sub_menu == 2) {
    //1A. Define Static Local IP
    if (dt[0] == "2A" ) {
      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("--> You Choosed as Server>");


    }
    if (dt[0] == "2B" ) {
      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("--> Youu Choosed as Client>");

    }
  }
  /*********************************************************************************************************/

  /*********************************************************************************************************/
  // Pilihan menu untuk Help 3
  if (flag_menu == 3) {
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
  // Pilihan menu untuk Help 4
  if (flag_menu == 4) {
    flag_sub_menu = 41;
    Serial1.println("**************************************");
    Serial1.println("Chooese your setting use e.g. '4A' ");
    Serial1.println("**************************************");
    Serial1.println("4A. Define Your Current Date");
    Serial1.println("4B. Define Your Current Time");
    Serial1.println("**************************************");
    flag_menu = 0;
  }

  // Check Eksekusi
  if (flag_sub_menu == 41) {
    // Pilihan "Eksekusi" Menu untuk Ubah Tanggal
    if (dt[0] == "4A" ) {
      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("**************************************");
      Serial1.println("Set Your Current Date Tahun,Bulan,Tanggal,Hari  e.g. 2020,05,31,0 (sparete comma)");
      Serial1.println("**************************************");
      Serial1.println("Daftar Konversi Hari:");
      Serial1.println("0. Minggu");
      Serial1.println("1. Senin");
      Serial1.println("2. Selasa");
      Serial1.println("3. Rabu");
      Serial1.println("4. Kamis");
      Serial1.println("5. Jum'at");
      Serial1.println("6. Sabtu");
      Serial1.println("**************************************");
      //      flag_sub_menu = 0;
      flag_menu = 0;
      flag_sub2_menu = 411;
    }
    if ( flag_sub2_menu == 411 & dt[0].toInt() >= 0 ) {

      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("**************************************");
      Serial1.print("--> Your Current Time Setting is: ");
      Serial1.print(dt[0].toInt());
      Serial1.print("/");
      Serial1.print(dt[1].toInt());
      Serial1.print("/");
      Serial1.print(dt[2].toInt());
      Serial1.print(", ");
      int hr = dt[3].toInt();
      switch (hr) {
        case 0:
          Serial1.println("Minggu");
          break;
        case 1:
          Serial1.println("Senin");
          break;
        case 2:
          Serial1.println("Selasa");
          break;
        case 3:
          Serial1.println("Rabu");
          break;
        case 4:
          Serial1.println("Kamis");
          break;
        case 5:
          Serial1.println("Jum'at");
          break;
        case 6:
          Serial1.println("Sabtu");
          break;
      }
      Serial1.println("**************************************");

    }
    // Pilihan "Eksekusi" Menu untuk Ubah Waktu
    if (dt[0] == "4B" ) {
      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("**************************************");
      Serial1.println("Set Your Current Time HH,MM,SS e.g. 23,40,32 (sparete comma)");
      Serial1.println("**************************************");
      //      flag_sub_menu = 0;
      flag_menu = 0;
      flag_sub2_menu = 412;
    }
    if ( flag_sub2_menu == 412 & dt[0].toInt() >= 0 ) {

      Serial1.print("\n");
      Serial1.print("\n");
      Serial1.println("**************************************");
      Serial1.print("--> Your Current Time Setting is: ");
      Serial1.print(dt[0].toInt());
      Serial1.print(":");
      Serial1.print(dt[1].toInt());
      Serial1.print(":");
      Serial1.print(dt[2].toInt());
      Serial1.print("\n");
      Serial1.println("**************************************");
    }

  }
  /*********************************************************************************************************/

  /*********************************************************************************************************/
  // Pilihan menu untuk Help 5
  if (flag_menu == 5) {
    Serial1.println("**************************************");
    Serial1.println("Chooese your email and password e.g. 1,admin@admin.com,123098765 (sparete comma) ");
    Serial1.println("**************************************");
    flag_menu = 0;
    flag_sub2_menu = 5;
  }

  // Pilihan "Eksekusi" Menu untuk Help 1
  if ( flag_sub2_menu == 5 & dt[0] == "1" ) {

    Serial1.print("\n");
    Serial1.print("\n");
    Serial1.println("**************************************");
    Serial1.print("--> Your email set up ist: ");
    Serial1.println((String)dt[1]);
    Serial1.print("--> Your pasword set up is: ");
    Serial1.println(dt[2]);
    Serial1.println("**************************************");

  }
  /*********************************************************************************************************/

  /*********************************************************************************************************/
  // Pilihan menu untuk Help 6
  if (flag_menu == 6) {
    flag_sub_menu = 61;
    Serial1.println("**************************************");
    Serial1.println("let's start your device use command: '1111' ");
    Serial1.println("**************************************");
    flag_menu = 0;
  }

  // Pilihan "Eksekusi" Menu untuk Help 1
  if (flag_sub_menu == 61 & dt[0].toInt() == 1111) {

    Serial1.print("\n");
    Serial1.print("\n");
    Serial1.println("**************************************");
    Serial1.print("--> The Program is starting[ ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    Serial1.print(" || ");
    Serial1.println("] 100%");
    Serial1.println("**************************************");
  }
  /*********************************************************************************************************/

  /*********************************************************************************************************/
  // Pilihan menu untuk Help 7
  if (flag_menu == 7) {
    flag_sub_menu = 71;
    Serial1.println("**************************************");
    Serial1.println("stop your device use command: '9999' ");
    Serial1.println("**************************************");
    flag_menu = 0;
  }

  // Pilihan "Eksekusi" Menu untuk Help 1
  if (flag_sub_menu == 71 & dt[0].toInt() == 9999) {

    Serial1.print("\n");
    Serial1.print("\n");
    Serial1.println("**************************************");
    Serial1.print("--> The Program is stopping[ ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    Serial1.print(" || ");
    Serial1.println("] 100%");
    Serial1.println("**************************************");
  }
  /*********************************************************************************************************/

  /*********************************************************************************************************/
  // Pilihan menu untuk Help 8
  if (flag_menu == 8) {
    flag_sub_menu = 81;
    Serial1.println("**************************************");
    Serial1.println("stop your device use command: '9999' ");
    Serial1.println("**************************************");
    flag_menu = 0;
  }

  // Pilihan "Eksekusi" Menu untuk Help 8
  if (flag_sub_menu == 81 & dt[0].toInt() == 5555) {

    Serial1.print("\n");
    Serial1.print("\n");
    Serial1.println("**************************************");
    Serial1.print("--> The Program is reseting[ ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    delay(100);
    Serial1.print(" || ");
    Serial1.print(" || ");
    Serial1.println("] 100%");
    Serial1.println("**************************************");
  }
  /*********************************************************************************************************/

  /*********************************************************************************************************/
  // Pilihan menu untuk Help 8
  if (flag_menu == 9) {
    flag_sub_menu = 91;
    Serial1.println("**************************************");
    Serial1.println("Show your device info / status use command: '777' ");
    Serial1.println("**************************************");
    flag_menu = 0;
  }

  // Pilihan "Eksekusi" Menu untuk Help 9
  if (flag_sub_menu == 91 & dt[0].toInt() == 777) {

    Serial1.print("\n");
    Serial1.print("\n");
    Serial1.println("**************************************");
    Serial1.println("--> Status Info ");
    delay(100);
    Serial1.println("Data1: ");
    delay(100);
    Serial1.println("Data2: ");
    delay(100);
    Serial1.println("Data3: ");
    delay(100);
    Serial1.println("**************************************");
  }
  /*********************************************************************************************************/

}
/*********************************************************************************************************/


/*********************************************************************************************************/
/*
   Parsing Data untuk UDP
*/

void parsingData2()
{
  int j2 = 0;
  //kirim data yang telah diterima sebelumnya
  //
  //inisialisasi variabel, (reset isi variabel)
  dt2[j2] = "";
  //proses parsing data
  for (i2 = 0; i2 < dataIn2.length(); i2++)
  {
    //pengecekan tiap karakter dengan karakter (#) dan (,)
    if ((dataIn2[i2] == '#') || (dataIn2[i2] == ','))
    {
      //increment variabel j, digunakan untuk merubah index array penampung
      j2++;
      dt2[j2] = ""; //inisialisasi variabel array dt[j]
    }
    else
    {
      //proses tampung data saat pengecekan karakter selesai.
      dt2[j2] = dt2[j2] + dataIn2[i2];
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
/*********************************************************************************************************/
