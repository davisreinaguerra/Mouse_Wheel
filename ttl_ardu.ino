#define TTL_out_pin 7
#define ready_in_pin 8




// Configure _______________________________________
int pulse_width = 10;                               // milliseconds
long duration = 10;                                 // seconds
long interpulseinterval = 5;                        // minutes

//int freqs[] = {5, 10, 20, 30, 40, 50};
int freqs[] = {50, 50, 50};




// Dont touch
bool state;
int freqs_size = sizeof(freqs) / sizeof(freqs[0]); // numBytes / size of a byte

void pulse_out(int pin, int duration, int freq, int pulse_width) {
  
  int total_pulses = duration * freq; // number of seconds * pulses per second
  int period = 1000 / freq; // number of periods that fit into a single second (1000)

  for (int i = 0; i < total_pulses; i++) {
    digitalWrite(pin, HIGH);   
    delay(pulse_width);        
    digitalWrite(pin, LOW);   
    delay(period - pulse_width);
  }
}

void setup() {
  pinMode(TTL_out_pin, OUTPUT);
  pinMode(ready_in_pin, INPUT);
}

void loop() {
  state = digitalRead(ready_in_pin);
  if (state == HIGH) {
    for (int i = 0; i < freqs_size; i++) {
      pulse_out(TTL_out_pin, duration, freqs[i], pulse_width);
      delay(interpulseinterval * 60000);
    }
  }
}
