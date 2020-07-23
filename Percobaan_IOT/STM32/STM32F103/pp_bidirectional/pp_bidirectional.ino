float temp_celsius = 0;
float temp_fahrenheit = 0;
float distance1;
long duration1;
float distance2;
long duration2;

long int pp_counter = 0;
long int total_visitor=0;
long int mall_capacity = 1000;
float capacity_procent=0;
long int pp_in1=0,pp_in2=0,pp_in3=0,pp_in4=0,pp_in5=0;

long int pp_out1, pp_out2;

int gate1=1, gate2=2, gate3=3, gate4=1, gate5=1, gate6=2, gate7=1;

int current_state1 = 0;
int previous_state1 = 0;
long flag_counter =0;

float calibrate1 = 50;
float calibrate2 = 50;


const int trigP1 = 28;  //GPIO-5 of nodemcu
const int echoP1 = 29;  //GPIO-6 of nodemcu
const int trigP2 = 30;  //GPIO-7 of nodemcu
const int echoP2 = 31;  //GPIO-8 of nodemcu


void setup() {
 Serial1.begin(115200);
 Serial3.begin(9600);
 pinMode(trigP1, OUTPUT);  // Sets the trigPin1 as an Output1
 pinMode(echoP1, INPUT);   // Sets the echoPin1 as an Input1
 pinMode(trigP2, OUTPUT);  // Sets the trigPin1 as an Output2
 pinMode(echoP2, INPUT);   // Sets the echoPin1 as an Input2

}
void loop() {
  digitalWrite(trigP1, LOW);   // Makes trigPin low
  delayMicroseconds(2);       // 2 micro second delay 
  digitalWrite(trigP1, HIGH);  // tigPin high
  delayMicroseconds(10);      // trigPin high for 10 micro seconds
  digitalWrite(trigP1, LOW);   // trigPin low
  duration1 = pulseIn(echoP1, HIGH);   //Read echo pin, time in microseconds
  distance1= duration1*0.034/2;        //Calculating actual/real distance

  digitalWrite(trigP2, LOW);   // Makes trigPin low
  delayMicroseconds(2);       // 2 micro second delay 
  digitalWrite(trigP2, HIGH);  // tigPin high
  delayMicroseconds(10);      // trigPin high for 10 micro seconds
  digitalWrite(trigP2, LOW);   // trigPin low
  duration2 = pulseIn(echoP2, HIGH);   //Read echo pin, time in microseconds
  distance2= duration2*0.034/2;        //Calculating actual/real distance

  if(distance1 < calibrate1 && distance2 >= calibrate2){ // sensor1 = 1 & sensor2 = 0
    current_state1 = 0; 
  }else if(distance1 < calibrate1 && distance2 < calibrate2){ // sensor1 = 1 & sensor2 = 1
    flag_counter = 1; 
  }
  if(distance1 >= calibrate1 && distance2 < calibrate2){ // sensor1 = 0 & sensor2 = 1
    current_state1 = 2; 
  }
  if(distance1 >= calibrate1 && distance2 >= calibrate2){ // sensor1 = 0 & sensor2 = 0
    current_state1 = 3;
    flag_counter = 0; 
  }

//  if(distance1 < calibrate1){ // sensor1 = 1 & sensor2 = 0
//    current_state1 = 0; 
//  }else if(distance1 >= calibrate1){ // sensor1 = 0 & sensor2 = 0
//    current_state1 = 3;
//
//  }

  if(previous_state1 != current_state1 ){
    if(current_state1 == 2 & flag_counter == 1){pp_in1++; total_visitor++;}
    if(current_state1 == 0 & flag_counter == 1){pp_in1--;pp_out1++;}

//    if(current_state1 == 0){pp_in1++; total_visitor++;}
//    if(current_state1 == 0){pp_in1--;pp_out1++;}
    
    previous_state1 = current_state1;
  }

  pp_counter = pp_in1 + pp_in2 + pp_in3;
  capacity_procent =(float) pp_counter / mall_capacity *100;
  
  
  
  
  Serial1.print("Distance1 = ");        //Output distance on arduino Serial1 monitor 
  Serial1.print(distance1);  
  Serial1.print("\tDistance2 = ");        //Output distance on arduino Serial1 monitor 
  Serial1.println(distance2);
  Serial1.print("Counter: ");
  Serial1.println(pp_counter);
  Serial1.print("Counter procent: ");
  Serial1.println(capacity_procent);  

  Serial3.print("Counter: ");
  Serial3.println(pp_counter);

  delay(10);
  


}
