/*********************************************************************
* Drehzahlsteuerung mit PWM von 1 Gleichstrom-Motor         19.04.2023
* 3 HC-Sensoren auslesen
* 1 Servo ansteuern
* LCD-Anzeige
* 2 Tasten abfragen
**********************************************************************/

#include "DC-Motor.h"
#include "HC-Sensor.h"
#include "Servo.h"
#include "LCD-Anzeige.h"

void setup(){
  motSetup();                          // Motor-Ports einrichten
  hcSetup();                           // HC-Sensoren einrichten 
  servoSetup();                        // Servomotor einrichten
  lcdSetup();                          // LCD einrichten
}
  
void loop(){
  motLi();                             // PWM erzeugen (100us)
  motFahre(motorSoll);                 // Beschleunigen und Bremsen (1ms Schritte)
  messHcR();                           // ganze Messung bearbeiten
  messHcM();
  messHcL(); 
  messSerie();                         // Reihum messen  
  servo(lenkWinkel);                   // Servo stellen
  anzeigen();                          // LCD-Anzeigefunktionen
  tasten();                            // Tastenabfrage und Menue
  fahren();                            // Testfahrt
}

void fahren(){
  static long timer;
  if(millis()>timer+100){
    timer=millis(); 
    switch(robotMenue){
      case 0: {
        if(robotGo){}
        break;
      }
      case 1: {
        if(robotGo){}
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
