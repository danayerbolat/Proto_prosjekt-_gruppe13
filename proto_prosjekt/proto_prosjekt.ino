#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>


//Proto prosjekt - skriv kommentarer og dokumenter alle endringene som har blitt gjort.


/*
  Define LED strips
*/
#define NUM_STRIPS 4 //define number of LED strips: 4
#define LED_COUNT_PER_STRIP 16 //define the number of LEDs on each strip

// Define the pin numbers for each LED strip
#define STRIP_1_PIN 1
#define STRIP_2_PIN 2
#define STRIP_3_PIN 3
#define STRIP_4_PIN 4

//NeoPixel array that allows us to manipulate each of the led trips 
/*Adafruit_NeoPixel strips[NUM_STRIPS] = {
  Adafruit_NeoPixel(LED_COUNT_PER_STRIP, STRIP_1_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LED_COUNT_PER_STRIP, STRIP_2_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LED_COUNT_PER_STRIP, STRIP_3_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LED_COUNT_PER_STRIP, STRIP_4_PIN, NEO_GRB + NEO_KHZ800)
};*/

//Initialize the LEDarray
int LEDarray[4][16];

/*
  Define music player
*/
#define CLK 13       // SPI Clock, shared with SD card
#define MISO 12      // Input data, from VS1053/SD card
#define MOSI 11      // Output data, to VS1053/SD card

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

/*
    Beat Counter
    This is function that indicates the beat. Each of the columns shall light up at the same time indicating the beat.
    What to fix: the last light is always on, needs to turn everything off 
*/
/*void beatCounter(){
  for (int i = 0; i < LED_COUNT_PER_STRIP; i++) {
    for (int j = 0; j < NUM_STRIPS; j++) {
      // Set color (red, green, blue) for each LED
      strips[j].setPixelColor(i-1, 0, 0, 0); // Red color (RGB)
      strips[j].setPixelColor(i, 0, 255, 0); // Red color (RGB)
    }
    // Show the updated state of LEDs for each strip
    for (int j = 0; j < NUM_STRIPS; j++) {
      strips[j].show(); 
    }
    delay(500); // Change to bpm when that is set up.
  }
  // Turn off all LEDs after the loop completes
  for (int j = 0; j < NUM_STRIPS; j++) {
    for (int i = 0; i < LED_COUNT_PER_STRIP; i++) {
      strips[j].setPixelColor(i, 0, 0, 0); // Turn off all LEDs
    }
    strips[j].show(); // Show the updated state
  }
}*/
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


/*void playSoundFile(char filename[]) {
  Serial.print("Playing file: ");
  Serial.println(filename);
  
  if (!musicPlayer.playFullFile(filename)) {
    Serial.println("Error playing file!");
    return;
  }
  
  Serial.println("Finished playing file.");
}*/


void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");
  //Initialize the LED strips: 
  /*for (int i = 0; i < NUM_STRIPS; i++) {
    strips[i].begin();
    strips[i].show(); // Initialize all pixels to 'off'
  }*/

  //Initialize musicPlayer:
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
   if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  printDirectory(SD.open("/"), 0);

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(1,1);

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
} 

void loop() {
  //beatCounter();
  Serial.println(F("Playing track dr2"));
  musicPlayer.playFullFile("/dr2.mp3");
  delay(2000);
  // Play another file in the background, REQUIRES interrupts!
  Serial.println(F("Playing track dr28"));
  musicPlayer.startPlayingFile("/dr28.mp3");
  delay(2000);
  Serial.println(F("Playing track dr30"));
  musicPlayer.startPlayingFile("/dr30.mp3");
  delay(2000);
  Serial.println(F("Playing track dr6"));
  musicPlayer.startPlayingFile("/dr6.mp3");
}
