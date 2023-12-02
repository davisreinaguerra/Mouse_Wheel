#ifndef LED_h
#define LED_h

#include "Arduino.h"

class LED {
  public:
    LED(int pin); // Constructor
  private:
    int _pin;
};

#endif