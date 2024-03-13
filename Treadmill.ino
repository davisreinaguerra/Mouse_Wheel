//Custom Classes_____________________
#include "lick_sensor.h"
#include "solenoid.h"
#include "alignment.h"
#include "looming.h"
#include "LED.h"
#include "ProbabilityFunctions.h"
#include "sound.h"

//Pin Assignments_____________________
                                 // D1   unassigned
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
//  1: free rewards
//  2: Cued Reward Puff Coinflip
//  3: Cued Reward/Loom Probablistic
//  4: Check alignment integrity

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

  // Test all the parts
  //reward_solenoid.pulse_valve(200);
  //airpuff_solenoid.pulse_valve(200);
  
  //looming.loom_on();
  //left_LED.LED_on();
  //right_LED.LED_on();
  //sound_stim.sound_on();

  //delay(3000);
  
  //looming.loom_off();
  //left_LED.LED_off();
  //right_LED.LED_off();
  //sound_stim.sound_off();

  begin_qm = read_config();

  // Manual Setup______________________________
  configuration.mode = 3;

  // GUI Setup_________________________________
  //configuration.mode = read_config();
  //configuration.date = read_config();
  //configuration.animal_code = read_config();
  //configuration.reward_duration = read_config();

  // Initialize Mode_______________________________________________

  // Mode 1 Free Rewards
  if (configuration.mode == 1) {
      // Set up Menu
      menu_categories[0] = "mode";
      menu_categories[1] = "rewards";
      menu_categories[2] = "elapsed";
      menu_categories[3] = "latency";
      
      initialize_menu();
      updatable_menu[0] = "free";
      update_menu();

      // Set up parameters
      configuration.total_trials = 50;
      configuration.intertrial_interval = 10000;
      configuration.delay_between_lick_and_reward = 50;
      configuration.reward_duration = 100;
      configuration.puff_duration = 100;

      // Initialize
      experiment_start_time = millis();
      trial_start_time = millis();
      trial_number = 1;
      right_LED.LED_on();
  }

  // Cued Reward/Loom Probablistic: ** This is technically a form of delay conditioning
  if (configuration.mode == 3) {
      // Configure and report parameters
      Serial.println("\nParams: ");
      Serial.println("Mode: Reward/Aversive Probablistic");
      configuration.total_trials = 50;                   Serial.println("Total Trials: " + String(configuration.total_trials));
      configuration.intertrial_interval = 5000;          Serial.println("Intertrial Interval: " + String(configuration.intertrial_interval));
      configuration.reward_duration = 100;                Serial.println("Reward Duration: " + String(configuration.reward_duration));
      configuration.loom_duration = 1000;                 Serial.println("Loom Duration: " + String(configuration.loom_duration));
      configuration.reward_prob = 50;                     Serial.println("Reward Probability: " + String(configuration.reward_prob));
                                                          Serial.println("Loom Probability: " + String(100 - configuration.reward_prob));                                                    
      configuration.cue_to_event_delay = 5000;            Serial.println("Cue to Event Delay: " + String(configuration.cue_to_event_delay));

      // Report alignment assignment
      Serial.println("\nAlignments: ");
      Serial.println("Yellow: LED Cues Bilat");
      Serial.println("Black: Aversive Delivery");
      Serial.println("Blue: Reward Delivery");
      Serial.println("Green: Licks");

      // Set up Menu 
      menu_categories[0] = "mode";
      menu_categories[1] = "trial";
      menu_categories[2] = "cue_del";
      menu_categories[3] = "rew_prob";
      initialize_menu();

      updatable_menu[0] = "Cued Rew/Loom";
      updatable_menu[1] = "1";
      updatable_menu[2] = (String((configuration.cue_to_event_delay/1000)) + "s");
      updatable_menu[3] = String(configuration.reward_prob);
      update_menu();

      // Initialize
      experiment_start_time = millis();
      trial_start_time = millis();
      trial_number = 1;
      Serial.println("\n$ Trial#: " + String(trial_number));
      right_LED.LED_on(); left_LED.LED_on(); // turn on both LED's
  }

  // Mode 5 - Pavlovian Reward
  if (configuration.mode == 5) {
      
      // Set up parameters
      Serial.println("\nParams: ");
      Serial.println("Mode: Pavlovian Reward");
      configuration.total_trials = 25;                    Serial.println("Total Trials: " + String(configuration.total_trials));
      configuration.intertrial_interval = 10000;          Serial.println("Intertrial Interval: " + String(configuration.intertrial_interval));
      configuration.reward_duration = 100;                Serial.println("Reward_Duration: " + String(configuration.reward_duration));
      configuration.reward_prob = 100;                    Serial.println("Reward Probability: " + String(configuration.reward_prob));
      configuration.cue_to_reward_delay = 5000;           Serial.println("Cue to Reward Delay: " + String(configuration.cue_to_reward_delay));

      Serial.println("\nAlignments: ");
      Serial.println("Yellow: LED Cue Right");
      Serial.println("Black: Reward Non-Delivery");
      Serial.println("Blue: Reward Delivery");
      Serial.println("Green: Licks");
      
      // Set up Menu 
      menu_categories[0] = "mode";
      menu_categories[1] = "trial";
      menu_categories[2] = "cue_del";
      menu_categories[3] = "rew_prob";
      initialize_menu();

      updatable_menu[0] = "PavlovR";
      updatable_menu[1] = "1";
      updatable_menu[2] = (String((configuration.cue_to_reward_delay/1000)) + "s");
      updatable_menu[3] = String(configuration.reward_prob);
      update_menu();

      // Start timer 
      experiment_start_time = millis();
      trial_start_time = millis();
      trial_number = 1;
      Serial.println("\n$ Trial#: " + String(trial_number));
      right_LED.LED_on();
  }

  // Pavlovian Loom
  if (configuration.mode == 6) {
      
      // Set up parameters
      Serial.println("\nParams: ");
      Serial.println("Mode: Pavlovian Loom");
      configuration.total_trials = 25;                    Serial.println("Total Trials: " + String(configuration.total_trials));
      configuration.intertrial_interval = 10000;          Serial.println("Intertrial Interval: " + String(configuration.intertrial_interval));
      configuration.loom_duration = 5000;                 Serial.println("Loom Duration: " + String(configuration.loom_duration));
      configuration.loom_prob = 100;                      Serial.println("Loom Probability: " + String(configuration.loom_prob));
      configuration.cue_to_loom_delay = 5000;             Serial.println("Cue to Loom Delay: " + String(configuration.cue_to_loom_delay));

      Serial.println("\nAlignments: ");
      Serial.println("Yellow: LED Cue Left");
      Serial.println("Black: Loom Non-Delivery");
      Serial.println("Blue: Loom Delivery");
      Serial.println("Green: Licks");
      
      // Set up Menu 
      menu_categories[0] = "mode";
      menu_categories[1] = "trial";
      menu_categories[2] = "cue_del";
      menu_categories[3] = "loom_prob";
      initialize_menu();

      updatable_menu[0] = "PavlovL";
      updatable_menu[1] = "1";
      updatable_menu[2] = (String((configuration.cue_to_loom_delay/1000)) + "s");
      updatable_menu[3] = String(configuration.loom_prob);
      update_menu();

      // Start timer 
      experiment_start_time = millis();
      trial_start_time = millis();
      trial_number = 1;
      Serial.println("\n$ Trial#: " + String(trial_number));
      left_LED.LED_on();
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
    case 1: // Free Rewards

      // Execute the below code every time the lick sensor reads a lick
      if (lick.is_licked(10)) {                                                  
        
        // Record Data
        latency_to_lick = millis() - trial_start_time;                                      // determines latency to lick from LED onset
        Serial.print("Trial Number: "); Serial.println(String(trial_number));               // print to serial port
        Serial.print("Lick Latency: "); Serial.println(String(latency_to_lick));            // Print latency to lick to serial port
        
        // Open valve and send alignment pulse
        align_yellow.align_onset();                                                         // Alignment onset to be interpreted as lick
        delay(configuration.delay_between_lick_and_reward);                                 // Wait 
        reward_solenoid.pulse_valve(configuration.reward_duration);                         // Open solenoid to deliver reward
        align_yellow.align_offset();                                                        // Alignment offset to be interpreted as reward delivery
        
        // Display data on I2C LCD
        reward_counter += 1;                                                                // increments up the reward counter
        updatable_menu[1] = String(reward_counter);                                         // displays number of rewards received in total 
        updatable_menu[2] = String(millis() - experiment_start_time);                       // displays the time elapsed since the start of the experiment
        updatable_menu[3] = String(latency_to_lick);                                        // displays latency to lick
        update_menu();                                                           // updates menu
        
        // Reset for next trial
        right_LED.LED_off();
        delay(configuration.intertrial_interval);
        right_LED.LED_on();
        trial_start_time = millis();
        trial_number += 1;     
      }

      // concludes trial if 
      if (trial_number > configuration.total_trials) {
        complete = true;
      }
      break;
    
    
    case 2: // Reward/Puff Coinflip
      // Code 
      trial_number++;
      coinflip = flip_coin();
      Serial.print("\nTrial Number: "); Serial.println(trial_number);
      Serial.print("Coin Flipped: "); Serial.println(coinflip);
      
      // Heads means air puff
      if (coinflip == "H") {
        left_LED.LED_on();
        airpuff_solenoid.valve_on();
        align_blue.align_onset();

        delay(configuration.puff_duration);

        left_LED.LED_off();
        airpuff_solenoid.valve_off();
        align_blue.align_offset();

        Serial.println("Air puffed");
      }

      // Tails means reward
      if (coinflip == "T") {
        right_LED.LED_on();
        reward_solenoid.valve_on();
        align_black.align_onset();

        delay(configuration.reward_duration);

        right_LED.LED_off();
        reward_solenoid.valve_off();
        align_black.align_offset();

        Serial.println("Reward Delivered");
      }
      
      if (trial_number >= configuration.total_trials) {
        complete = true;
        break;
      }
      
      delay(configuration.intertrial_interval);
      break;

    case 3: // Reward_Aversive Probablistic
      
      // Alignment Designations
      //Yellow: General cue/Event (ON when any cue is presented, OFF when event takes place)
      //Blue: Reward Cue and Delivery (ON when reward cue (right) is presented, OFF when Reward Delivered)
      //Black: Loom Cue and Delivery (ON when loom cue (left) is presented, OFF when Loom Presented)
      //Green: Licks (shunted lick sensor state)
      
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

        case false: // Loom Case                                                                          
          
          left_LED.LED_on();                                                                              // Turn on left LED
          align_black.align_onset();                                                                      // Align_on to left LED                                                 
          align_yellow.align_onset();                                                                     // Align_on to general cue
          
          time_elapsed = 0;                                                                               
          while(time_elapsed < configuration.cue_to_event_delay) {                                        // While you enforce the cue to event delay:                
            align_green.align_shunt(lick.is_licked(5));                                                   //    Shunt lick sensor state to Green alignment
            time_elapsed = millis() - trial_start_time;                                                   
          }
             
          align_black.align_offset();                                                                     // Align_off to loom presentation
          align_yellow.align_offset();                                                                    // Align_off to general event
          Serial.println(":( Loom Delivered at t = " + String(millis() - trial_start_time) + "ms");       // Report Loom
          looming.loom_on(); delay(configuration.loom_duration); looming.loom_off();                      // Present Loom
          left_LED.LED_off();                                                                             // Turn off the left LED
          break;
      }

      // Conclude experiment if trial number exceeds total trials _______________________________________________________
      if (trial_number >= configuration.total_trials) {
        complete = true;
        break;
      }
      
      // Enforce the intertrial Interval ________________________________________________________________________________
      delay(configuration.intertrial_interval);                                       // Enforce Intertrial Interval
      
      // Initialize next trial __________________________________________________________________________________________
      trial_number += 1;                                                              // Increment Trials
      Serial.println("\n$ Trial#: " + String(trial_number));                          // Report Trial Increment     
      updatable_menu[1] = String(trial_number);                                       // Update Trial Number                                                                          
      update_menu();
      trial_start_time = millis();      

      break;

    case 4: // Check Alignment Integrity
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
    
    /*case 5: // Pavlovian Reward
      
      // Alignment Designations
      // Yellow: LED Cue Right
      // Black: Reward Non-Delivery
      // Blue: Reward Delivery 
      // Green: Licks

      // Wait for delay and Check for anticipatory licks_________________________________________________________________
      align_yellow.align_onset();                                                     // Align onset to LED                                                      //
      time_elapsed = 0;                                                               //
      while(time_elapsed < configuration.cue_to_reward_delay) {                        
        align_green.align_shunt(lick.is_licked(5));                                   // Shunt state of lick sensor to green alignment
        time_elapsed = millis() - trial_start_time;                                        // update elapsed time variable
      }                                                                                            
      
      // Deliver reward probablistically_________________________________________________________________________________
      switch (enforce_prob(configuration.reward_prob)) {
        case true:
          align_blue.align_onset(); 
          Serial.println(":) Reward Delivered at t = " + String(millis() - trial_start_time) + "ms"); 
          reward_solenoid.pulse_valve(configuration.reward_duration);
          break;
        case false:
          align_black.align_onset();
          Serial.println(":( Reward Omitted at t = " + String(millis() - trial_start_time) + "ms");
          delay(configuration.reward_duration); 
          break;
      }
      
      // Finalize the Trial _____________________________________________________________________________________________
      align_blue.align_offset(); align_yellow.align_offset(); align_black.align_offset();    // offset the alignments   
      right_LED.LED_off();                                                                   // turn off the light

      // Conclude experiment if trial number exceeds total trials _______________________________________________________
      if (trial_number >= configuration.total_trials) {
        complete = true;
        break;
      }
      
      // Enforce the intertrial Interval ________________________________________________________________________________
      delay(configuration.intertrial_interval);
      
      // Initialize next trial __________________________________________________________________________________________
      right_LED.LED_on();
      trial_number += 1;
      Serial.println("\n$ Trial#: " + String(trial_number));                                          
      updatable_menu[1] = String(trial_number);                                       // update trial number                                                                          
      update_menu();
      trial_start_time = millis();      

      break;
  
    *//*case 6: // Pavlovian Loom...
      
      // Alignment Designations
      // Yellow: LED Cue Left
      // Black: Loom Non-Delivery
      // Blue: Loom Delivery 
      // Green: Licks

      // Wait for delay and Check for anticipatory licks_________________________________________________________________
      align_yellow.align_onset();                                                     // Align onset to LED                                                      //
      time_elapsed = 0;                                                               //
      while(time_elapsed < configuration.cue_to_loom_delay) {                        
        align_green.align_shunt(lick.is_licked(5));                                   // Shunt state of lick sensor to green alignment
        time_elapsed = millis() - trial_start_time;                                   // update elapsed time variable
      }                                                                                            
      
      // Deliver reward probablistically_________________________________________________________________________________
      switch (enforce_prob(configuration.loom_prob)) {
        case true:
          align_blue.align_onset(); 
          Serial.println(":( Loom Delivered at t = " + String(millis() - trial_start_time) + "ms"); 
          looming.loom_on();
          delay(configuration.loom_duration);
          looming.loom_off();
          break;
        case false:
          align_black.align_onset();
          Serial.println(":) Loom omitted at t = " + String(millis() - trial_start_time) + "ms");
          delay(configuration.loom_duration); 
          break;
      }
      
      // Finalize the Trial _____________________________________________________________________________________________
      align_blue.align_offset(); align_yellow.align_offset(); align_black.align_offset();    // offset the alignments   
      left_LED.LED_off();                                                                   // turn off the light

      // Conclude experiment if trial number exceeds total trials _______________________________________________________
      if (trial_number >= configuration.total_trials) {
        complete = true;
        break;
      }
      
      // Enforce the intertrial Interval ________________________________________________________________________________
      delay(configuration.intertrial_interval);
      
      // Initialize next trial __________________________________________________________________________________________
      left_LED.LED_on();
      trial_number += 1;
      Serial.println("\n$ Trial#: " + String(trial_number));                                          
      updatable_menu[1] = String(trial_number);                                       // update trial number                                                                          
      update_menu();
      trial_start_time = millis();      

      break;
    */
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
