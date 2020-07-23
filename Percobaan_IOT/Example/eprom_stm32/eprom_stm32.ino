#include <EEPROM.h>
uint16 Status;
uint16 Data;
uint16 DataWrite = 123;
uint16 AddressWrite = 5;


void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  //Status = EEPROM.write(AddressWrite, DataWrite);
//  EEPROM.write(AddressWrite, DataWrite);
//  Serial1.print("EEPROM.write(0x");
//  Serial1.print(AddressWrite, DEC);
//  Serial1.print(", 0x");
//  Serial1.print(DataWrite, DEC);
//  Serial1.print(") : Status : ");
//  Serial1.println(Status, DEC);

  //      Status = EEPROM.read(AddressWrite, &Data);
  EEPROM.read(AddressWrite, &Data);
  Serial1.print("EEPROM.read(0x");
  Serial1.print(AddressWrite, DEC);
  Serial1.print(", &..) = 0x");
  Serial1.print(Data, DEC);
  Serial1.print(" : Status : ");
  Serial1.println(Status, DEC);

}

void loop() {
  // put your main code here, to run repeatedly:

}
