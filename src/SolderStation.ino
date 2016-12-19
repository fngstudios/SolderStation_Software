#include <Arduino.h>

/*
    SolderStation v0.2
    fngStudios
    26/07/2016
    Ezequiel Pedace
    
    Change log:
    
    v0.2 25/08/2016
    -Detect TC errors
    -Auto Off timer, configurable.
    
    v0.1 26/07/2016
    First Version
    


 */
//-------------------------------------------------------------------------------------
//    Includes

#include <MAX31856.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Button.h>
#include <PID_v1.h>
//------------------------------------------------------------------------------------- 
//    Defines


#define LONG_PRESS         1000
#define DEBOUNCE_MS         25
#define DEFAULT_OFF_TIME    1  //Minutos
#define MAX_OFF_TIME        9
#define MIN_OFF_TIME        1
#define DEFAULT_TC_TYPE     3
#define DEFAULT_IDLE_TEMP   90
#define MAX_IDLE_TEMP       150
#define MIN_IDLE_TEMP       80
#define MAX_WORK_TEMP       300
#define MIN_WORK_TEMP       150
#define MAX_CONTRAST        100
#define MIN_CONTRAST        30
#define DEFAULT_CONTRAST    60
#define MAX_KP              100
#define MIN_KP              2
#define DEFAULT_KP          10
#define MAX_KI              0.1
#define MIN_KI              0.0001
#define DEFAULT_KI          0.001
#define MAX_KD              30
#define MIN_KD              0
#define DEFAULT_KD          5
#define VALID_CONFIG        'y'
#define CONFIG_EEPROM_ADDR  30
//------------------------------------------------------------------------------------- 
//    Variables de programa

char TC_Desc[9]="BEJKNRST";
unsigned int Work_Setpoint;
float Temperature;
double Output;
double Input;
double  setPoint;
unsigned char Estado = 0;  //0:Working 1:Idle 2:Menu 3:OFF
unsigned char Menu_Page = 0;
String temp;
long TimeStamp = 0;
//------------------------------------------------------------------------------------- 
//   Estructura de configuracion
  
 struct Configuration_T {
  unsigned char IDLE_T;
  unsigned char CONTRASTE;
  unsigned char TC_TYPE;
  unsigned char OFF_TIME;
  double KP;
  double KI;
  double KD;
  byte VALID; // Ultimo, para comprobaciones de integridad.
} Current_Config;

 
//------------------------------------------------------------------------------------- 
//    Constructores

LiquidCrystal lcd(2, 4, 8, 7, 6, 5);
MAX31856 *Themocouple;
Button btnProg(A2, false, true, DEBOUNCE_MS);
Button btnIdle(A3, false, true, DEBOUNCE_MS);
PID myPID(&Input, &Output, &setPoint, Current_Config.KP, Current_Config.KI, Current_Config.KD, DIRECT);
//-------------------------------------------------------------------------------------

void setup() {

    loadConfig();
    Init_HW();
    LCD_Init();
  

}



void loop() {
  checkBtns();
  readTC();  
  doWork();  
  refreshLCD();   
  myPID.Compute();
  updateOutput();
  delay(200);


}


void checkBtns(){
  
   btnProg.read();
   btnIdle.read();

   
}



void doWork(){
  
  switch(Estado){
    case 0:        setPoint = readPote();
                   if (btnIdle.wasPressed()){Estado = 1;SetOffTime();}
                   if (btnProg.pressedFor(LONG_PRESS)){Estado = 2;lcd.clear();while(btnProg.isPressed()){checkBtns();}}
                   break;
                   
    case 1:        setPoint = Current_Config.IDLE_T;
                   if (btnIdle.wasPressed()){Estado = 0;}
                   if (btnProg.pressedFor(LONG_PRESS)){Estado = 2;lcd.clear();while(btnProg.isPressed()){checkBtns();}}
                   if (checkOffTime()){Estado = 3;};
                   break;

    case 2:        
                   switch (Menu_Page){
                      case 0:         Current_Config.IDLE_T = readPote();   // prog idle setpoint
                                      if (btnIdle.wasPressed()){Menu_Page = 1;lcd.clear();}
                                      break;    
                      case 1:         Current_Config.CONTRASTE = readPote();   // prog contrast
                                      setContraste();
                                      if (btnIdle.wasPressed()){Menu_Page = 2;lcd.clear();}
                                      break;    
                      case 2:         Current_Config.TC_TYPE = readPote();   // prog TC Type
                                      if (btnIdle.wasPressed()){Menu_Page = 3;lcd.clear();}
                                      break;    
                      case 3:         Current_Config.KP = readPote();   // prog KP
                                      if (btnIdle.wasPressed()){Menu_Page = 4;lcd.clear();}
                                      break;    
                      case 4:         Current_Config.KI = readPote();   // prog KI
                                      if (btnIdle.wasPressed()){Menu_Page = 5;lcd.clear();}
                                      break;    
                      case 5:         Current_Config.KD = readPote();   // prog KD
                                      if (btnIdle.wasPressed()){Menu_Page = 6;lcd.clear();}
                                      break;    
                      case 6:         Current_Config.OFF_TIME = readPote();   // prog OFF_TIME
                                      if (btnIdle.wasPressed()){Menu_Page = 0;lcd.clear();}
                                      break;    

                   }
                   if (btnProg.pressedFor(LONG_PRESS)){saveConfig();Estado = 0;lcd.clear();while(btnProg.isPressed()){checkBtns();}}
                   break;

     case 3:       setPoint = 0;
                   if (btnIdle.wasPressed()){Estado = 1;SetOffTime();}
                   if (btnProg.wasPressed()){Estado = 1;SetOffTime();}
                   break; 
          
}
}




void loadConfig(){

  EEPROM.get(CONFIG_EEPROM_ADDR,Current_Config);
    if ( Current_Config.VALID != VALID_CONFIG){
      Current_Config.IDLE_T = DEFAULT_IDLE_TEMP;
      Current_Config.CONTRASTE = DEFAULT_CONTRAST;
      Current_Config.TC_TYPE = DEFAULT_TC_TYPE;
      Current_Config.OFF_TIME = DEFAULT_OFF_TIME;
      Current_Config.KP = DEFAULT_KP;
      Current_Config.KI = DEFAULT_KI;
      Current_Config.KD = DEFAULT_KD;
      Current_Config.VALID = VALID_CONFIG;
      EEPROM.put(CONFIG_EEPROM_ADDR,Current_Config);
    }
}

void saveConfig(){
      EEPROM.put(CONFIG_EEPROM_ADDR,Current_Config);
      myPID.SetTunings(Current_Config.KP, Current_Config.KI, Current_Config.KD);
      setTC_Type();  
}  

void SetOffTime(){
  TimeStamp = millis();
}

unsigned char checkOffTime(){
  if (TimeStamp + Current_Config.OFF_TIME * 60000 < millis()){
    return 1;
  }
  return 0;
}
