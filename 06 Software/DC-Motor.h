/*********************************************************************
 * Routinen zur Ansteuerung eines Gleichstrommotors     Stand 31.3.2023
 *   Antwortvariable int motMess; für die Stromauswertung in mA
 *   Antwortvariable boolean motFertig; wenn Beschl. abgeschlossen = true
 * Einbinden der Routine über: 
 *   #include "DC-Motor.h"
 * Eintrag der Initialisierung in das Setup:  
 *   motSetup();
 * Eintrag in die loop-Schleife:   
 *   motLi(); Ist-Wertebereich -255..0..255
 * Beschleunigen und Bremsen: Eintrag in loop-Schleife  
 *   motFahre(motorSoll); Soll-Wertebereich -255..0..255
 ********************************************************************/

#define MLV 2         // Motor L vorwärts / rückwärts
#define MLR 3
#define MM A0         // Motor Strommessung

const int pwmFq=100;  // PWM-Frequenz des Motors festlegen
                      // 100us*256 Stufen = 25,6 ms pro Periode
const int beschl=10;   // Beschleunigungs- und Bremskonstante in ms

// globale Variable:
int motorSoll = 0;    // Soll-Geschwindigkeit -255..0..+255
int speedL = 0;       // Ist-Geschwindigkeit Motor -255..0..255
int motMess;          // Messung am Motor, bei 5 Ohm entsprechen
                      // die Messwerte etwa einem Strom in mA
boolean motFertig = true;  // Beschleunigung abgeschlossen

void motSetup(){
  pinMode(MLV,OUTPUT);  pinMode(MLR,OUTPUT);
}

// Beschleunigen und Bremsen vor und rück
void motFahre(int sollL){  // Sollwerte -255..0..+255
  static long timer;   // Wecker
  if(millis()>timer+beschl){
    timer=millis();  // Wecker neu stellen
    if(sollL>speedL) speedL++;
    if(sollL<speedL) speedL--;
    if(sollL==speedL) motFertig=true; else motFertig=false; 
  }
}

// PWM Motor
void motLi(){
  static long timer;        // Wecker für Mikrosekunden
  static byte pwmTime;      // Pulsbreite 0..255
  static boolean pwmPhase;  // PWM Puls Ein oder Aus
  boolean ri;               // Drehrichtung 
  int sp = speedL;          // globale Variable speedL auslesen
  if(micros()>timer+pwmTime*pwmFq){
    timer=micros();         // Wecker neu stellen
    if(sp<0) ri=1; else ri=0; // Vorzeichen in Richtung wandeln
    sp=abs(sp);               // Vorzeichen löschen
    if (pwmPhase){            // PWM-Puls "Aus" - Ports abschalten
      motMess = analogRead(MM);
      digitalWrite(MLV,LOW); digitalWrite(MLR,LOW);
      pwmPhase=false;         // Phasenstatus "Aus" merken
      pwmTime=255-sp;         // Wecker für Aus-Zeit stellen
    } else {                  // PWM-Puls "Ein" - Ports einschalten
      if(ri && sp>0) digitalWrite(MLV,HIGH); 
      if(!ri && sp>0) digitalWrite(MLR,HIGH);
      pwmPhase=true;          // Phasenstatus "Ein" merken
      pwmTime=sp;             // Wecker für Ein-Zeit stellen
    }
  }
}
