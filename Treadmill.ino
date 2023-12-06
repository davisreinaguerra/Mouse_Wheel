//Custom Classes_____________________
#include "lick_sensor.h"
#include "solenoid.h"
#include "alignment.h"
#include "looming.h"
#include "LED.h"
#include "ProbabilityFunctions.h"

//Pin Assignments_____________________
                                // D1   unassigneds
                                // D2   unassigned
alignment align_yellow(3)       // D3   Yellow alignment wire
alignment align_black(4)        // D4   Black alignment wire
alignment align_blue(5)         // D5   Blue alignment wire
alignment align_green(6)        // D6   Green alignment wire
                                // D7   unassigned
looming looming(8)              // D8   basic trigger using NPN on MatrixPortal M4
LED left_LED(9)                 // D9   Green LED wire (left mouse perspective)
LED right_LED(10)               // D10  Yellow LED wire (right mouse perspective)
lick_sensor lick(11)            // D11  Wired to cap sensor
solenoid reward_solenoid(12)    // D12  Yellow Solenoid Wire
solenoid airpuff_solenoid(13)   // D13  Blue Solenoid Wire


//Configuration Struct_________________
struct configuration {
  char mode;
  char date;
  char animal_code;
  long reward_duration;
  long trial_duration;
  int delay_between_lick_and_reward;
  int puff_duration;
  long intertrial_interval;
  long total_trials;
}

//_________Variable Initializations_________________
bool complete = false;
int trial_number = 0;


void setup() {
  // Set up serial port
  Serial.begin(9600);

  // Manual Setup______________________________
  configuration.mode = "reward_puff_cued_probablistic";
  configuration.reward_duration = 200;
  configuration.puff_duration = 200;
  configuration.trial_duration = 600000;
  configuration.delay_between_lick_and_reward = 50;
  configuration.intertrial_interval = 10000;

  // GUI Setup_________________________________
  //configuration.mode = read_config();
  //configuration.date = read_config();
  //configuration.animal_code = read_config();
  //configuration.reward_duration = read_config();

  // Initialize timer for timeout and other time-based calculations
  start time = millis();
}

void loop() {
  // put your main code here, to run repeatedly:

  switch (configuration.mode) {
    case "free_rewards":
      
      // Execute the below code every time the lick sensor reads a lick
      if lick.islicked(10) {                                                  
        Serial.print("Lick: "); Serial.println(Str(millis()-start_time))      // Print to serial port "Lick: Time in ms"
        align_yellow.align_onset();                                      // Alignment onset to be interpreted as lick
        delay(configuration.delay_between_lick_and_reward);                   // Wait 
        reward_solenoid.pulse_valve(configuration.reward_duration)            // Open solenoid to deliver reward
        align_yellow.align_offset();                                     // Alignment offset to be interpreted as reward delivery
      }

      // concludes trial if 
      if ((millis()-start_time) > configuration.trial_duration) {
        complete == true
      }
      break;
    case "reward_puff_cued_probablistic":
      // Code 
      trial_number++;
      char coinflip = flip_coin();
      
      // Heads means air puff
      if (coinflip == "H") {
        left_LED.LED_on();
        airpuff_solenoid.valve_on()
        align_blue.align_onset();

        delay(configuration.puff_duration)

        left_LED.LED_off();
        airpuff_solenoid.valve_off()
        align_blue.align_offset();
      }

      // Tails means reward
      if (coinflip == "T") {
        right_LED.LED_on();
        reward_solenoid.valve_on()
        align_yellow.align_onset();

        delay(configuration.reward_duration)

        right_LED.LED_off();
        reward_solenoid.valve_off()
        align_yellow.align_offset();
      }
      
      if (trial_number>configuration.total_trials) {
        complete = true;
        break;
      }
      
      delay(configuration.intertrial_inerval)
      break;

    case "looming_every_20":
      trial_number++;
      
      looming.loom_on();
      delay(2000);
      looming.loom_off();

      if (trial_number>configuration.total_trials) {
        complete = true;
        break;
      }

      delay(20000);

  }

  if (complete == true) {
    // Push Final Data back to computer over serial port
    Serial.println("Done!");
    while(1) {}
  }

}


