const int pinRelay1 = PB12;
const int pinRelay2 = PB13;

void setup() {
  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);

  //init relay
  digitalWrite(pinRelay1, LOW);
  digitalWrite(pinRelay2, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  digitalWrite(pinRelay1, HIGH);
  digitalWrite(pinRelay2, LOW);

  delay(3000);
  digitalWrite(pinRelay1, LOW);
  digitalWrite(pinRelay2, HIGH);
}
