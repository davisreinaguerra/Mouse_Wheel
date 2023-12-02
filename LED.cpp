#include "LED.h"
#include "Arduino.h"

// constructor
LED::LED(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}
