#include "Arduino.h"
#include "lamp.h"

LAMP::LAMP(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void LAMP::blink(int delayTime)
{
  if (millis()-lastMillis>delayTime) {
    lastMillis = millis();
    _ledState = !_ledState;
    digitalWrite(_pin, _ledState);
  } 
}
