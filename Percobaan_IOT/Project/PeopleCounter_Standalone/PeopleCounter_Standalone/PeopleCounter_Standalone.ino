/*
   Program irostech untuk People Counter standalone versi bolak-balik.
   Program bekerja dengan komponen-komponen sbb:
   1. STM32 (Main controller)
   2. OLED (menampilkan data maximum capacity dan current capacity)
   3. Relay (Open saat maximum capacity=current capacity
   4. SD card (Menyimpan CSV tiap update current capacity (?)) V
   5. ESP8266 :
   6. IR Sensor
*/
/******************************************OLED*********************************************************/
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

//Uncomment dari jenis OLED yang digunakan

//U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
//U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//U8X8_SSD1306_128X32_WINSTAR_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//U8X8_SSD1306_64X48_ER_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // EastRising 0.66" OLED breakout board, Uno: A4=SDA, A5=SCL, 5V powered
//U8X8_SSD1306_48X64_WINSTAR_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_64X32_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_64X32_1F_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
//U8X8_SSD1306_96X16_ER_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // EastRising 0.69"
//U8X8_SSD1306_72X40_ER_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42"
//U8X8_SSD1306_128X64_NONAME_6800 u8x8(13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);
U8X8_SSD1306_128X64_NONAME_8080 u8x8(13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);

//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);    // VDD=5V SCL=A5 SDA=A4
int a = 10;
enum {BufSize = 5}; // If a is short use a smaller number, eg 5 or 6
char bufcurrent[BufSize];
char bufmaxcap[BufSize];

/********************************************************************************************************/
#include <DS1302.h>
#include <itoa.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
/******************************************Relay*********************************************************/
const int pinRelay1 = PB12;
const int pinRelay2 = PB13;

/********************************************************************************************************/


/********************************************IR**********************************************************/
#include <SharpIR.h>
SharpIR sensor1( SharpIR::GP2Y0A41SK0F, PA0 );
SharpIR sensor2( SharpIR::GP2Y0A41SK0F, PA1 );
int current_state1 = 0;
int current_state2 = 0;
int state_sensor1 = 0;
int state_sensor2 = 0;
int previous_state1 = 0;
int flag_counter = 0;

float calibrate1 = 250;
float calibrate2 = 250;

int distance1, distance2;
/********************************************************************************************************/

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
char tanggal[7];
}

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
int i; // Counter data parser Serial1 Bluetooth
boolean parsing = false;
int flag_menu = 0;
int flag_sub_menu = 0;
int flag_sub2_menu = 0;
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
uint16_t pp_in, pp_out;
uint16_t currentcap = 0;
uint16_t maxcap = 0;;

void setup() {
  // put your setup code here, to run once:
  u8x8.begin();
  u8x8.setPowerSave(0);
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);
  Serial1.begin(9600);
  rtc.writeProtect(false);
  rtc.halt(false);
  Serial1.print("Initializing SD card...");
  if (!SD.begin(SDCARD_CS)) {
    Serial1.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial1.println("card initialized.");

  //  Time t(2020, 7, 18, 12, 59, 59, 7); // aktifkan untuk update

  //  Time t(setthn, setbln, settgl, setjam, setmnt, setdtk, sethr); // aktifkan untuk update

  // Set the time and date on the chip.
  //  rtc.time(t);
  //debugging isi SD card
  root = SD.open("/");
  printDirectory(root, 0);


}

void loop() {
  // put your main code here, to run repeatedly:
  Serial1.println("Tes");
  //relayHandler(currentcap, maxcap);

  //oled();
  infrared();
}

/***************************Relay**************************/

/**********************************************************/
void relayHandler(int currentcap, int maxcap) {
  //Nyalakan relay saat kapasitas mencapai maksimum
  if (currentcap >= maxcap) {
    digitalWrite(pinRelay1, HIGH);
    digitalWrite(pinRelay2, HIGH);
  } else {
    digitalWrite(pinRelay1, LOW);
    digitalWrite(pinRelay2, LOW);
  }
}


/*******************************Penulisan csv di SD card******************************
  Brief:Menulis ke dalam csv dengan format sesuai tanggal, mengembalikan variabel
  i yang menandakan baris
  write_csv disini hanya akan dipanggil didalam fungsi infrared, untuk memastikan bahwa penulisan
  dilakukan saat counter berubah
  /*************************************************************************************/

