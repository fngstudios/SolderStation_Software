#include <Arduino.h>

byte customChar[8] = {0b01100,0b10010,0b10010,0b01100,0b00000,0b00000,0b00000,0b00000};



void LCD_Init(){
lcd.begin(16, 2);  
lcd.createChar(1, customChar); 
setContraste();
  
}

void setContraste(){
  analogWrite(9,(100-Current_Config.CONTRASTE));  
}

void lcdPrintT(){
  lcd.setCursor(0, 1);
   lcd.print(Temperature);
   lcd.write(1);
}

void refreshLCD(){
  
  switch(Estado){
    case 0:      lcd.setCursor(0,0);
                 lcd.print("WORKING   O:");
                 temp = String(showOutput());
                 if (temp.length() == 1){temp = "00"+temp;}else if(temp.length() == 2){temp = "0"+temp;}
                 lcd.print(temp);
                 lcd.print('%');
                 lcd.setCursor(0,1);
                 if (Temperature > 9000){
                 lcd.print("TC ERROR");
                 }else{
                 lcd.print("T:");
                 temp = String(int(Temperature));
                 if (temp.length() == 1){temp = "00"+temp;}else if(temp.length() == 2){temp = "0"+temp;}
                 lcd.print(temp);
                 lcd.write(1);
                 lcd.print(" >> S:");
                 temp = String(int(setPoint));
                 if (temp.length() == 1){temp = "00"+temp;}else if(temp.length() == 2){temp = "0"+temp;}
                 lcd.print(temp);
                 lcd.write(1);
                 }                 
                 break;

    case 1:      lcd.setCursor(0,0);
                 lcd.print("IDLE      O:");
                 temp = String(showOutput());
                 if (temp.length() == 1){temp = "00"+temp;}else if(temp.length() == 2){temp = "0"+temp;}
                 lcd.print(temp);
                 lcd.print('%');
                 lcd.setCursor(0,1);
                 if (Temperature > 9000){
                 lcd.print("TC ERROR");
                 }else{  
                 lcd.print("T:");
                 temp = String(int(Temperature));
                 if (temp.length() == 1){temp = "00"+temp;}else if(temp.length() == 2){temp = "0"+temp;}
                 lcd.print(temp);
                 lcd.write(1);
                 lcd.print(" >> S:");
                 temp = String(int(setPoint));
                 if (temp.length() == 1){temp = "00"+temp;}else if(temp.length() == 2){temp = "0"+temp;}
                 lcd.print(temp);
                 lcd.write(1);
                 }             
                 break;
                 
     case 2:     switch(Menu_Page){
                   case 0:      lcd.setCursor(0,0);        //IDLE
                                lcd.print("IDLE TEMP");
                                lcd.setCursor(0,1);
                                lcd.print("S:");
                                temp = String(int(Current_Config.IDLE_T));
                                if (temp.length() == 1){temp = "00"+temp;}else if(temp.length() == 2){temp = "0"+temp;}
                                lcd.print(temp);
                                lcd.write(1);            
                                break;

                   case 1:      lcd.setCursor(0,0);        //CONTRASTE
                                lcd.print("CONTRASTE");
                                lcd.setCursor(0,1);
                                lcd.print("C:");
                                temp = String(int(Current_Config.CONTRASTE));
                                if (temp.length() == 1){temp = "00"+temp;}else if(temp.length() == 2){temp = "0"+temp;}
                                lcd.print(temp);
                                lcd.print("%");            
                                break;

                   case 2:      lcd.setCursor(0,0);        //TCtype
                                lcd.print("TC TYPE");
                                lcd.setCursor(0,1);
                                lcd.print("TIPO: ");
                                lcd.print(TC_Desc[Current_Config.TC_TYPE]);          
                                break;

                   case 3:      lcd.setCursor(0,0);        //KP
                                lcd.print("KP");
                                lcd.setCursor(0,1);
                                lcd.print(Current_Config.KP);          
                                break;
                                
                   case 4:      lcd.setCursor(0,0);        //KI
                                lcd.print("KI");
                                lcd.setCursor(0,1);
                                lcd.print(Current_Config.KI);    
                                break;
                                
                   case 5:      lcd.setCursor(0,0);        //KD
                                lcd.print("KD");
                                lcd.setCursor(0,1);
                                lcd.print(Current_Config.KD);          
                                break; 
 
                    case 6:      lcd.setCursor(0,0);        //OFF TIME
                                lcd.print("OFF TIME");
                                lcd.setCursor(0,1);
                                lcd.print(Current_Config.OFF_TIME);
                                lcd.print(" Min");          
                                break;                               
                 }
                 break;
                 
     case 3:     lcd.clear();
                 lcd.setCursor(0,0);
                 lcd.print("    STAND BY");
                 break;
  }
}

int showOutput(){
  return (float(Output/255))*100;
}
