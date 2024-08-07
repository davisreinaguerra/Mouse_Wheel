//Custom Classes_____________________
#include "lick_sensor.h"
#include "solenoid.h"
#include "alignment.h"
#include "looming.h"
#include "LED.h"
#include "ProbabilityFunctions.h"
#include "sound.h"

//Pin Assignments_____________________
alignment align_yellow(2);       // D2   Yellow alignment wire
alignment align_black(3);        // D3   Black alignment wire
alignment align_blue(4);         // D4   Blue alignment wire
alignment align_green(5);        // D5   Green alignment wire
                                 // D6   unassigned
sound_player sound_stim(7);      // D7   Trigger for Sound Player based on Adafruit Music Maker
looming looming(8);              // D8   basic trigger using NPN on MatrixPortal M4
LED left_LED(9);                 // D9   Green LED wire (left mouse perspective)
LED right_LED(10);               // D10  Yellow LED wire (right mouse perspective)
lick_sensor lick(11);            // D11  Wired to cap sensor
solenoid reward_solenoid(12);    // D12  
solenoid airpuff_solenoid(13);   // D13  


//Configuration Struct_________________
struct configuration {
  
  int mode;
  String date;
  String animal_code;
  long total_trials;
  
  // Event Durations
  long reward_duration;
  long puff_duration;
  long loom_duration;
  long sound_duration;

  // Probabilities
  int reward_prob;
  int puff_prob;
  int loom_prob;
  int sound_prob;
  
  // Cue to event delays
  long cue_to_event_delay;   // when a generic delay is needed
  
  long cue_to_reward_delay;
  long cue_to_puff_delay;
  long cue_to_loom_delay;
  long cue_to_sound_delay;

  // Timing parameters 
  int delay_between_lick_and_reward;
  long intertrial_interval;


} configuration;

//_________Variable Initializations_________________
bool complete = false;
int trial_number = 0;
String coinflip = flip_coin();
long reward_counter = 0;
long latency_to_lick;
long experiment_start_time;
long trial_start_time;
int begin_qm;

// timer functionality
long timer_start;
long time_elapsed;

//_________LCD_Screen_______________________________
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);
String menu_categories[4] = {"", "", "", ""};
String updatable_menu[4] = {"", "", "", ""};


//_________Modes____________________________________
//  1: Check everything
//  2: Cued Reward/Loom Probablistic

