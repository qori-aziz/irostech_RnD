#include <SD.h>

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
int pp_in, pp_out, currentcap,cap;
/***************************************RTC**************************************************************/
#include <DS1302.h>
#include <itoa.h>

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

void setup(){
  
}

void loop(){
  
}
/*******************************************************************************************************
uint16_t pp_in, pp_out;
uint16_t currentcap = 0;
uint16_t maxcap = 0;;


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
    currentcap = pp_in-pp_out;

    myFile.print(String(currentcap));
    myFile.print(",");

    myFile.println(String(cap));
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
