/*********************************************************************
* Drehzahlsteuerung mit PWM von 1 Gleichstrom-Motor         12.05.2023
* 3 HC-Sensoren auslesen
* 1 Servo ansteuern
* LCD-Anzeige
* 2 Tasten abfragen
* Kamera abfragen
* 
* globale Variable:
*   motorSoll    -255..0..255 
*   lenkWinkel    40..90..120 (links 40, rechts 110)
*   readHcL, readHcM, raedHcR
*   pixyFarbe     0, 1-rot-rechts umfahren, 2-grün-links umfahren
**********************************************************************/

#include "DC-Motor.h"
#include "HC-Sensor.h"
#include "Servo.h"
#include "LCD-Anzeige.h"
#include "Kamera.h"

void setup(){
  motSetup();                          // Motor-Ports einrichten
  hcSetup();                           // HC-Sensoren einrichten 
  servoSetup();                        // Servomotor einrichten
  lcdSetup();                          // LCD einrichten
  pixySetup();                         // Kamera einrichten
}
  
void loop(){
  motLi();                             // PWM erzeugen (100us)
  motFahre(motorSoll);                 // Beschleunigen und Bremsen (1ms Schritte)
  messHcR();                           // ganze Messung bearbeiten
  messHcM();
  messHcL(); 
  messSerie();                         // Reihum messen  
  servo(lenkWinkel);                   // Servo stellen
  anzeigen(pixyFarbe, pixyBlocks);     // LCD-Anzeigefunktionen
  tasten();                            // Tastenabfrage und Menue
  pixyRead();                          // Kamera auslesen
  fahren();                            // Testfahrt
}

void fahren(){
  static long timer;
  if(millis()>timer+100){
    timer=millis();
    pixyOn=true;
    switch(robotMenue){
      case 0: {
        pixyOn=false;
        if(robotGo){fahrRegler0();}
        break;
      }
      case 1: {
        if(robotGo){fahrRegler1();}
        break;
      }
      case 2: {
        if(robotGo){}
        break;
      }
      case 3: {
        if(robotGo){}
        break;
      }
    }
  }
}

void fahrRegler0(){   // links herum
  if (readHcM<45 || readHcL>30) lenkWinkel=40; else lenkWinkel=90;   // links Abbiegen
  if (readHcL<15) lenkWinkel=105;                                    // Gegenlenken
  if (readHcM<20 || readHcL<10) motorSoll=0; else motorSoll=100;     // Notstop oder freie Fahrt
}

void fahrRegler1(){   // mit Hindernissen links herum
  lenkWinkel=90;
  if (pixyFarbe==1 && readHcM<35) lenkWinkel=110;
  if (pixyFarbe==2 && readHcM<35) lenkWinkel=55; 
  if (pixyFarbe==0 && readHcL>30) lenkWinkel=40;   
  if (readHcL<15) lenkWinkel=105;                                    // Gegenlenken
  if (readHcR<15) lenkWinkel=65;                                     // Gegenlenken
  if (readHcL<10 || readHcM<20) motorSoll=0; else motorSoll=100;                   // Notstop oder freie Fahrt
}
