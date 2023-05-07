/***************************************************************************
* Ultraschall-Sensor HC-SR04                                 Stand 18.04.2023
*    Polling für HC-Sensor-Steuerung; der ganze Messprozess
*    beide Ports je Sensor müssen initialisiert sein
*    benötigt globale Variable startHC und readHC
*    startHC LH-Flanke startet Messzyklus
*    startHC HL-Flanke meldet Messung fertig
*    Antwortvariable readHcL, readHcM, readHcR mit Entfernungen Links, Mitte, Rechts   
* Einbinden der Routine über    
*   #include "HC-Sensor.h"
* Eintrag der Initialisierung im Setup:
*   hcSetup();
* Eintrag der 3 Objekte in der loop-Schleife:
*   messHcL();
*   messHcM();
*   messHcR();
* Zusätzlich die Reihum-Messung in loop eintragen:
*   messSerie();
****************************************************************************/

#define TRGL 4  // HC-SR04 Links Trig
#define ECHL 5  // HC-SR04 Links Echo
#define TRGM 6  // HC-SR04 Mitte Trig
#define ECHM 7  // HC-SR04 Mitte Echo
#define TRGR 8  // HC-SR04 Rechts Trig
#define ECHR 9  // HC-SR04 Rechts Echo

boolean startHcR = false; // set true: startet Messung, read false: Messung fertig 
boolean startHcM = false; // set true: startet Messung, read false: Messung fertig 
boolean startHcL = false; // set true: startet Messung, read false: Messung fertig 

// globale Variable:
int readHcR;              // Abstand auslesen in cm
int readHcM;              // Abstand auslesen in cm
int readHcL;              // Abstand auslesen in cm

void hcSetup(){
  pinMode(TRGR, OUTPUT); pinMode(ECHR, INPUT);
  pinMode(TRGM, OUTPUT); pinMode(ECHM, INPUT);
  pinMode(TRGL, OUTPUT); pinMode(ECHL, INPUT);
}

void messSerie(){
  static long timer;        // Wecker
  static int zaehler=0;     // Sensor-Zähler
  if(millis()>timer+50){    // alle 50ms ein anderer Sensor
    timer=millis();         // Wecker neu stellen
    switch(zaehler){
      case 0: {if(startHcL==false) startHcL=true; break;}
      case 1: {if(startHcM==false) startHcM=true; break;}
      case 2: {if(startHcR==false) startHcR=true; break;}
    }
    zaehler++; if(zaehler>2) zaehler=0;
  }
}

void messHcR(){
  const int triggerPin=TRGR, echoPin=ECHR;
  static long timer;                // Wecker
  static int phase;                 // 3 Phasen für Messwertermittlung
  static boolean startOld = false;   // Hilfsvariable für Startvorgang
  static boolean messOld = false;    // Hilfsvariable für Echostart
  static int messTab[8];             // Array für Gleitenden Durchschnitt
  if(startHcR>startOld){             // Start der Messung erkennen: LH-Flanke
    timer=micros();                  // Zeit für Triggerpuls merken 
    digitalWrite(triggerPin, HIGH);  // Trigger einschalten
    startOld = startHcR;             // Messprozess nun aktiv, Startpegel merken
    phase = 0;                       // erste Phase: Trigger
  }
  switch (phase){
    case 0: {
      if(micros()>timer+10){         // Trigger-Ende 10us abwarten
        phase=1; digitalWrite(triggerPin, LOW);  // Trigger abschalten und
      }                              // Phase 2 einleiten: Warte auf Echo
      break;
    }
    case 1: {
      if(digitalRead(echoPin)>messOld){  // Echostart abwarten: LH-Flanke
        phase=2; timer=micros();      // Zeit für Echo-Beginn merken
        messOld=HIGH;                 // Echopegel merken
      }                               // Phase 3 abwarten: Echo-Ende
      break;
    }
    case 2: {
      if(digitalRead(echoPin)<messOld){  // Echoende abwarten
        phase=3; messOld = LOW;        // Echo HL-Flanke erkannt, Phase inaktiv
        long tHelp = (micros()-timer)/2;
        /*if(tHelp<=10000){
          int messWert = 0;
          for(int i=7; i>0; i--) messTab[i] = messTab[i-1]; // MW schieben
          messTab[0] = tHelp/29.1;    // Messwert berechnen (Mikrosek -> ms)
          if(tHelp>10000) messTab[0] = 0;    // 9000 = ca. 3m
          for(int i=7; i>=0; i--) messWert = messWert + messTab[i];
          readHcR = messWert/8;        // Gleitender Durchschnitt
        } */
        if(tHelp<9000) readHcR=tHelp/29.1; //else readHcR=0;
        startHcR = false;              // Messung fertig signalisieren
        startOld = startHcR;           // Startpegel merken für Neustart
      }
      break;
    }
  }
}

