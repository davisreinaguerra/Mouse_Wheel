//______________ Custom Classes_____________________
#include "lick_sensor.h"
#include "solenoid.h"

#define reward_solenoid 12
#define airpuff_solenoid 13


//_____________Configuration Struct_________________
struct configuration {
  char mode;
  char date;
  char animal_code;
}

//________________Data Struct_______________________
struct data {
  int total_number_of_licks;
  int puffs_delivered;
  int rewards_delivered;
}

//_________Variable Initializations_________________
bool complete = false;


void setup() {
  // Set up serial port
  Serial.begin(9600);

  configuration.mode = read_config();
  configuration.date = read_config();
  configuration.animal_code = read_config();

  // Initialize timer for timeout and other time-based calculations
  start time = millis();
}

void loop() {
  // put your main code here, to run repeatedly:

  switch (configuration.mode) {
    case "free_rewards":
      // Code here
      Serial.print("Thing happened: "); Serial.println(Str(millis()-start_time))
      // Must include an if statement which sets complete == true if a ending condition is met (ex. timeout)
      break;
    case "reward_puff_probablistic":
      // Code here
      Serial.print("Thing happened: "); Serial.println(Str(millis()-start_time))
      // Must include an if statement which sets complete == true if a ending condition is met (ex. timeout)
      break;
  }

  if (complete == true) {
    // Push Final Data back to computer over serial port
    Serial.println();

    while(1) {}
  }

}
