/*********************************************************************
* Servomotor für Lenkung                              Stand 12.05.2023
*   Servomotoren erhalten alle 20ms einen Puls zwischen 0,5 und 2,5ms
*   entsprechend dem Stellwinkel 0 bis 180°, das Signal nur 1s senden 
*   um Strom zu sparen. Der Servo hat nur eine Sek Zeit zum Stellen,
*   daher nur kleinere Winkel-Schritte ausführen. 
*   
*   globale Variable int lenkWinkel definieren
*   Einbinden der Routine mit:
*     #include "Servo.h"
*   für Setup:
*     servoSetup();
*   für Loop:
*     servo(wi); mit Winkel wi 0..180 als Parameter
**********************************************************************/
#define SVP 15       // Anschluss A1! ServoPort

// globale Variable:

int lenkWinkel = 90;      // Lenkbereich 50..110°

void servoSetup(){
  pinMode(SVP, OUTPUT);
}

void servo(int wi){
  static long timer;          // Wecker für Mikrosekunden 
  static int puls;            // Pulslänge "Ein" und Restlänge bis 20000us "Aus"
  static int wiOld;           // Hilfsvariable Winkel vom vorherigen Aufruf
  static int svCnt = 0;       // Hilfszähler für 50 Pulse
  if(svCnt>0 || abs(wi-wiOld)>0){
    if(abs(wi-wiOld)>0) {
      svCnt=50;               // Servosignal 50*20ms = 1s senden
      wiOld=wi;
    }
  
    if(micros()>timer+puls){
      timer = micros();       // Wecker neu stellen
      if(puls>2500){          // der alte Pulswert für "Aus"
        digitalWrite(SVP, HIGH);
        puls = map(wi,0,180,500,2500);
      }
      else {                 // der alte Pulswert für "Ein"
        digitalWrite(SVP, LOW);
        puls = 20000-puls;
        svCnt--;
      }
    }
  }
}
