#include <String.h>
#include <SD.h>
#include <DS1302.h>
#include <itoa.h> // untuk windows

#define SDCARD_CS PA4
#define error(s) error_P(PSTR(s))

//OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

//Kudu ditambah pin pull up external
char str1[] = "current capacity";
char str2[] = "max capacity";


// GENERAL
int pp_in, pp_out, i, dissum1, dissum2;

float dist1, dist2;

// SD
File root;

char clientline[BUFSIZ];
char name[17];
//char c;

int number = 1;

boolean current_line_is_blank = true;

// RELAY
const int pinRelay1 = PB12;
const int pinRelay2 = PB13;

// INFRARED
const int pinIR1 = PB0;
const int pinIR2 = PA1;

int current_state1 = 0;
int current_state2 = 0;
int state_sensor1 = 0;
int state_sensor2 = 0;
int previous_state1 = 0;
int flag_counter = 0;
int calibrate1 = 10;
int calibrate2 = 10;

float distance1, distance2;
float thr = 35;

// SLAVE
char buff [11];
char buffthr[4];
char buffcap[4];

int idxthr = 0;
int idxcap = 0;

int cap = 5;
int currentcap = 0;
int indx = 0;
int x = 0;
int state_buffer;
unsigned long previousmillis1 = 0;
unsigned long previousmillis2 = 0;
unsigned long previousmillis3 = 0;
unsigned long currentmillis;

//Buat debug
HardwareSerial Serial2(PA3, PA2);
void setup() {

  // INISIALISASI
  Serial1.begin(57600);
  Serial2.begin(115200);
  //Pindahin ke I2C2 sesuai skematik alat
  Wire.setSDA(PB11);
  Wire.setSCL(PB10);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    //        Serial2.println("SSD1306 allocation failed");
    for (;;); // Don't proceed, loop forever
  }
  SD.begin(SDCARD_CS);
  //  if (!SD.begin(SDCARD_CS)) {
  //        Serial2.println("initialization failed!");
  //    while (1);
  //  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 2 seconds
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();

  // Clear the buffer
  display.clearDisplay();

  // INISIALISASI RELAY
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);
  digitalWrite(pinRelay1, HIGH);
  digitalWrite(pinRelay2, HIGH);

}

void loop() {
  //Serial
  currentmillis = millis();

  if ((currentmillis - previousmillis1) >= 1) {
    serialhandler();
    previousmillis1 = currentmillis;
  }
  //Serial1.println("x");


  // IR
  if ((currentmillis - previousmillis2) >= 30) {
    infrared();
    previousmillis2 = currentmillis;
  }

  //  Serial1.println("tesdebug");

  //OLED
  lcdhandler();


  // RELAY
  relayhandler();
  //Serial1.setTimeout(1000);

}

void serialhandler() {
  // SERIAL COMMUNICATION
  if (Serial1.available() > 0) {
    char c = Serial1.read();
    //    Serial2.println(c);
    //    Serial1.println("Masuk serial");
    //memset(buffcap, 0, sizeof buffcap);

    if (c == '*') { //Inisialisasi array
      indx = 0;
      memset(buff, 0, sizeof buff);
      buff[indx] = c;
      indx = indx + 1;
    }

    if ((indx < sizeof buff) && (buff[0] == '*')) {
      buff[indx] = c;
      indx = indx + 1;
      //Serial.println(buff);
      //Serial.println("karakter masuk ke buff");
      //delay(100);
      if (c == '#') {
        for (x = 0; x < indx; x++) {
          //Serial1.println("Karakter yang masuk");
          //Serial1.print(buff[x]);
        }
        //      Serial1.println();
        parsing1();
        indx = 0;
        memset(buff, 0, sizeof buff);
      }
    }
    //    else{
    //      indx=0;
    //      memset(buff, 0, sizeof buff);
    //    }
  }
  //Serial1.flush();
  //  delay(20);
}
/*** SD ***/
void write_csv() {
  //Inisialisasi file (buka file atau buat baru sesuai tanggal)
  //Requirement yang belom ada sekarang : tanggal dan number

  //parsingData2();
  String namafile = ("datalog.csv");
  //  Serial1.println("nama file di sd card:");

  //  Serial1.println(namafile);
  File myFile = SD.open(namafile, FILE_WRITE);

  //Jika dapat dibuka, write
  if (myFile) {
    //    Serial1.print("Writing to csv...");

    //Simpan urutan
    myFile.print(String(number));
    //Ke kolom selanjutnya
    myFile.print(",");

    //Simpan data angka
    myFile.print(String(pp_in));
    myFile.print(",");

    myFile.print(String(pp_out));
    myFile.print(",");


    myFile.print(String(currentcap));
    myFile.print(",");

    myFile.println(String(cap));
    // close the file:
    myFile.close();
    //    Serial1.println("done.");
    number = number + 1;
  } else {
    // if the file didn't open, print an error:
    //    Serial1.println("error opening csv");
    number = number;
  }
}


