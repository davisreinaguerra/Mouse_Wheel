// Libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>

// Standard definitions for Adafruit MusicMaker
#define CLK 13                // SPI Clock, shared with SD card
#define MISO 12               // Input data, from VS1053/SD card
#define MOSI 11               // Output data, to VS1053/SD card
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7       // VS1053 chip select pin (output)
#define SHIELD_DCS    6       // VS1053 Data/command select pin (output)
#define CARDCS 4              // Card chip select pin
#define DREQ 3                // VS1053 Data request, ideally an Interrupt pin

// Trigger
#define trig_pin 9

// Variable initializations/configurations
bool trig_state;

// I2C Liquid Crystal Display
LiquidCrystal_I2C lcd(0x27,20,4);
String menu_categories[4] = {"sound","loudness","delay",""};
String updatable_menu[4] = {"", "", "", ""};

// musicPlayer object
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

// Manual Config ________________________________________________________
String sound_filestring = "sweep";
long volumeL = 60;
long volumeR = 60;
long delay_between_trig_and_sound = 0;
// crescendo
uint8_t crescendo_start_vol = 80;
uint8_t crescendo_end_vol = 40;
long crescendo_delay = 5;

void setup() {
  Serial.begin(9600);

  // Set up trigger
  pinMode(trig_pin, INPUT);

  // Initialize LCD screen
  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  // Check for Connections
  if (!musicPlayer.begin()) {while (1);} print_on_line(1, "Music Player Found");
  if (!SD.begin(CARDCS)) {while (1);} print_on_line(2, "SD Card Found");
  delay(1000);

  // Initialize Menu
  initialize_menu();
  updatable_menu[0] = String(sound_filestring);
  updatable_menu[1] = "L: " + volumeL + " R:" + volumeR;
  updatable_menu[2] = String(delay_between_trig_and_sound);
  update_menu();

  // Show the available files
  //printDirectory(SD.open("/"), 0);

  /*
  for(uint8_t i = crescendo_start_vol; i > crescendo_end_vol; i--) {
    musicPlayer.setVolume(i, i);
    musicPlayer.startPlayingFile("/white_~1.wav");
    delay(crescendo_delay);
  }
  */

  musicPlayer.setVolume(volumeL, volumeR);

}



void loop() {

  trig_state = digitalRead(trig_pin);
  Serial.println(trig_state);

  switch (trig_state) {
    case LOW:
      break;
    case HIGH:
      delay(delay_between_trig_and_sound);
      musicPlayer.playFullFile("/sweep_~1.wav");
      break;
  }

  delay(50);

}

// Function Declarations_________________________________


void print_on_line(int line, String message) {
  lcd.setCursor(0, line-1);
  lcd.print(message);
}

void update_menu() {
  initialize_menu();

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


void initialize_menu() {
  lcd.clear();

  // first line - Sound
  lcd.setCursor(12, 0);
  lcd.print(menu_categories[0]);

  // second line - Loudness
  lcd.setCursor(12, 1);
  lcd.print(menu_categories[1]);

  // third line - Delay
  lcd.setCursor(12, 2);
  lcd.print(menu_categories[2]);

  // fourth line - Duration
  lcd.setCursor(12, 3);
  lcd.print(menu_categories[3]);
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
