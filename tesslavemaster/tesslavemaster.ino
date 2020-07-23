#include <String.h>
char buff [11];
char buffthr[4];
char buffcap[4];
int idxthr = 0;
int idxcap = 0;
int thr;
int cap;
int indx = 0;
int x = 0;
//#include <SoftwareSerial.h>
//SoftwareSerial s(PB3,PB4); //RX,TX
HardwareSerial Serial2(PA3, PA2);
int state_buffer;
void setup() {
  Serial2.begin(115200);
  Serial1.begin(57600);
  //s.begin(57600);
  Serial2.println("Nawn");
}

void loop() {
  if (Serial1.available() > 0) {
    char c = Serial1.read();
    //Serial2.println(c);
    //    Serial2.println("haha1");
    if (indx < sizeof buff) {
      buff[indx] = c;
      indx = indx + 1;
      //Serial.println(buff);
      //Serial.println("test");
      delay(50);

    }
    if (c == '#') {
      for (x = 0; x < indx; x++) {
        Serial2.print(buff[x]);
      }
      Serial2.println();
      parsing1();
      indx = 0;
    }
  }
}


//BISA YEYYYYYYYYYY ALHAMDULILLAH

void parsing1() {
  x = 0;
  while (x < indx) {
    if (buff[x] == '*') {
      x++;
    } else if (buff[x] == ',') {
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
      break;
    } else {
      buffcap[idxcap] = buff[x];
      idxcap++;
      x++;
    }
  }
  thr = atoi(buffthr);
  cap = atoi(buffcap);
  Serial2.print("thr = ");
  Serial2.println(thr);
  Serial2.print("cap = ");
  Serial2.println(cap);
  x = 0; idxcap = 0; idxthr = 0;
  memset(buffcap, 0, sizeof buffcap);
  memset(buffthr, 0, sizeof buffthr);
}
