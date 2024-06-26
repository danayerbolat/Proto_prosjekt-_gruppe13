﻿# BeatBox
Dette er den tilhørende Arduino-koden for vårt prosjekt "BeatBox" i faget TPD4126: Prototyping av fysiske brukeropplevelser. <br />
### Gruppe 13: Solveig Heilund Midttun, Sara Gravdal Vølstad, Dana Yerbolat og Tord Agnalt Østmo<br />
<br />
Målet med prosjektet er å lage en interaktiv modell med fokus på temaet tid. Denne modellen, rettet mot barn i alderen 5-13 år, skal vli en del av utstillingen på Vitensenteret i Trondheim. Vi har valgt å designe og utvikle en rytme-maskin, som inspirer barna til å utforske rytme som en representasjon av tid. <br />
<br />

![IMG_5223](https://github.com/danayerbolat/Proto_prosjekt/assets/80036171/942d93bc-678c-4111-b05c-3fd1d1af90b7)

<br />

### Funksjoner i modellen vår inkluderer: 
- Free play, hvor brukeren kan spille fritt på rytme-maskinen. 
- Record, hvor brukeren kan 'spille inn' taktene og bygge videre til å lage en komplisert rytme 
- BPM: BPM kan justeres opp og ned
- Premade beats: Legger inn en premade beats av stortrommer som man kan bygge videre på.
- Reset: fjerne tidligere beats og lage en ny en!

### Utstyr:
- Adafruit Music Player VS1053 
- LED-lys
- Diverse knapper

### Kodebiblioteker som vi har brukt i dette prosjektet er: 
- <SPI.h>
- <SD.h>
- <math.h>
- <Adafruit_VS1053.h>, hvorav koden fra eksempelkoden 'simple_player' har blitt tatt i bruk: https://github.com/adafruit/Adafruit_VS1053_Library/tree/master/examples/player_simple
- <FastLED.h>, hvorav eksempelkodene har blitt tatt i bruk for å definere LED-lysene: https://github.com/FastLED/FastLED