/*** INFRARED ***/
void infrared() {
  //Menentukan orang masuk apa keluar dari state
  for (int i = 0; i < 20; i++) {
    dissum1 = dissum1 + analogRead(pinIR1);
    Serial2.println(analogRead(pinIR1));
    dissum2 = dissum2 + analogRead(pinIR2);
    Serial2.println(analogRead(pinIR2));
    delay(3);
  }
  //3.3 -> sesuatu
  //5->3.3

  //Konversi ke voltase
  dist1 = dissum1 * (3.3) / (4095 * 20);
  //  Serial1.println(dist1);
  dist2 = dissum2 * (3.3) / (4095 * 20);
  //  Serial1.println(dist2);

  //  //Konversi ke jarak asli
  //  distance1 = 30 * pow(dist1, -1.2);
  //  distance2 = 30 * pow(dist2, -1.2);
  distance1 = 19.48 * (pow(dist1, 4)) - 152.22 * (pow(dist1, 3)) + 437.59 * (pow(dist1, 2)) - 571.88 * dist1 + 328.3;
  distance2 = 19.48 * (pow(dist2, 4)) - 152.22 * (pow(dist2, 3)) + 437.59 * (pow(dist2, 2)) - 571.88 * dist2 + 328.3;
  //  Serial2.print("distance 1 = ");
  //  Serial2.println(distance1);
  //  Serial2.print("distance 2 = ");
  //  Serial2.println(distance2);
  dissum1 = 0; dissum2 = 0;

  //
  if (distance1 < thr && distance2 >= thr) { // sensor1 = 1 & sensor2 = 0
    current_state1 = 0;
    //    Serial1.println("sensor 1 kena");
  } else if (distance1 < thr && distance2 < thr) { // sensor1 = 1 & sensor2 = 1
    flag_counter = 1;
    //    Serial1.println("sensor 1 dan 2 kena");
  } else if (distance1 >= thr && distance2 < thr) { // sensor1 = 0 & sensor2 = 1
    current_state1 = 2;
    //    Serial1.println("sensor 2 kena");
  } else  if (distance1 >= thr && distance2 >= thr) { // sensor1 = 0 & sensor2 = 0
    current_state1 = 3;
    flag_counter = 0;
    state_sensor1 = 3;
    state_sensor2 = 3;
  }
  // Logic Gate for 1 sensor
  if (distance1 < thr & state_sensor1 == 3) { // sensor1 = 1
    //    Serial1.println("Sensor 1 kena lagi");
    state_sensor1 = 0;
  } else if (distance2 < thr & state_sensor2 == 3) { // sensor2 = 1
    state_sensor2 = 0;
    //    Serial1.println("Sensor 2 kena lagi");
  } /*else if (distance1 >= calibrate1 & state_sensor1 == 3) { // sensor1 = 0
    state_sensor1 = 1;
  } else if (distance2 >= calibrate2 & state_sensor2 == 3) { // sensor2 = 0
    state_sensor2 = 1;
  }*/
  if (previous_state1 != current_state1 ) {
    if (current_state1 == 0 & state_sensor2 == 0) {
      //      Serial1.println("debug3");
      //Gatau kenapa ini ketuker
      //      pp_in++;
      if (pp_in > 0) {
        pp_out++;
        currentcap = pp_in - pp_out;
        write_csv();
      }

      //total_visitor++;
      state_sensor1 = 2;
      state_sensor2 = 2;

    } else if (current_state1 == 2 & state_sensor1 == 0) {
      //pp_in--;
      //Gatau kenapa ketuker
      //      if (pp_in > 0) {
      //        pp_out++;
      //      }
      pp_in++;
      currentcap = pp_in - pp_out;
      write_csv();
      state_sensor1 = 2;
      state_sensor2 = 2;
    }
    previous_state1 = current_state1;
  }
  //pp_counter = pp_in;
  //currentcap = pp_in - pp_out;
  //Serial.println("Jumlah orang masuk, keluar, kapasitas sekarang:");
  //  Serial.println(pp_in);
  //  Serial.println(pp_out);
  //  Serial.println(currentcap);
}

