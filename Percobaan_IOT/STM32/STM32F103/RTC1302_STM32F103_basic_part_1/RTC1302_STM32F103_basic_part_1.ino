#include <stdio.h>
#include <DS1302.h>

namespace {

// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet:
//
//   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
//const int kCePin   = 2;  // Chip Enable
//const int kIoPin   = 4;  // Input/Output
//const int kSclkPin = 5;  // Serial1 Clock

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

void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to Serial1 so we can see the time.
  Serial1.println(buf);
}

}  // namespace

void setup() {
  Serial1.begin(115200);

  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
//  setthn = 2020;
//  setbln = 07;
//  settgl = 06;
//
//  setjam = 23;
//  setmnt = 34;
//  setdtk = 00;
//  sethr = 2;
//  
//  Time t(setthn, setbln, settgl, setjam, setmnt, setdtk, sethr); // aktifkan untuk update

  // Set the time and date on the chip.
//  rtc.time(t);
}

// Loop and print the time every second.
void loop() {
  printTime();
  delay(1000);
}
