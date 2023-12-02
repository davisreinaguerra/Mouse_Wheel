//Custom Classes_____________________
#include "lick_sensor.h"
#include "solenoid.h"
#include "alignment.h"
#include "looming.h"
#include "LED.h"

//Pin Assignments_____________________
                                // D1   unassigneds
                                // D2   unassigned
alignment align_lick_reward(3)  // D3   Yellow alignment wire
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
  int reward_duration;
  int trial_duration;
  int delay_between_lick_and_reward;
}

//_________Variable Initializations_________________
bool complete = false;


void setup() {
  // Set up serial port
  Serial.begin(9600);

  // Manual Setup______________________________
  configuration.mode = "free_rewards";
  configuration.reward_duration = 200;
  configuration.trial_duration = 600000;
  configuration.delay_between_lick_and_reward = 50;

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
        align_lick_reward.align_onset();                                      // Alignment onset to be interpreted as lick
        delay(configuration.delay_between_lick_and_reward);                   // Wait 
        reward_solenoid.pulse_valve(configuration.reward_duration)            // Open solenoid to deliver reward
        align_lick_reward.align_offset();                                     // Alignment offset to be interpreted as reward delivery
      }

      // concludes trial if 
      if ((millis()-start_time) > configuration.trial_duration) {
        complete == true
      }
      break;
    case "reward_puff_probablistic":
      // Code here
      Serial.print("Thing happened: "); Serial.println(Str(millis()-start_time))
      // Must include an if statement which sets complete == true if a ending condition is met (ex. timeout)
      break;
  }

  if (complete == true) {
    // Push Final Data back to computer over serial port
    Serial.println("Done!");

    // Print and remaining data to the serial port

    while(1) {}
  }

}