void setup() {
  // Set up serial port
  Serial.begin(9600);
  Serial.flush();

  // Initialize LCD Screen_____________________
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("I'm afraid I cant do");
  lcd.setCursor(0, 1); lcd.print("that Dave...");
  lcd.setCursor(6, 3); lcd.print("Treadmill_v1.1");

  // GUI Setup_________________________________
  //configuration.mode = read_config();
  //configuration.date = read_config();
  //configuration.animal_code = read_config();
  //configuration.reward_duration = read_config();
  configuration.reward_prob = read_config();
  begin_qm = read_config();

  // Manual Setup______________________________
  configuration.mode = 3;

  // Initialize Modes_______________________________________________

  // Cued Reward/Loom Probablistic: ** This is technically a form of delay conditioning
  if (configuration.mode == 3) {
      // Configure and report parameters
      Serial.println("\nParams: ");
      Serial.println("Mode: Reward/Aversive Probablistic");
      configuration.total_trials = 50;                    Serial.println("Total Trials: " + String(configuration.total_trials));
      configuration.intertrial_interval = 5000;           Serial.println("Intertrial Interval: " + String(configuration.intertrial_interval));
      configuration.reward_duration = 45;                 Serial.println("Reward Duration: " + String(configuration.reward_duration));
      //configuration.loom_duration = 1000;                 Serial.println("Loom Duration: " + String(configuration.loom_duration));
      configuration.puff_duration = 200;                  Serial.println("Puff Duration: " + String(configuration.puff_duration));
                                                          Serial.println("Reward Probability: " + String(configuration.reward_prob));
                                                          Serial.println("Loom Probability: " + String(100 - configuration.reward_prob));                                                    
      configuration.cue_to_event_delay = 5000;            Serial.println("Cue to Event Delay: " + String(configuration.cue_to_event_delay));

      // Report alignment assignment
      Serial.println("\nAlignments: ");
      Serial.println("Yellow: General Cue/Event (ON: Cue, OFF:Event)");
      Serial.println("Blue: Reward (ON: Left Cue, OFF: Reward)");
      Serial.println("Black: Aversion (ON: Right Cue, OFF: Aversion)");
      Serial.println("Green: Licks");

      // Set up Menu 
      menu_categories[0] = "mode";
      menu_categories[1] = "trial";
      menu_categories[2] = "cue_del";
      menu_categories[3] = "rew_prob";
      initialize_menu();

      updatable_menu[0] = "Cued Rew/Ave";
      updatable_menu[1] = "1";
      updatable_menu[2] = (String((configuration.cue_to_event_delay/1000)) + "s");
      updatable_menu[3] = String(configuration.reward_prob);
      update_menu();

      // Initialize
      experiment_start_time = millis();
      trial_start_time = millis();
      trial_number = 1;
      Serial.println("\n$ Trial#: " + String(trial_number));
  }

  switch (begin_qm) {
    case 1: 
      Serial.println("So it begins...");
      break;
    case 0:
      while(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  switch (configuration.mode) {
    case 1: // Check Everything
      

      // Check LED's
      right_LED.LED_on();
      left_LED.LED_on();
      delay(1000);
      right_LED.LED_on();
      left_LED.LED_on();
      delay(1000);

      // Check Loom and Sound
      looming.loom_on();
      sound_stim.sound_on();
      delay(1000);
      looming.loom_off();
      sound_stim.sound_off();
      delay(1000);

      // Check solenoids
      reward_solenoid.pulse_valve(200);
      airpuff_solenoid.pulse_valve(200);

      // Check Alignment integrity
      align_yellow.align_onset();
      align_black.align_onset();
      align_blue.align_onset();
      align_green.align_onset();

      delay(200);
      align_yellow.align_offset();

      delay(400);
      align_black.align_offset();

      delay(800);
      align_blue.align_offset();

      delay(1000);
      align_green.align_offset();

      delay(100);
      break;

    case 3: // Reward_Aversion Probablistic
      
      // _____________________________________Alignment Designations_____________________________________________
      // Yellow: General cue/Event (ON when any cue is presented, OFF when event takes place)
      // Blue: Reward Cue and Delivery (ON when reward cue (right) is presented, OFF when Reward Delivered)
      // Black: Aversion Cue and Delivery (ON when aversion cue (left) is presented, OFF when aversion Presented)
      // Green: Licks (shunted lick sensor state)
      
      // Present either Reward or Aversion ________________________________________________________________________________________________
      switch (enforce_prob(configuration.reward_prob)) {                                                  // Check for Reward probability
        
        case true: // Reward Case
          right_LED.LED_on();                                                                             // Turn on right LED
          align_blue.align_onset();                                                                       // Align_on to right LED
          align_yellow.align_onset();                                                                     // Align_on to general cue presentation                                              
          
          time_elapsed = 0;                                                               
          while(time_elapsed < configuration.cue_to_event_delay) {                                        // While you enforce the cue to event delay:                
            align_green.align_shunt(lick.is_licked(5));                                                   //    Shunt lick sensor state to Green alignment
            time_elapsed = millis() - trial_start_time;                                   
          }   
          
          align_blue.align_offset();                                                                      // Align_off to reward delivery
          align_yellow.align_offset();                                                                    // Align off to general event
          Serial.println(":) Reward Delivered at t = " + String(millis() - trial_start_time) + "ms");     // Report Reward Delivery
          reward_solenoid.pulse_valve(configuration.reward_duration);                                     // Deliver reward
          right_LED.LED_off();                                                                            // Turn Off right LED
          break;

        case false: // Aversive Case                                                                          
          
          left_LED.LED_on();                                                                              // Turn on left LED
          align_black.align_onset();                                                                      // Align_on to left LED                                                 
          align_yellow.align_onset();                                                                     // Align_on to general cue
          
          time_elapsed = 0;                                                                               
          while(time_elapsed < configuration.cue_to_event_delay) {                                        // While you enforce the cue to event delay:                
            align_green.align_shunt(lick.is_licked(5));                                                   //    Shunt lick sensor state to Green alignment
            time_elapsed = millis() - trial_start_time;                                                   
          }
             
          align_black.align_offset();                                                                     // Align_off to aversion presentation
          align_yellow.align_offset();                                                                    // Align_off to general event
          Serial.println(":( Aversion Delivered at t = " + String(millis() - trial_start_time) + "ms");   // Report Aversion
          //looming.loom_on(); delay(configuration.loom_duration); looming.loom_off();                    // Present Loom
          airpuff_solenoid.pulse_valve(configuration.puff_duration);                                     // Present Puff
          left_LED.LED_off();                                                                             // Turn off the left LED
          break;
      }

      // Conclude experiment if trial number exceeds total trials _________________________________________________________________________
      if (trial_number >= configuration.total_trials) {
        complete = true;
        break; // escape switch statement before starting a new trial
      }
      
      // Finalize and initialize next trial _______________________________________________________________________________________________
      delay(configuration.intertrial_interval);                                       // Enforce Intertrial Interval
      trial_number += 1;                                                              // Increment Trials
      Serial.println("\n$ Trial#: " + String(trial_number));                          // Report Trial Increment     
      updatable_menu[1] = String(trial_number); update_menu();                        // Update Trial Number                                                                          
      trial_start_time = millis();      

      break;
  }

  // Finalize ___________________________________________________
  if (complete == true) {
    // Push Final Data back to computer over serial port
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Complete!");
    while(1) {}
  }

}

// Menu functions 
void initialize_menu() {
  lcd.clear();

  lcd.setCursor(12, 0);
  lcd.print(menu_categories[0]);

  lcd.setCursor(12, 1);
  lcd.print(menu_categories[1]);

  lcd.setCursor(12, 2);
  lcd.print(menu_categories[2]);

  lcd.setCursor(12, 3);
  lcd.print(menu_categories[3]);
}

void update_menu() {
  initialize_menu();

  lcd.setCursor(0, 0);
  lcd.print(updatable_menu[0]);

  lcd.setCursor(0, 1);
  lcd.print(updatable_menu[1]);

  lcd.setCursor(0, 2);
  lcd.print(updatable_menu[2]);

  lcd.setCursor(0, 3);
  lcd.print(updatable_menu[3]);
}

long int read_config() {
  while (!Serial.available()) {}
  return Serial.parseInt();
}
