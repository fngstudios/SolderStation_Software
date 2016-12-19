#include <Arduino.h>

/*
 * LCD RS pin to digital pin 2
 * LCD Enable pin to digital pin 4
 * LCD D4 pin to digital pin 8
 * LCD D5 pin to digital pin 7
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 5
 * LCD Contraste digital pin 9 default to 28.
 * POTE A1
 * Boton Prog A2
 * Boton Idle A3
 * Base A0
 * Heater digital pin 3
 * CS digital pin 10
 * Mosi digital pin 11
 * Miso digital pin 12
 * Sck digital pin 13
 */


unsigned char CR0_INIT;
unsigned char CR1_INIT;
unsigned char MASK_INIT;

void Init_HW(){
pinMode(9,OUTPUT);
pinMode(A2,INPUT);
pinMode(A3,INPUT);
pinMode(3,OUTPUT);


  Themocouple = new MAX31856(11, 12, 10, 13);
  setTC_Type();

  myPID.SetOutputLimits(0, 255);
  myPID.SetSampleTime(500);
  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(Current_Config.KP, Current_Config.KI, Current_Config.KD);  
}

void setTC_Type(){
  CR0_INIT = (CR0_AUTOMATIC_CONVERSION + CR0_OPEN_CIRCUIT_FAULT_TYPE_K /* + CR0_NOISE_FILTER_50HZ */); 
  CR1_INIT = (CR1_AVERAGE_2_SAMPLES + Current_Config.TC_TYPE);
  MASK_INIT = (~(MASK_VOLTAGE_UNDER_OVER_FAULT + MASK_THERMOCOUPLE_OPEN_FAULT));

  Themocouple->writeRegister(REGISTER_CR0, CR0_INIT);
  Themocouple->writeRegister(REGISTER_CR1, CR1_INIT);
  Themocouple->writeRegister(REGISTER_MASK, MASK_INIT);
}

void readTC(){
     Temperature = Themocouple->readThermocouple(CELSIUS);
     Input = Temperature;
}

float readPote(){
  int pote = analogRead(A1);
  float val = 0;
  switch (Estado){
    case 0:         val = map(pote, 1024, 0, MAX_WORK_TEMP, MIN_WORK_TEMP);
                    break;
    case 1:         
                    break;
    case 2:         
                    switch(Menu_Page){
                        case 0:      val = map(pote,1024,0,MAX_IDLE_TEMP,MIN_IDLE_TEMP);  //Idle Temp
                                     break;
                        case 1:      val = map(pote,1024,0,MAX_CONTRAST,MIN_CONTRAST);  //Contraste
                                     break;
                        case 2:      val = map(pote,1024,0,7,0);  //TcType
                                     break;
                        case 3:      val = map(pote,1024,0,MAX_KP,MIN_KP);  //KP
                                     break;
                        case 4:      val = mapfloat(pote,1024,0,MAX_KI,MIN_KI);  //KI
                                     break;
                        case 5:      val = mapfloat(pote,1024,0,MAX_KD,MIN_KD);  //KD
                                     break;
                        case 6:      val = map(pote,1024,0,MAX_OFF_TIME,MIN_OFF_TIME);  //OFF TIME
                                     break;
                    }
                    break;
  }
  return val;
                   
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void updateOutput(){
  analogWrite(3,Output);
}