void write_csv() {
  //inisialisasi file (buka file atau buat baru sesuai tanggal)
  //Requirement yang belom ada sekarang : tanggal dan number
  //parsingData2();
  //if (dt2[1] == "1") {
  printTime();
  String namafile = String(tanggal) + String(".csv");
  Serial1.println(namafile);
  File myFile = SD.open(namafile, FILE_WRITE);

  //Jika dapat dibuka, write
  if (myFile) {
    Serial1.print("Writing to csv...");

    //simpan urutan
    myFile.print(String(number));
    //Ke kolom selanjutnya
    myFile.print(",");

    //simpan data angka
    myFile.print(String(pp_in));
    myFile.print(",");

    myFile.print(String(pp_out));
    myFile.print(",");

    myFile.print(String(currentcap));
    myFile.print(",");

    myFile.println(String(maxcap));
    // close the file:
    myFile.close();
    Serial1.println("done.");
    number = number + 1;
  } else {
    // if the file didn't open, print an error:
    Serial1.println("error opening csv");
    number = number;
  }
  //}
}

/*******************************************OLED**********************************************************/
void oled() {
  /* U8g2 Project: SSD1306 Test Board */
  //pinMode(10, OUTPUT);
  //pinMode(9, OUTPUT);
  //digitalWrite(10, 0);
  //digitalWrite(9, 0);

  /* U8g2 Project: KS0108 Test Board */
  //pinMode(16, OUTPUT);
  //digitalWrite(16, 0);
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.setCursor(0, 0);
  u8x8.print(currentcap);
  u8x8.setCursor(0, 2);
  u8x8.print(maxcap);
}
/*********************************************************************************************************/

/*******************************************Calculate*****************************************************/
void calculation() {
  //Masi ragu isinya apa aja
  currentcap = pp_in - pp_out;
}
/*********************************************************************************************************/


/******************************************Infrared*******************************************************/
void infrared() {
  //Menentukan orang masuk apa keluar dari state
  distance1 = sensor1.getDistance();
  distance2 = sensor1.getDistance();
  if (distance1 < calibrate1 && distance2 >= calibrate2) { // sensor1 = 1 & sensor2 = 0
    current_state1 = 0;
  } else if (distance1 < calibrate1 && distance2 < calibrate2) { // sensor1 = 1 & sensor2 = 1
    flag_counter = 1;
  } else if (distance1 >= calibrate1 && distance2 < calibrate2) { // sensor1 = 0 & sensor2 = 1
    current_state1 = 2;
  } else  if (distance1 >= calibrate1 && distance2 >= calibrate2) { // sensor1 = 0 & sensor2 = 0
    current_state1 = 3;
    flag_counter = 0;
    state_sensor1 = 3;
    state_sensor2 = 3;
  }
  if (previous_state1 != current_state1 ) {
    if (current_state1 == 0 & state_sensor2 == 0) {
      pp_in++;
      write_csv();
      //total_visitor++;
      /*
        Serial13.print(F("PP: "));
        Serial13.print(pp_counter);
        Serial13.print(F("\tSens1: "));
        Serial13.print(distance1);
        Serial13.print(F("\tSens2: "));
        Serial13.print(distance2);
        Serial13.print(F("state: "));
        Serial13.println(current_state1);

      */
      state_sensor1 = 2;
      state_sensor2 = 2;

    } else if (current_state1 == 2 & state_sensor1 == 0) {
      pp_in--;
      pp_out++;
      write_csv();

      /*
        Serial13.print(F("PP: "));
        Serial13.print(pp_counter);
        Serial13.print(F("\tSens1: "));
        Serial13.print(distance1);
        Serial13.print(F("\tSens2: "));
        Serial13.print(distance2);
        Serial13.print(F("\tstate: "));
        Serial13.println(current_state1);

      */
      state_sensor1 = 2;
      state_sensor2 = 2;
    }


    previous_state1 = current_state1;
  }

  //pp_counter = pp_in;
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

  snprintf(tanggal, sizeof(tanggal), "%02d%02d%04d", t.date, t.mon, t.yr);

  // Print the formatted string to Serial1 so we can see the time.
  //  Serial1.println(buf);
}
/*********************************************************************************************************/
void parsingData2() //saat ini belom dipake
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
  //      Serial1.print("data 5 : ");v
  //      Serial1.print(dt[4].toInt());
  //      Serial1.print("\n\n");
}
/*********************************************************************************************************/


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
