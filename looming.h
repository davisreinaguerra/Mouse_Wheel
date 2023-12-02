#ifndef looming_h
#define looming_h

#include "Arduino.h"

class looming {
  public:
    looming(int pin); // Constructor
  private:
    int _pin;
};

#endif