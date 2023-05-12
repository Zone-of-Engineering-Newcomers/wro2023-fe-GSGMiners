/************************************************************
* LCD-Anzeige und Tasten Funktionen               12.05.2023
*   LCD-Bibliothek - Achtung behindert HC, DC, Servo! 
*   -> während der Fahrt LCD-Senden abschalten
* Einbinden über:
*   #include "LCD-Anzeige"
* im void setup() ergänzen:
*   lcdSetup();
* im void loop ergänzen:  
*   anzeigen();  // Inhalte darstellen
*   tasten();
*************************************************************/

#define TAL 16  // Anschluss A2! Taste Links
#define TAR 17  // Anschluss A3! Taste Rechts

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);      // I2C-Display-Modul Objekt "lcd"

boolean lcdOn = 1;                     // LCD aktiviert (1) und deaktiviert (0)

// globale Variable:
boolean robotGo = 0;                   // Robot starten (1) und stoppen (0)
int robotMenue = 0;                    // Robot Menue (0..3)

void lcdSetup(){
  pinMode(TAL, INPUT_PULLUP);
  pinMode(TAR, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();                     
}

void lcdAnzeige(boolean on){           // 0 - aus, 1 - an
  lcdOn = on;
  if(on) lcd.backlight(); else lcd.noBacklight();
}

void anzeigen(int farbe, int blocks){
  static long timer;
  if(millis()>timer+200){
    timer=millis();
    if(lcdOn){
      lcd.setCursor(0,0); lcd.print("Menue"+String(robotMenue));
      lcd.setCursor(7,0); lcd.print("B"+String(blocks)+" ");
      lcd.setCursor(12,0); lcd.print("F"+String(farbe)+" ");
      lcd.setCursor(1,1); lcd.print("L"+String(readHcL)+"  ");
      lcd.setCursor(6,1); lcd.print("M"+String(readHcM)+"  ");
      lcd.setCursor(11,1); lcd.print("R"+String(readHcR)+"  ");
    }
  }  
}

// Linke Taste steuert das Menue 0..3
// Rechte Taste toggelt Start und Stop
void tasten(){
  static long timer;
  static boolean tLOld=1, tROld=1;
  if(millis()>timer+100){
    timer = millis();

    // linke Taste (Menue)
    if(digitalRead(TAL)<tLOld) {        // Taste gedrückt: HL-Flanke
      tLOld=0;
      robotMenue++;
      if(robotMenue>3) robotMenue=0; 
    }
    if(digitalRead(TAL)>tLOld) tLOld=1; // Taste losgelassen: LH-Flanke

    // rechte Taste (Start/Stop)
    if(digitalRead(TAR)<tROld) {        // Taste gedrückt: HL-Flanke
      tROld=0;
      robotGo=!robotGo;
      if(robotGo) lcdAnzeige(0); else {
        lcdAnzeige(1);
        lenkWinkel=90; speedL=0; motorSoll=0; 
      }
    }
    if(digitalRead(TAR)>tROld) tROld=1; // Taste losgelassen: LH-Flanke
  }
}
