int pp_in, pp_out,i, dissum1,dissum2;
float dist1,dist2;
/********************************************IR**********************************************************/
//#include <SharpIR.h>
//SharpIR sensor1( SharpIR::GP2Y0A41SK0F, PA0 );
//SharpIR sensor2( SharpIR::GP2Y0A41SK0F, PA1 );
int current_state1 = 0;
int current_state2 = 0;
int state_sensor1 = 0;
int state_sensor2 = 0;
int previous_state1 = 0;
int flag_counter = 0;

int calibrate1 = 10;
int calibrate2 = 10;
float distance1, distance2;

const int pinIR1 = PA0;
const int pinIR2 = PA1;
/********************************************************************************************************/
void setup() {
  Serial1.begin(115200);
}

void loop() {
  
  // put your main code here, to run repeatedly:
  infrared();
//  Serial1.print("sensor 1:  ");
//  Serial1.println(dist1);
  delay(500);

  Serial1.print("sensor 1: cm ");
  Serial1.println(distance1);
//  delay(500);
  Serial1.print("sensor 2: cm ");
  Serial1.println(distance2);
}

void infrared() {
  //Menentukan orang masuk apa keluar dari state
  for (int i=0; i<20; i++){
    dissum1=dissum1+analogRead(pinIR1);
    dissum2=dissum2+analogRead(pinIR2);
    delay(5);
  }
  //3.3 -> sesuatu
  //5->3.3
  dist1= dissum1*3.3*(5/3.3)/(1023*20);
  dist2= dissum2*3.3*(5/3.3)/(1023*20);
  distance1 = 30*pow(dist1,-1.2);
  distance2 = 30*pow(dist2,-1.2);
  dissum1=0;dissum2=0;

  //Rumus
  
  //
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
      //write_csv();
      //total_visitor++;
      state_sensor1 = 2;
      state_sensor2 = 2;

    } else if (current_state1 == 2 & state_sensor1 == 0) {
      pp_in--;
      pp_out++;
      //write_csv();
      state_sensor1 = 2;
      state_sensor2 = 2;
    }
    previous_state1 = current_state1;
  }
  //pp_counter = pp_in;
}