/*** SERIAL COMMUNICATION ***/
void parsing1() {
  int flagcorrect1 = 0;
  int flagcorrect2 = 0;
  x = 0;
  while (x < indx) {
    if (buff[x] == '*') {
      x++;
    } else if (buff[x] == ',') {
      flagcorrect1 = 1;
      break;
    } else if (buff[x] == '#') {
      break;
    } else if ((buff[x] > 57) || (buff[x] < 48)) {
      flagcorrect1 = 0;
      break;
    } else {
      buffthr[idxthr] = buff[x];
      idxthr++;
      x++;
    }
  }
  while (x < indx) {
    if (buff[x] == ',') {
      x++;
    } else if (buff[x] == '#') {
      flagcorrect2 = 1;
      break;
    } else if ((buff[x] > 57) || (buff[x] < 48)) {
      flagcorrect2 = 0;
      break;
    } else {
      buffcap[idxcap] = buff[x];
      idxcap++;
      x++;
    }
  }
  if ((flagcorrect1 == 1) && (flagcorrect2 == 1)) {
    thr = atoi(buffthr);
    cap = atoi(buffcap);
  }
  //  Serial1.print("thr = ");
  //  Serial1.println(thr);
  //  Serial1.print("cap = ");
  //  Serial1.println(cap);
  x = 0; idxcap = 0; idxthr = 0;
  memset(buffcap, 0, sizeof buffcap);
  memset(buffthr, 0, sizeof buffthr);
}

void lcdhandler(void) {
  //clear, text size, text color, cursor!!! Kurang 1 aja gabakal keluar
  display.clearDisplay();
  delay(5);
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setCursor(20, 0);     // Start at top-left corner
  display.setTextColor(SSD1306_WHITE);
  display.println(F(str1));
  //display.display();
  display.setCursor(60, 10);
  display.setTextSize(2);
  display.print(currentcap);
  //display.display();
  display.setTextSize(1);
  display.setCursor(20, 27);
  display.print(F(str2));
  display.setCursor(60, 45);
  display.setTextSize(2);
  display.print(cap);
  display.display();
  delay(10);
  //  currentcap = currentcap + 1;
  //  cap = cap + 1;
}

void relayhandler() {
  // put your main code here, to run repeatedly:
  if (currentcap >= cap) {
    delay(2);
    digitalWrite(pinRelay1, LOW);
    digitalWrite(pinRelay2, LOW);
  } else {
    delay(2);
    digitalWrite(pinRelay1, HIGH);
    digitalWrite(pinRelay2, HIGH);
  }
}
