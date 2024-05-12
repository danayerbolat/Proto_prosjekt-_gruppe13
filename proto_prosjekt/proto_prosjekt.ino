/*
  Kode biblioteker
*/
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <FastLED.h>
#include <math.h>

/*
  Definere pins for Adafruit Music Player,
  disse er hentet fra eksempelkoden 'player_simple' i Adafruit VS1053 biblioteket
*/
#define CLK 13   // SPI Clock, shared with SD card
#define MISO 12  // Input data, from VS1053/SD card
#define MOSI 11  // Output data, to VS1053/SD card

#define SHIELD_RESET -1  // VS1053 reset pin (unused!)
#define SHIELD_CS 7      // VS1053 chip select pin (output)
#define SHIELD_DCS 6     // VS1053 Data/command select pin (output)

#define CARDCS 4  // Card chip select pin
#define DREQ 3    // VS1053 Data request, ideally an Interrupt pin

/*
  Definere LED-matrisen
  Disse er hentet fra FastLED biblioteket og modifisert for å prosjektet vårt
*/
#define NUM_LEDS_MATRISE 64
#define NUM_LEDS_TROMMER 30
#define LED_PIN_MATRISE 9
#define COLOR_ORDER GRB
#define CHIPSET_MATRISE WS2812B

CRGB matriseLeds[NUM_LEDS_MATRISE];

CRGB farger[] = {
  CRGB::DarkViolet,
  CRGB::Orange,
  CRGB::Crimson,
  CRGB::Blue
};

/*
  Definere admin buttons på siden av LED-matrisen
  Pin 2: Minske BPM
  Pin A5: Record
  Pin 5: Øke BPM
  Pin 7: Reset beatArray og reset BPM = 60
  Pin A4: Sette stortromme til å spille en baserytme
*/
int admBtnPin[] = { 2, A5, 5, 7 , A4}; //Array for admin button pins
int lengdeAdm = 5; //Lengden til admBtnPin array
bool record = true; // Flag for recording mode

/*
  Definere drumpads
*/
#define stortrommePin A0
#define skarptrommePin A1
#define hihatPin A2
#define symbalPin A3

const int trommePins[] = { A0, A1, A2, A3 }; //Array for drum trigger pins
const int antTrommer = 4; //Antall drums
int buttonStates[antTrommer] = { 1, 1, 1, 1 };      //Nåværene tilstand av knappene
int lastButtonStates[antTrommer] = { 1, 1, 1, 1 };  //Tidligere tilstand av knappene
unsigned long lastDebounceTimes[antTrommer];        //Tiden siste knappe tilstand endret seg
unsigned long debounceDelay = 100; //Debounce delay time


const int antBeats = 16; //Antall beats
int beatArray[antBeats];  //Definere en 1d array for å lagre beats

int beatNr = 0; //Nåværende beat nr
int BPM = 60; //Beats per minute 
uint32_t forrigeTid = 0; //Forrige tid
int intervall = 0; //Intervall mellom beats

//Initialisere en klasse for music player
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

void setup() {
  //Her fylles beatArray med -1
  for (int j = 0; j < 16; j++) {
    beatArray[j] = -1;
  }

  initilizeSoundModule(); //Initialisere funksjonene brukt til musicplayeren
  
  beatArray[0] = 1; 

   // Sette pin modes for admin buttons og tromme triggers
  for (int i = 0; i < lengdeAdm; i++) {
    pinMode(admBtnPin[i], INPUT_PULLUP);
  }
  for (int i = 0; i < antTrommer; i++) {
    pinMode(trommePins[i], INPUT_PULLUP);
  }

  //Initialisere LED matrise
  FastLED.addLeds<CHIPSET_MATRISE, LED_PIN_MATRISE, COLOR_ORDER>(matriseLeds, NUM_LEDS_MATRISE).setCorrection(TypicalLEDStrip);
}

void loop() {
  int aktuellTromme = lesTrommeBtn(); //Les tilstanden til trommene 
  
  
  if (record) {
    increaseBeatNr(); //Hvis dette er i recording modus, øke beatNr og oppdatere displayet
    skrivTilArray(aktuellTromme); // Skriv tromme-inputen til beatArray
    

  } else {
    FastLED.clear(); 
    FastLED.show(); 

    if (aktuellTromme != 0) {
      spillLyd(aktuellTromme); //Dersom en (eller flere) tromme er aktiv, spill den korresponderende lyden
    }
  }
  readAdmBtn(); //Les admin buttons for bruker input
}

