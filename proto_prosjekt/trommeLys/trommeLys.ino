/*
  Biblioteket brukt for trommelysene
*/
#include <FastLED.h>

#define LED_PIN_TROMMER 10
#define NUM_LEDS_TROMMER 32
#define COLOR_ORDER_TROMMER BRG
#define CHIPSET_TROMMER WS2811  


CRGB leds[NUM_LEDS_TROMMER];

void setup() {
FastLED.addLeds<CHIPSET_TROMMER, LED_PIN_TROMMER, COLOR_ORDER_TROMMER>(leds, NUM_LEDS_TROMMER).setCorrection(TypicalLEDStrip);
}


void loop() {
trommeLys(); //kalle trommeLys funksjonen
delay(10000); //Delay for 10 sekunder
}

/*
  Funksjonen for å sette lyset rundt trommene
*/
void trommeLys() {
  //Definere starten og slutten på hver tromme
  #define startpixel_hihat 0
  #define sluttpixel_hihat 7

  #define startpixel_stortromme 7
  #define sluttpixel_stortromme 17

  #define startpixel_symbal 17
  #define sluttpixel_symbal 23

  #define startpixel_skarptromme 23
  #define sluttpixel_skarptromme 32

  //Definere hue verdi for hver av trommene
  #define hue_skarptromme 277  //gul
  #define hue_stortromme 232   //lilla
  #define hue_hihat 250        //rosa
  #define hue_symbal 180       //blå

  #define bright 180
  #define dim 100

  //Sette fargen for hver tromme
  for (int i = startpixel_skarptromme; i < sluttpixel_skarptromme; i++) {
    leds[i] = CHSV(hue_skarptromme, 252, bright);  // full brightness
    }


  //stortromme
    for (int i = startpixel_stortromme; i < sluttpixel_stortromme; i++) {
      leds[i] = CHSV(hue_stortromme, 255, bright);  // full brightness
      //FastLED.show();
    }
  

  //hihat
    for (int i = startpixel_hihat; i < sluttpixel_hihat; i++) {
      leds[i] = CHSV(hue_hihat, 250, bright);
    }
  

  //symbal
    for (int i = startpixel_symbal; i < sluttpixel_symbal; i++) {
      leds[i] = CHSV(hue_symbal, 235, bright);
    }

  FastLED.show(); //oppdatere LED displayet
}