void messHcM(){
  const int triggerPin=TRGM, echoPin=ECHM;
  static long timer;
  static int phase;
  static boolean startOld = false;
  static boolean messOld = false;
  static int messTab[8];             // Array für Gleitenden Durchschnitt
  if(startHcM>startOld){             // 1. Start der Messung erkennen: LH-Flanke
    timer=micros();                  // Zeit für Triggerpuls merken 
    digitalWrite(triggerPin, HIGH);  // Trigger starten
    startOld = startHcM;             // Messprozess nun aktiv, Startpegel merken
    phase = 0;
  }
  switch (phase){
    case 0: {
      if(micros()>timer+10){         // 2. Trigger-Ende 10us abwarten
        phase=1; digitalWrite(triggerPin, LOW);  // Trigger abschalten
      }
      break;
    }
    case 1: {
      if(digitalRead(echoPin)>messOld){  // 3. Echostart abwarten LH-Flanke
        phase=2; timer=micros();      // Zeit für Echo-Beginn merken
        messOld=HIGH;                 // Echopegel merken
      }
      break;
    }
    case 2: {
      if(digitalRead(echoPin)<messOld){  // 4. Echoende abwarten
        phase=3; messOld = LOW;     // Echo HL-Flanke erkannt
        long tHelp = (micros()-timer)/2;
        /*if(tHelp<=10000){
          int messWert = 0;
          for(int i=7; i>0; i--) messTab[i] = messTab[i-1]; // MW schieben
          messTab[0] = tHelp/29.1;    // Messwert berechnen
          if(tHelp>10000) messTab[0] = 0;
          for(int i=7; i>=0; i--) messWert = messWert + messTab[i];
          readHcM = messWert/8;        // Gleitender Durchschnitt
        }*/
        if(tHelp<9000) readHcM=tHelp/29.1; //else readHcM=0;
        startHcM = false;              // Messung fertig signalisieren
        startOld = startHcM;           // Startpegel merken
      }
      break;
    }
  }
}

void messHcL(){
  const int triggerPin=TRGL, echoPin=ECHL;
  static long timer;
  static int phase;
  static boolean startOld = false;
  static boolean messOld = false;
  static int messTab[8];             // Array für Gleitenden Durchschnitt
  if(startHcL>startOld){              // 1. Start der Messung erkennen: LH-Flanke
    timer=micros();                  // Zeit für Triggerpuls merken 
    digitalWrite(triggerPin, HIGH);  // Trigger starten
    startOld = startHcL;              // Messprozess nun aktiv, Startpegel merken
    phase = 0;
  }
  switch (phase){
    case 0: {
      if(micros()>timer+10){         // 2. Trigger-Ende 10us abwarten
        phase=1; digitalWrite(triggerPin, LOW);  // Trigger abschalten
      }
      break;
    }
    case 1: {
      if(digitalRead(echoPin)>messOld){  // 3. Echostart abwarten LH-Flanke
        phase=2; timer=micros();      // Zeit für Echo-Beginn merken
        messOld=HIGH;                 // Echopegel merken
      }
      break;
    }
    case 2: {
      if(digitalRead(echoPin)<messOld){  // 4. Echoende abwarten
        phase=3; messOld = LOW;     // Echo HL-Flanke erkannt
        long tHelp = (micros()-timer)/2;
        /*if(tHelp<=10000){
          int messWert = 0;
          for(int i=7; i>0; i--) messTab[i] = messTab[i-1]; // MW schieben
          messTab[0] = tHelp/29.1;    // Messwert berechnen
          if(tHelp>10000) messTab[0] = 0;
          for(int i=7; i>=0; i--) messWert = messWert + messTab[i];
          readHcL = messWert/8;        // Gleitender Durchschnitt
        }*/
        if(tHelp<9000) readHcL=tHelp/29.1; //else readHcL=0;
        startHcL = false;              // Messung fertig signalisieren
        startOld = startHcL;           // Startpegel merken
      }
      break;
    }
  }
}
