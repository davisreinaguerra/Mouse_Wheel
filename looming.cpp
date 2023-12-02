#include "looming.h"
#include "Arduino.h"

// constructor
looming::looming(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}
