#include "solenoid.h"
#include "Arduino.h"

// constructor
solenoid::solenoid(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void solenoid::valve_off() {
  digitalWrite(_pin, LOW);
}

void solenoid::pulse_valve(int reward_duration) {
  digitalWrite(_pin, HIGH);
  delay(reward_duration);
  digitalWrite(_pin, LOW);
}
