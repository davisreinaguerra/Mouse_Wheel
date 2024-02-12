//Custom Classes_____________________
#include "lick_sensor.h"
#include "solenoid.h"
#include "alignment.h"
#include "looming.h"
#include "LED.h"
#include "ProbabilityFunctions.h"

//Pin Assignments_____________________
                                 // D1   unassigned
                                 // D2   unassigned
alignment align_yellow(3);       // D3   Yellow alignment wire
alignment align_black(4);        // D4   Black alignment wire
alignment align_blue(5);         // D5   Blue alignment wire
alignment align_green(6);        // D6   Green alignment wire
                                 // D7   unassigned
looming looming(8);              // D8   basic trigger using NPN on MatrixPortal M4
LED left_LED(9);                 // D9   Green LED wire (left mouse perspective)
LED right_LED(10);               // D10  Yellow LED wire (right mouse perspective)
lick_sensor lick(11);            // D11  Wired to cap sensor
solenoid reward_solenoid(12);    // D12  
solenoid airpuff_solenoid(13);   // D13  


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
  long total_rewards_allowed;
} configuration;

//_________Variable Initializations_________________
bool complete = false;
int trial_number = 0;
String coinflip = flip_coin();
long reward_counter = 0;
int menu_offset = 0;
long latency_to_lick;
long experiment_start_time;
long trial_start_time;

//_________LCD_Screen_______________________________
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);
String menu_categories[4] = {"", "", "", ""};
String updatable_menu[4] = {"", "", "", ""};


//_________Modes____________________________________
//  1: free rewards
//  2: Reward Puff Cue Probablistic
//  3: Looming every 20

void setup() {
  // Set up serial port
  Serial.begin(9600);

  // Initialize LCD Screen_____________________
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("I'm afraid I cant do");
  lcd.setCursor(0, 1); lcd.print("that Dave...");
  lcd.setCursor(6, 3); lcd.print("Treadmill_v1.1");

  delay(5000); // hold on title 

  // Manual Setup______________________________
  configuration.mode = 1;
  configuration.reward_duration = 100;
  configuration.puff_duration = 200;
  configuration.trial_duration = 600000;
  configuration.delay_between_lick_and_reward = 50;
  configuration.intertrial_interval = 10000;
  configuration.total_trials = 50;

  // GUI Setup_________________________________
  //configuration.mode = read_config();
  //configuration.date = read_config();
  //configuration.animal_code = read_config();
  //configuration.reward_duration = read_config();

  // Initialize timer for timeout and other time-based calculations

  // Setting up menu and initilaization stuff based on mode
  if (configuration.mode == 1) {
      menu_offset = 12;
      menu_categories[0] = "mode";
      menu_categories[1] = "rewards";
      menu_categories[2] = "elapsed";
      menu_categories[3] = "latency";

      initialize_menu(menu_offset);
      updatable_menu[0] = "free";
      update_menu(menu_offset);
      right_LED.LED_on();
      experiment_start_time = millis();
      trial_start_time = millis();
      trial_number = 1;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  switch (configuration.mode) {
    case 1:

      // Execute the below code every time the lick sensor reads a lick
      if (lick.is_licked(10)) {                                                  
        
        // Record Data
        latency_to_lick = millis() - trial_start_time;                                            // determines latency to lick from LED onset
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
        update_menu(menu_offset);                                                           // updates menu
        
        // Reset for next trial
        right_LED.LED_off();
        delay(configuration.intertrial_interval);
        right_LED.LED_on();
        trial_start_time = millis();
        trial_number += 1;     
      }

      // concludes trial if 
      if (trial_number > configuration.total_trials) {
        complete == true;
      }
      break;
    
    
    case 2:
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

    case 3:
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

// Custom Functions
void initialize_menu(int menu_line) {
  lcd.clear();

  // first line - Sound
  lcd.setCursor(menu_line, 0);
  lcd.print(menu_categories[0]);

  // second line - Loudness
  lcd.setCursor(menu_line, 1);
  lcd.print(menu_categories[1]);

  // third line - Delay
  lcd.setCursor(menu_line, 2);
  lcd.print(menu_categories[2]);

  // fourth line - Duration
  lcd.setCursor(menu_line, 3);
  lcd.print(menu_categories[3]);
}

void update_menu(int menu_line) {
  initialize_menu(menu_line);

  // first line - Sound
  lcd.setCursor(0, 0);
  lcd.print(updatable_menu[0]);

  // second line - Loudness
  lcd.setCursor(0, 1);
  lcd.print(updatable_menu[1]);

  // third line - Delay
  lcd.setCursor(0, 2);
  lcd.print(updatable_menu[2]);

  // fourth line - Duration
  lcd.setCursor(0, 3);
  lcd.print(updatable_menu[3]);
}
