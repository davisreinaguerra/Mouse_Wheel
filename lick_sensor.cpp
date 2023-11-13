#include "lick_sensor.h"
#include "Arduino.h"

// constructor
lick_sensor::lick_sensor(int pin) {
  pinMode(pin, INPUT);
  _pin = pin;
}

bool lick_sensor::is_licked(int n_checks) {
  for (int i = 0; i < n_checks; i++) {
    if (digitalRead(_pin) == true) {return true;}
  }
  return false;
}


/*
bool IR_sensor::is_broken() {
  check1 = !digitalRead(_pin);
  check2 = !digitalRead(_pin);
  if (check1 == 1 && check2 == 1) {
    return true;
  } else {
    return false;  
  }
}
*/