/*
  Funksjon som behandler bruker input fra admin buttons og endrer samsvarende oppførselen til koden  
*/
void readAdmBtn() {
  bool admBtn[] = { 0, 0, 0, 0, 0}; //array for admin buttons

  //Itere over alle admBtn og lese dens digital verdi
  for (int i = 0; i <= lengdeAdm; i++) {
    admBtn[i] = !digitalRead(admBtnPin[i]);
  }

  //Minske BPM, minimum BPM = 40
  if (admBtn[0]) {
    BPM -= 2;
    delay(debounceDelay/2);
    if (BPM < 40) {
      BPM = 40;
    }
  }

  //Slå av/på recording mode 
  if (admBtn[1]) {
    record = !record;
    delay(500);
  }

  //Øke BPM, max BPM = 120
  if (admBtn[2]) {
    BPM += 2;
    delay(debounceDelay/2);
    if (BPM > 120) {
      BPM = 120;
    }
    Serial.println(BPM);
  }

  //Reset beatArray og reset BPM til 60
  if (admBtn[3]) {
    resetArray();
    BPM = 60;
  }

  //Sette en base rytme med stortromme som skal spilles hver fjerde beat
  if (admBtn[4]){
    int hverFjerde[] = {0,4,8,12};
    for (int i = 0; i < 4; i ++){
      if (beatNr == hverFjerde[i]){beatArray[hverFjerde[i]] = 3;}
      else {beatArray[hverFjerde[i]] = -3;}
    }
  }

}

/*
  Funksjon som øker beatNr 
*/
void increaseBeatNr() {
  uint32_t tidNaa = millis(); //Får tida nå i millisekunder
  intervall = ((float(60) / float(4)) * 1000) / (BPM);  //Regne ut intervallen mellom beats basert på BPM
  //her hadde jeg mye problemer med bit overflow og avrundingsfeil, derfor er det float i alle ledd  
  
  if (tidNaa - forrigeTid >= intervall) { 
    forrigeTid = tidNaa; //Sett forrige tid til tiden nå
    beatNr++; //øke beatNr
    beatNr %= 16;
    flyttTaktLinje();  // flytt taktlinjen en frem når beaten endres
    arrToLight(); //oppdatere LED-matrisen
    spillLyd(beatArray[beatNr]); //Spille lyden som samsvarer med beaten
  }
}

void flyttTaktLinje() {
  beatArray[beatNr] *= -1;  //flytter taktstreken til neste steg i takt med beatNr variabel

  if (beatNr == 0) {      // Denne koden nedover her skal skru av forrige taktstrek når man har gått videre
    beatArray[15] *= -1;  //Dette er i tilfelle du er på første slaget
  }

  else {
    beatArray[beatNr - 1] *= -1;  //Dette er bare skru av forrige beat strek
  }

}

/*
  Funksjonen oppdaterer beatArray basert på inputs fra trommene
*/
void skrivTilArray(int trommeVerdi) {
  int nyArrVerdi = trommeVerdi | beatArray[beatNr];  //litt binærlogikk for å ikke få dobbelt opp av en enkelt tromme
  if (beatArray[beatNr] != nyArrVerdi) { //sjekker om det er en endring i verdien i beatArray
    beatArray[beatNr] = nyArrVerdi; //om det er, oppdatere beatArray med den nye verdien
    arrToLight(); //oppdatere LED-matrisen
  }
}

/*
  Funksjonen som spiller lyd basert på input fra tromme-knapene 
*/
void spillLyd(int trommeNr) {
  musicPlayer.stopPlaying();

  //denne metoden kan være så enkel fordi vi har kalt filene våre /dr- også binærkoden for kombinasjonen av hvilke tromme vi vil ha - .mp3
  trommeNr--;  //Skrev feil binærkode på filene, så fikser det i kode istedet for å endre alle filene

  if (trommeNr != 0) { //dersom trommeNr er ikke lik 0, altså at det er én tromme som er aktiv:
    String filbane = String("/dr") + trommeNr + String(".mp3"); //Lage en path til den korresponderende trommelydfilen.
    Serial.println(filbane);
    musicPlayer.startPlayingFile(filbane.c_str()); //starte å spille tromme lyd-filen
  }
}

/*
  Funksjon som resetter beatArray
*/
void resetArray() {
  //Reset alle leddene i beatArrayu til -1
  for (int i = 0; i < 16; i++) {
    beatArray[i] = -1;
  }
  beatArray[beatNr] = 1; //Sette beatNr til 1, altså sette den tilbake til start posisjonen
  fill_solid(matriseLeds, 64, CRGB::Black); //Slå av alle LEDene i matrisen
  FastLED.show();
}

