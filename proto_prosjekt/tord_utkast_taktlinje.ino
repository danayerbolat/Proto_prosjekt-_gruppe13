int beatArray[4][16]; //definere 2d arrayet som skal lagre beatsene

int beatNr = 0;
int BPM = 60;
  
void setup(){	
    for (int i = 0; i < 4; i++) { // her fyller jeg arrayet med 1
    for (int j = 0; j < 16; j++) {
      beatArray[i][j] = 1;
    }
  }
  

  for (int i = 0; i <4; i++){	//setter startstreken til å være på index 0
    beatArray[i][0] *= -1;
  }
 Serial.begin(9600);
 printArray();
}

void loop()
{
  increaseBeatNr(); //bruker millis funksjonen til å få 16 delene til å gå

  

}

void increaseBeatNr(){
  if ((millis() % BPM*16/60*1000) == 0){
  	beatNr++;
    beatNr %= 16;
    flyttTaktLinje();	// flytt taktlinjen en frem når beaten endres
  printArray();
  }
}
void flyttTaktLinje(){
	for (int i = 0; i < 4; i++){	//flytter taktstreken til neste steg i takt med beatNr variabel
      beatArray[i][beatNr] *= -1;}
      if (beatNr == 0){			// Denne koden nedover her skal skru av forrige taktstrek når man har gått videre
      	for (int i = 0; i <4; i++){	//Dette er i tilfelle du er på første slaget
          beatArray[i][15] *= -1;} 
      }
      else{
        for (int i = 0; i <4; i++){	//Dette er bare skru av forrige beat strek
          beatArray[i][beatNr-1] *= -1;}
      }
          
      
    }

void twoDimArrToLight(){
/*
 om verdien er negativ, betyr det at den spilles nå
 om verdien er 1 betyr det at cellen er tom
 stortromme har verdi 2
 skarptromme har verdi 4
 hihat har verdi 8
 symbal har verdi 16
 
*/
  
}
void printArray(){
    for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 16; j++) {
      Serial.print(beatArray[i][j]);
      Serial.print(" ");
    }
    Serial.println(); // Move to the next line after printing each row
  }
 Serial.println();
}
