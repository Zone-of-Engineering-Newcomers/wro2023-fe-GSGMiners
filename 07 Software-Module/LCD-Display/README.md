# LCD-Anzeige

<div align = "center">
<table>
  <thead>
    <tr>
      <th align = "center">Titel</th>
      <th align = "center">Autor</th>
      <th align = "center">Stand</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align = "center">LCD-Anzeige</td>
      <td align = "center">Finnian Belger</td>
      <td align = "center">12.05.2023</td>
    </tr>
  </tbody>
</table>
</div>

LCD ist die Abkürzung von Liquid Crystal Display. 
Eine LCD-Anzeige besteht grundsätzlich aus zwei Glasscheiben und einer speziellen Flüssigkeit dazwischen. Das Besondere an der Flüssigkeit ist nun, dass diese die Polarisationsebene von Licht dreht. Dieser Effekt wird durch Anlegen eines elektrischen Feldes beeinflusst.

## Table of Contents

- [1. Elektrische Anschlüsse]()
- [2. Funktionsweise eines LCD-Displays]()
- [3. Unser Code]()

---

## 1. Elektrische Anschlüsse

- Über den SDA-Anschluss (Serieller Datenanschluss) werden immer Daten verschickt und der SCL-Anschluss entscheidet ob diese Daten gültig sind.

<div align = "center">
  <img src = "Elektrische Anschluesse.png" alt = "Anschlüsse des LCD-Displays" width = "auto" height = "200">
<p><i>Anschlüsse des LCD-Displays</i></p>
</div>

### Anschlüsse am RoboCar

- Der GND wird am Steckbrett auf der negativen Leiste angeschlossen und das Kabel ist schwarz.
- Der VCC wird am Steckbrett auf der positiven Leiste angeschlossen und das Kabel ist rot.
- Der SDA geht an A4 und das Kabel ist braun.
- Der SCL geht an A5 und das Kabel ist weiß.

## 2. Funktionsweise eine LCD-Displays

- Eine einfache LCD-Zelle (Twisted Nematic) besteht zunächst aus zwei um 90° zueinander verdrehten linearen Polarisationsfilter.
- Zwischen den Filtern befinden sich zwei durchsichtige Elektroden (bspw. Glasplatten mit sehr dünner Metallbeschichtung).
- Zwischen den beiden Elektroden befinden sind nun die längliche Flüssigkristalle. Die Flüssigkristalle ordnen sich hier entlang der beiden Orientierungsschichten an, die auf die Elektroden aufgebracht ist.
- Diese Orientierungsschichten sind wie die Polfilter um 90° zueinander verdreht. Daher ordnen sich die Flüssigkristalle auch verdreht.
- Nun fällt unpolarisiertes Licht der Hintergrundbeleuchtung (meist von LEDs) auf den ersten Polfilter. Das Licht wird linear polarisiert. 
- Anschließend passiert es die durchsichtige Elektrode und trifft auf die Flüssigkristalle. Diese sorgen durch ihre verdrehte Anordnung dafür, dass auch die Polarisationsebene des sie passierenden Lichtes um 90° gedreht wird, da die Polarisationsebene der Ausrichtung der Moleküle folgt.
- Somit kann das Licht nun auch den zweiten Polfilter passieren, da seine Polarisationsebene gerade der Durchlassrichtung dieses Filters entspricht. In diesem Zustand ist die LCD-Zelle lichtdurchlässig.
- Legt man jedoch an die beiden Elektroden eine Spannung an, so entsteht zwischen den beiden Elektroden ein elektrisches Feld.
- Dieses Feld sorgt dafür, dass sich die Flüssigkristalle nicht mehr entlang der Orientierungsschicht anordnen, sondern entlang der elektrischen Feldlinien.
- Trifft nun linear polarisiertes Licht auf die Flüssigkristalle, so wird die Polarisationsebene des Lichtes nicht verdreht.
- Das Licht passiert die Flüssigkristalle unverändert, trifft auf den zweiten Polfilter und wird von diesem absorbiert.
- Die Zelle lässt also bei angelegter Spannung kein Licht durch.
- Zwischen der durchlässigen Zelle bei keiner angelegten Spannung und der komplett lichtundurchlässigen Zelle bei angelegter Spannung  kann der Lichtdurchgang quasi stufenlos über die Höhe der angelegten Spannung geregelt werden.
- Je kleiner die Spannung wird, desto mehr Licht kann die Zelle passieren.

## 3. Unser Code

Wir haben am Anfang erst mal die LiquidCrystal_I2C Bibiolotek hinzugefügt, 2 Tasten definiert und den Bild und das Display als `lcd` definiert.

```C++
#include <LiquidCrystal_I2C.h>
#define TAL 16  
#define TAR 17 
LiquidCrystal_I2C lcd(0x27,16,2);
```

Dann das Ein-/Ausschalten des Lcd, das Starten/ Stoppen des Roboter und die 3 Modi.

```C++
boolean lcdOn = 1;  
boolean robotGo = 0;                   
int robotMenue =  0;
```

Nachdem haben wir das `void lcdSetup` gemacht, wo wir die Knöpfe und das Hintergrundlicht definiert haben. 

```C++
void lcdSetup(){
  pinMode(TAL, INPUT_PULLUP);
  pinMode(TAR, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
```

Außerdem haben wir programmiert, wann der Hintergrund leuchten soll.

```C++
void lcdAnzeige(boolean on){           
  lcdOn = on;
  if(on) lcd.backlight(); else lcd.noBacklight();
}
```

Danach haben wir die Texte, die angezeigt werden sollen hinzugefügt.

```C++
void anzeigen(){
  static long timer;
  if(millis()>timer+200){
    timer=millis();
    if(lcdOn){
      lcd.setCursor(0,0); lcd.print("Menue"+String(robotMenue));
      lcd.setCursor(7,0); lcd.print("W"+String(lenkWinkel)+" ");
      lcd.setCursor(12,0); lcd.print("S"+String(motorSoll)+" ");
      lcd.setCursor(1,1); lcd.print("L"+String(readHcL)+"  ");
      lcd.setCursor(6,1); lcd.print("M"+String(readHcM)+"  ");
      lcd.setCursor(11,1); lcd.print("R"+String(readHcR)+"  ");
    }
  }  
}
```

Danach haben wir programmiert, was bei den 3 verschiedenen Modi auf dem Bildschirm passiert. Außerdem haben wir programmiert, dass das Display aussgeht, wenn das Robocar fährt, da in der Bibliothek auch ein Delay vorhanden ist, was die HC-Sensoren, DC-Motor und den Servo behindert.

```C++
void tasten(){
  static long timer;
  static boolean tLOld=1, tROld=1;
  if(millis()>timer+100){
    timer = millis();
    if(digitalRead(TAL)<tLOld) {        
      tLOld=0;
      robotMenue++;
      if(robotMenue>3) robotMenue=0; 
    }
    if(digitalRead(TAL)>tLOld) tLOld=1;     
    if(digitalRead(TAR)<tROld) {        
    tROld=0;
      robotGo=!robotGo;
      if(robotGo) lcdAnzeige(0); else {
        lcdAnzeige(1);
        lenkWinkel=90; speedL=0; motorSoll=0; 
      }
    }
    if(digitalRead(TAR)>tROld) tROld=1;   
  }
}  
```
