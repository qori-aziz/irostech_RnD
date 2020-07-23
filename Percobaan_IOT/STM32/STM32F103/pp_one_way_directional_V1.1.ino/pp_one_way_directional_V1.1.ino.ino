#include <ArduinoJson.h>
#include <HCSR04.h>


/*
  HCSR04(trigger, echo, temperature, distance)

  trigger     - trigger pin
  echo        - echo pin
  temperature - ambient temperature, in C
  distance    - maximun measuring distance, in cm
*/
HCSR04 ultrasonicSensor_01(PA1, PA0, 20, 400);
HCSR04 ultrasonicSensor_02(PB4, PB5, 20, 400);

float distance1;
long duration1;
float distance2;
long duration2;

long int pp_counter = 0;
long int total_visitor = 0;
long int mall_capacity = 1000;
float capacity_procent = 0;
long int pp_in1 = 0, pp_in2 = 0, pp_in3 = 0, pp_in4 = 0, pp_in5 = 0;

long int pp_out1, pp_out2;

int gate1 = 1, gate2 = 2, gate3 = 3, gate4 = 1, gate5 = 1, gate6 = 2, gate7 = 1;

int current_state1 = 0;
int current_state2 = 0;
int state_sensor1 = 0;
int state_sensor2 = 0;
int previous_state1 = 0;
int flag_counter = 0;

float calibrate1 = 250;
float calibrate2 = 250;


const int trigP1 = 28;  //GPIO-5 of nodemcu
const int echoP1 = 29;  //GPIO-6 of nodemcu
const int trigP2 = 30;  //GPIO-7 of nodemcu
const int echoP2 = 31;  //GPIO-8 of nodemcu



void setup() {
  Serial1.begin(115200);
  Serial3.begin(9600);
  ultrasonicSensor_01.begin(); //set trigger as output & echo pin as input
  ultrasonicSensor_02.begin(); //set trigger as output & echo pin as input

}
void loop() {
  distance1 = ultrasonicSensor_01.getDistance();
  distance2 = ultrasonicSensor_02.getDistance();

  Serial1.print(F("\tS1: "));
  Serial1.print(distance1);
  Serial1.print(F("\tS2: "));
  Serial1.println(distance2);
  delay(100);

  StaticJsonDocument<200> doc;
  doc["distance1"] = distance1;
  doc["distance2"] = distance2;

  if (distance1 <= calibrate1) {
    current_state1 = 1;
  } else {
    current_state1 = 0;
  }

  if (previous_state1 != current_state1) {
    if (current_state1 == 1) {
      pp_in1++;
      Serial1.print(F("PP: "));
      Serial1.print(pp_counter);

      Serial1.print(F("\tS1: "));
      Serial1.print(distance1);
      Serial1.print(F("\tS2: "));
      Serial1.println(distance2);
      delay(100);
    }
    previous_state1 = current_state1;

  }

  pp_counter = pp_in1 + pp_in2 + pp_in3;
  capacity_procent = (float) pp_counter / mall_capacity * 100;


  //  if (distance1 != HCSR04_OUT_OF_RANGE | distance2 != HCSR04_OUT_OF_RANGE)
  //  {
  //    Serial3.print(F("PP: "));
  //    Serial3.print(pp_counter);
  //
  //    Serial3.print(F("\tS1: "));
  //    Serial3.print(distance1);
  //    Serial3.print(F("\tS2: "));
  //    Serial3.println(distance2);
  //
  //    /*
  //    Serial3.print(F("\tSS1: "));
  //    Serial3.print(state_sensor1);
  //    Serial3.print(F("\tSS2: "));
  //    Serial3.print(state_sensor2);
  //    Serial3.print(F("\tcs: "));
  //    Serial3.println(current_state1);
  //
  //
  //    //serializeJson(doc, Serial3);
  //    */
  //
  //  } else {
  //
  //    Serial3.print(F("PP: "));
  //    Serial3.print(pp_counter);
  //
  //    Serial3.print(F("\tS1: "));
  //    Serial3.print(distance1);
  //    Serial3.print(F("\tS2: "));
  //    Serial3.println(distance2);
  //    /*
  //    Serial3.print(F("PP: "));
  //    Serial3.print(pp_counter);
  //
  //      Serial3.print(F("\tS1:out"));
  //      Serial3.print(F("\tS2:out"));
  //
  //    Serial3.print(F("\tSS1: "));
  //    Serial3.print(state_sensor1);
  //    Serial3.print(F("\tSS2: "));
  //    Serial3.print(state_sensor2);
  //    Serial3.print(F("\tcs: "));
  //    Serial3.println(current_state1);
  //    */
  //
  //  }


}
