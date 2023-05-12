/***************************************************************  
*  Pixy-Cam-Farberkennung Rot-Grün              Stand 12.05.2023
*    Berechnung der größten Farbfläche aus mehreren Boxen, damit Erkennung
*    des näheren Objektes
*    dynamisches Einschalten der LEDs um Strom zu sparen
***************************************************************/

#include <Pixy2.h>

Pixy2 pixy;

int pixyFarbe;        // 0 keine erkannt, 1 Rot erkannt, 2 Grün erkannt
int pixyBlocks;
boolean pixyOn=false;

void pixySetup(){
  pixy.init();
}

void pixyRead(){
  int f1, f2; 
  static long timer;
  if (pixyOn){
    if(millis()>timer+400) pixy.setLamp(1,0);
    if(millis()>timer+500){
      timer=millis();
      pixy.ccc.getBlocks();
      pixy.setLamp(0,0);
      pixyBlocks=pixy.ccc.numBlocks;
      if (pixy.ccc.numBlocks) {
        f1=0; f2=0;
        for (int i=0; i<pixy.ccc.numBlocks; i++)
        {
          if (pixy.ccc.blocks[i].m_signature==1) f1=f1+pixy.ccc.blocks[i].m_width*pixy.ccc.blocks[i].m_height;
          if (pixy.ccc.blocks[i].m_signature==2) f2=f2+pixy.ccc.blocks[i].m_width*pixy.ccc.blocks[i].m_height;
        }
        if (f1>f2) pixyFarbe=1;
        if (f2>f1) pixyFarbe=2;
      } else pixyFarbe=0;
    }
  }
}
