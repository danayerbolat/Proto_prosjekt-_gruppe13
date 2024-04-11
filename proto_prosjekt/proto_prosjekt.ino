//Proto prosjekt - skriv kommentarer og dokumenter alle endringene som har blitt gjort.
#include <Adafruit_NeoPixel.h> //hvis vi bruker NeoPixel LED striper

#define NUM_STRIPS 4 //define number of LED strips: 4
#define LED_COUNT_PER_STRIP 16 //define the number of LEDs on each strip

// Define the pin numbers for each LED strip
#define STRIP_1_PIN 1
#define STRIP_2_PIN 2
#define STRIP_3_PIN 3
#define STRIP_4_PIN 4

//NeoPixel array that allows us to manipulate each of the led trips 
Adafruit_NeoPixel strips[NUM_STRIPS] = {
  Adafruit_NeoPixel(LED_COUNT_PER_STRIP, STRIP_1_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LED_COUNT_PER_STRIP, STRIP_2_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LED_COUNT_PER_STRIP, STRIP_3_PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(LED_COUNT_PER_STRIP, STRIP_4_PIN, NEO_GRB + NEO_KHZ800)
};

//Initialize the LEDarray
int LEDarray[4][16];



void setup() {
  //Initialize the LED strips: 
  for (int i = 0; i < NUM_STRIPS; i++) {
    strips[i].begin();
    strips[i].show(); // Initialize all pixels to 'off'
  }
} 

/*
    Beat Counter
    This is function that indicates the beat. Each of the columns shall light up at the same time indicating the beat.
    What to fix: the last light is always on, needs to turn everything off 
*/
void beatCounter(){
  for (int i = 0; i < LED_COUNT_PER_STRIP; i++) {
    for (int j = 0; j < NUM_STRIPS; j++) {
      // Set color (red, green, blue) for each LED
      strips[j].setPixelColor(i-1, 0, 0, 0); // Red color (RGB)
      strips[j].setPixelColor(i, 0, 255, 0); // Red color (RGB)
      strips[j].show();
    }
    delay(500); // Change to bpm when that is set up. 
  }
}

void loop() {
  beatCounter();
}
