#include "alignment.h"
#include "Arduino.h"


alignment::alignment(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void alignment::align_onset() {
  digitalWrite(_pin, HIGH);
}

void alignment::align_offset() {
  digitalWrite(_pin, LOW);
}