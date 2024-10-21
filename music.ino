#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define SHIELD_RESET  -1     
#define SHIELD_CS     7      
#define SHIELD_DCS    6     
#define CARDCS 4
#define DREQ 3     

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

void setup() {
  Serial.begin(9600);

  if (! musicPlayer.begin()) {
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);
  }

  printDirectory(SD.open("/"), 0);
  
  musicPlayer.setVolume(80,80);
}

void loop() {
  musicPlayer.playFullFile("/1KHZ.wav");
  delay(1000);
  musicPlayer.playFullFile("/2KHZ.wav");
  delay(1000);
  musicPlayer.playFullFile("/3KHZ.wav");
  delay(1000);
  musicPlayer.playFullFile("/4KHZ.wav");
  delay(1000);
  musicPlayer.playFullFile("/5KHZ.wav");
  delay(1000);
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
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