/*
  Funksjonen oppdaterer LED-matrisen basert på beatene lagret i beatArray
*/
void arrToLight() {
  /*
    om verdien er 1 betyr det at cellen er tom
    stortromme har verdi 2
    skarptromme har verdi 4
    hihat har verdi 8
    symbal har verdi 16
  */
  FastLED.clear();

  if (record) {
    //Om recording-modus er på, oppdatere LED-matrisen til å visualisere beatene 
    for (int k = 0; k < antBeats; k++) {
      if (beatNr == k) {
        for (int i = 0; i < 4; i++) {
          matriseLeds[antBeats * i + k] = CRGB::BurlyWood;
        }
      } else {
        for (int i = 0; i < 4; i++) {
          //Serial.println(antBeats * i + k);
          matriseLeds[antBeats * i + k] = CRGB::Black;
        }
      }
    }

    //Oppdatere LED-matrisen basert på beatene lagret i beatArrayen
    for (int j = 0; j < antBeats; j++) {
      if (beatArray[j] != 1 || beatArray[j] != -1) {
        int decompTall = beatArray[j]; //Decompose verdien i beatArray til individuelle trommeslag
        int binaryArray[5]; //Array for å lagre binære verdi til trommeslaget 

        //Konvertere beatArray verdien til en binær verdi
        for (int i = 4; i >= 0; i--) {
          binaryArray[i] = decompTall % 2;
          decompTall /= 2;
        }

        //Oppdatere LED-matrisen basert på trommeslag
        for (int i = 0; i < 4; i++) {
          if (binaryArray[3 - i]) {
            matriseLeds[16 * i + j] = farger[i]; // Sette LED farger basert på trommeslag 
          }
        }
      }
    }

    FastLED.show();
  }
}

/*
  Funksjon for å printe hele arrayet på en linje, brukt mest for debugging
*/
void printArray() {  
  for (int j = 0; j < antBeats; j++) {
    Serial.print(beatArray[j]);
    Serial.print(" ");
  }
  Serial.println();
}

/*
  Funksjonen som skal initialisere musicplayeren
  denne er hentet direkte fra eksempelkoden 'simple_player' som er en del av adafruit Music Player VS1053 sin innebygd bibliotek
*/
void initilizeSoundModule() {
  Serial.println("Adafruit VS1053 Simple Test");

  //Initialize musicPlayer:
  if (!musicPlayer.begin()) {  // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1)
      ;
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1)
      ;  // don't do anything more
  }

  // list files
  printDirectory(SD.open("/"), 0);

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(1, 1);

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
}

/*
  Funksjonen som skal printe ut innholdet i SD-kortet for enklere filhandling og debugging
  denne er hentet direkte fra eksempelkoden 'simple_player' som er en del av adafruit Music Player VS1053 sin innebygd bibliotek
*/
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      //Serial.println("nomorefiles");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

/*
  Funksjonen for å lese trommeslagene, den skal bruke arrays istedenfor enkeltverdier, ha debounce og edgedetecting 
*/
int lesTrommeBtn() {
  int trommeVerdi = 0;
  
  for (int i = 0; i < antTrommer; i++) {
    int reading = !digitalRead(trommePins[i]);  //sette en lokal variabel til 0 når knappen ikke er trykket og 1 når den er trykket

    //Her sjekker den at knappen har endret seg OG at den før var 0 (altså både rising edge og debounce i en)
    //Om dette skjer skal den starte et debounce timer og lagre verdien til lastButtonStates[i]
    if (reading != lastButtonStates[i] && lastButtonStates[i] == 0) {
      // Reset the debounce timer
      lastDebounceTimes[i] = millis();
    //Serial.println("ny tromme trykket"); //litt feilsøking
      lastButtonStates[i] = reading;
    }

    // Her sjekker den at knappen ikke har blitt trykket de siste 100ms 
    if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
      if (reading != buttonStates[i]) {
        buttonStates[i] = reading;  // Dette passer på at koden blir kun kjørt en gang
        if (buttonStates[i] == HIGH) {
          trommeVerdi += 1 << i + 1;  //litt gangster bitmanipulasjon for å forenkle tidligere kode 
          /*den gjør det samme som dette, den er bare mye kulere
            int trommeVerdi = 0;
            trommeVerdi += 2 * !digitalRead(stortrommePin);
            trommeVerdi += 4 * !digitalRead(skarptrommePin);
            trommeVerdi += 8 * !digitalRead(hihatPin);
            trommeVerdi += 16 * !digitalRead(symbalPin);
          */
        }
      }
    }

    lastButtonStates[i] = reading;
  }

  //Dette er for å spille lyden ide knappen blir trykket
  if (trommeVerdi > 1) {
    trommeVerdi++;
    spillLyd(trommeVerdi);
  }
  return (trommeVerdi);
}