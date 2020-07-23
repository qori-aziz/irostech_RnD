#ifndef LAMP_h
#define LAMP_h

#include "Arduino.h"

class LAMP
{
  public:
    LAMP(int pin);
    void blink(int delayTime);
  private:
    int _pin;
    int _ledState = LOW;
//    int _ledState = HIGH;
    unsigned long lastMillis = 0;
//    unsigned long blinkTime = 50;
};

#endif
