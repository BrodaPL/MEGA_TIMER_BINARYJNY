/*
 * Cyfrowe potencjometry:
 *    +Podstawowe oprogramowanie v2
 * Podłączenie podstawowe EEPROM'u
 *    +Podstawowa edycja pamięci
 * Buzzer
 *    +oprogramowanie v1
 * Czujnik światła
 *    +oprogramowanie v1
 * Zegar rtc
 *    +oprogramowanie v1
 * Pilot i dioda odbiorcza IR
 *    +oprogramowanie v1
 * Termometr
 *    +oprogramowanie v1
 * Kontrola PCF'ów
 *    +oprogramowanie v1
 * Budzik
 *    -brak oprogramowania
 */

#include <MyBuzzer.h>
#include <MyPCF8574Controller.h>
//#include <MyRemote.h>
//#include <MyTimer.h>
#include <MyMCP41.h>
#include <MyMCP41Controller.h>
#include <MyAlarmManager.h>

//#include <I2C_eeprom.h>
//#include <IRremoteInt.h>
#include <IRremote.h>
//#include <PCint.h>
#include <Rtc_Pcf8563.h>
#include <EEPROM.h>

#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define RTCC_R 0xa3
#define RTCC_W 0xa2
#define RTCC_CENTURY 21

#define HOUR_PCF_ADDRESS 0x20
#define MINUTE_PCF_ADDRESS 0x21
#define SECOND_PCF_ADDRESS 0x22

#define RECV_PIN 9

#define ONE_WIRE_BUS 8

#define BUZZER_PIN 7

#define HOUR_MCP41 4
#define MINUTE_MCP41 5
#define SECOND_MCP41 6

#define REMOTE_CHANNEL_MINUS 16753245
#define REMOTE_CHANNEL 16736925
#define REMOTE_CHANNEL_PLUS 16769565
#define REMOTE_PREVIOUS 16720605
#define REMOTE_NEXT 16712445
#define REMOTE_PLAY_PAUSE 16761405
#define REMOTE_MINUS 16769055
#define REMOTE_PLUS 16754775
#define REMOTE_EQUALIZER 16748655
#define REMOTE_NUM_0 16738455
#define REMOTE_NUM_100_PLUS 16750695
#define REMOTE_NUM_200_PLUS 16756815
#define REMOTE_NUM_1 16724175
#define REMOTE_NUM_2 16718055
#define REMOTE_NUM_3 16743045
#define REMOTE_NUM_4 16716015
#define REMOTE_NUM_5 16726215
#define REMOTE_NUM_6 16734885
#define REMOTE_NUM_7 16728765
#define REMOTE_NUM_8 16730805
#define REMOTE_NUM_9 16732845

#define STATE_SHOW_TIMER 1
#define STATE_SET_TIME 2
#define STATE_PLAY_ANIMATION 3
#define STATE_SHOW_TEMPERATURE 4
#define STATE_EDIT_MEMORY 5
#define STATE_SHOW_DATE 6
#define STATE_SET_DATE 7
#define STATE_SHOW_ALARMS 8
#define STATE_RUN_ALARM 9

#define MAX_TIMER_INDEX 2

#define PHOTO_SENSOR_PIN A0
int photoSensorValue = 0;

#define EEPROM_MAXBYTES 256
//#define EEPROM_ADDRESS 0x51
#define EEPROM_ADDRESS_JUMP 8

#define BIT0 B00000001
#define BIT1 B00000010
#define BIT2 B00000100
#define BIT3 B00001000
#define BIT4 B00010000
#define BIT5 B00100000
#define BIT6 B01000000
#define BIT7 B10000000

#define ALARMS_AMOUNT 8
#define ALARM_BYTE_SIZE 8
#define ALARM_FIRST_ADDRESS 0

Rtc_Pcf8563 rtc;

IRrecv irrecv(RECV_PIN);
decode_results results;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

MyBuzzer buzzer(BUZZER_PIN);

MyMCP41 mpc41Tab[3] = {
  MyMCP41(HOUR_MCP41, 255, 128), //96
  MyMCP41(MINUTE_MCP41, 255, 120),//96
  MyMCP41(SECOND_MCP41, 255, 104) //128
};
MyMCP41Controller mcp41controll(mpc41Tab);

byte state= STATE_SHOW_TIMER;

boolean blinkerFlag = true;

int refresh_interval=1000;
unsigned long previousMillis=0;
unsigned long previousSoundMillis=0;
unsigned long currentSoundMillis=0;

//podłączamy analog 4 - A4 do SDA (15 nóżka)
//podłączamy analog 5 - A5 do SCL (14 nóżka)
//oba urządzenia można podłączyć razem
 

byte pcfAddress[3] = {HOUR_PCF_ADDRESS, MINUTE_PCF_ADDRESS, SECOND_PCF_ADDRESS};

MyPCF8574Controller pcfController(pcfAddress);

byte timerIndex =0; //0- hour, 1- minute, 2- second

//byte anim1=0, anim2=0, anim3=0, animationStep=0;
byte animationStep = 0;
int animationSpeed = 0;
int buzzerInterval = 0;


//I2C_eeprom eeprom(EEPROM_ADDRESS, EEPROM_MAXBYTES);
byte eepromLoadedByte=0;
byte eepromWorkingByte=0;
int eepromByteAddress=0;

//byte testTab[4][2];
//byte **testTab;
//byte alarmTab[ALARMS_AMOUNT][2];
//byte **alarmList;
byte testTab[8]={1,2,4,8,16,32,64,128};

MyAlarmManager alarmManager(ALARM_FIRST_ADDRESS);

byte activeAlarmIndex =0;


byte oldMinut=0;
byte oldHour=0;
byte oldDay=0;





//////////////////////
/////   SETUP    /////
//////////////////////

void setup() {
//alarmManager.updateAlarms(1,2,3,4,5,6);
  //fillArray(alarmTab,8,2,0);
  makeSerial(9600);
  //for(int i=0;i<8;i++) Serial.println(testTab[i]);
  
//  delay(100);
//  eeprom.begin();
//  delay(100);

  mcp41controll.initialize();
  SPI.begin();  


  delay(100);
  pcfController.initialize();
  
  delay(1000); // po to aby moduł zegara RTC pmiał czas na zainicjowanie się.
  rtc.initClock();
  delay(500);
  rtc.setTime(2, 48, 30);//h m s
  delay(100);
  rtc.setDate(1, 3, 6, 21, 17); //day, weekday, month, cenury, year
  delay(100);
  
  irrecv.enableIRIn();

  sensors.begin();
  delay(100);

  buzzer.initialize();
  //buzzer.playShowTimerSound();
  buzzer.mute();

  

oldMinut= rtc.getMinute();
oldHour = rtc.getHour();
oldDay  = rtc.getDay();


}









//////////////////////
/////    LOOP    /////
//////////////////////

void loop() {

  unsigned long currentMillis = millis();
  currentSoundMillis = millis();



  //////////////////////////////////////////
  /////////////  REMOTE INPUT  /////////////
  if (irrecv.decode(&results)) {
      //Serial.println(results.value, HEX);
      //Serial.println(results.value);
      /*if(results.value == 16720605)
      {
        showBinaryNumber(&pcf8574_H, 1);
      }*/
      switch(results.value)
      {
        case REMOTE_PREVIOUS:
          if( state==STATE_SET_TIME || state==STATE_SET_DATE ){
            //decreaseTimerIndex();
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(750,30);
            timerIndex = changeTimerIndex(timerIndex, 1);
          }else if(state==STATE_EDIT_MEMORY){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(750,30);
            eepromByteAddress = changeEepromByteAddress(-1);
            eepromReadProcedure();
          }
          if(state = STATE_SHOW_ALARMS){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(750,30);
            activeAlarmIndex = changeActiveAlarmIndex(activeAlarmIndex, -1);
          }
          break;
          
        case REMOTE_NEXT:
          if( state==STATE_SET_TIME || state==STATE_SET_DATE ){
            //increaseTimerIndex();
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(1000,30);
            timerIndex = changeTimerIndex(timerIndex, -1);
          }else if(state==STATE_EDIT_MEMORY){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(1000,30);
            eepromByteAddress = changeEepromByteAddress(1);
            eepromReadProcedure();
          }
          if(state = STATE_SHOW_ALARMS){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(1000,30);
            activeAlarmIndex = changeActiveAlarmIndex(activeAlarmIndex, 1);
          }
          break;
          
        case REMOTE_PLAY_PAUSE:
          if( state==STATE_SHOW_TIMER ){
            state = STATE_PLAY_ANIMATION;
            buzzer.unmute();
            if(!buzzer.isMuted()) buzzer.playInitializeAnimationenSound();
            initializeAnimation();  // BYYYYRZYDYYYKOOOO!
            pcfController.initializeAnimationen();
          }else if(state==STATE_PLAY_ANIMATION){
            if(!buzzer.isMuted()) buzzer.playShowTimerSound();
            state = STATE_SHOW_TIMER;
          }else if(state==STATE_EDIT_MEMORY){
            if(!buzzer.isMuted()) buzzer.playWriteMemorySound();
            
            EEPROM.write(eepromByteAddress, eepromWorkingByte);
            //eepromWriteProcedure(eepromByteAddress,eepromWorkingByte);

            delay(10);
            eepromReadProcedure();
          }
          break;
          
        case REMOTE_MINUS:
          if( state==STATE_SET_TIME ){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(350,40);
            changeTimeByIndex(timerIndex, -1);
            //minuteMCP-=10;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(350,40);
            changeDateByIndex(timerIndex, -1);
          }
          break;
          
        case REMOTE_PLUS:
          if( state==STATE_SET_TIME ){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(600,40);
            changeTimeByIndex(timerIndex, 1);
            //minuteMCP+=10;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(600,40);
            changeDateByIndex(timerIndex, 1);
          }
          break;
          
        case REMOTE_EQUALIZER:
          if( state==STATE_SET_TIME ){
            if(!buzzer.isMuted()) buzzer.playShowTimerSound();
            state = STATE_SHOW_TIMER;
          }else if(state==STATE_SHOW_TIMER){
            if(!buzzer.isMuted()) buzzer.playSetTimeSound();
            state = STATE_SET_TIME;
          }
          break;
          
        case REMOTE_CHANNEL_MINUS:
          if( state==STATE_SHOW_TEMPERATURE ){
            if(!buzzer.isMuted()) buzzer.playShowTimerSound();
            state = STATE_SHOW_TIMER;
          }else if(state==STATE_SHOW_TIMER){
            if(!buzzer.isMuted()) buzzer.playShowTemperatureSound();
            state = STATE_SHOW_TEMPERATURE;
          }else if(state==STATE_EDIT_MEMORY){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(350,40);
            eepromByteAddress = changeEepromByteAddress(-EEPROM_ADDRESS_JUMP);
            eepromReadProcedure();
          }
          break;
          
        case REMOTE_CHANNEL_PLUS:
          if( state==STATE_SHOW_TIMER){
            buzzer.reverseMute();
            if(!buzzer.isMuted()){
              buzzer.playUnmuteSound();
            }else{
              buzzer.playMuteSound();
            }
          }else if(state==STATE_EDIT_MEMORY){
            if(!buzzer.isMuted()) buzzer.playBuzzerBip(600,40);
            eepromByteAddress = changeEepromByteAddress(EEPROM_ADDRESS_JUMP);
            eepromReadProcedure();
          }
          break;

        case REMOTE_CHANNEL:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playShowTimerSound();
            state = STATE_SHOW_TIMER;
          }else if(state==STATE_SHOW_TIMER){
            if(!buzzer.isMuted()) buzzer.playEditMemorySound();
            state = STATE_EDIT_MEMORY;
          }
          break;

        case REMOTE_NUM_0:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            eepromWorkingByte = eepromWorkingByte^BIT0;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            rtc.setDate(rtc.getDay(), 0, rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
          }
          break;
        case REMOTE_NUM_1:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            eepromWorkingByte = eepromWorkingByte^BIT1;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            rtc.setDate(rtc.getDay(), 1, rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
          }
          break;          
        case REMOTE_NUM_2:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            eepromWorkingByte = eepromWorkingByte^BIT2;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            rtc.setDate(rtc.getDay(), 2, rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
          }
          break;
        case REMOTE_NUM_3:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            eepromWorkingByte = eepromWorkingByte^BIT3;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            rtc.setDate(rtc.getDay(), 3, rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
          }
          break;            
        case REMOTE_NUM_4:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            eepromWorkingByte = eepromWorkingByte^BIT4;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            rtc.setDate(rtc.getDay(), 4, rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
          }
          break;
        case REMOTE_NUM_5:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            eepromWorkingByte = eepromWorkingByte^BIT5;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            rtc.setDate(rtc.getDay(), 5, rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
          }
          break;  
        case REMOTE_NUM_6:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            eepromWorkingByte = eepromWorkingByte^BIT6;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            rtc.setDate(rtc.getDay(), 6, rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
          }
          break;
        case REMOTE_NUM_7:
          if( state==STATE_EDIT_MEMORY ){
            if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
            eepromWorkingByte = eepromWorkingByte^BIT7;
          }
          break;
        case REMOTE_NUM_8:
          //////////////////////////////
          ///////////TEST ALARMOW
          if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
          if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
          ////setAlarm(byte index, byte on_off, byte hour, byte minute, byte cycle, byte nap, byte year, byte month, byte day){
//        alarmManager.setAlarm(index,  on_off, hour, minute, cycle,  nap,  year,  month, day);
          alarmManager.setAlarm(0,      1,      23,   20,     255,    10,   17,    6,     1);
          alarmManager.setAlarm(1,      1,      1,    10,     1,      10,   0,     1,     1);
          alarmManager.setAlarm(2,      1,      10,   10,     13,     15,   17,    5,     31);
          alarmManager.setAlarm(3,      1,      12,   10,     129,    20,   99,    5,     20);
          alarmManager.setAlarm(4,      1,      13,   10,     128,    40,   100,   5,     20);
          alarmManager.setAlarm(5,      1,      14,   10,     141,    59,   17,    5,     20);
          alarmManager.setAlarm(6,      1,      20,   10,     255,    60,   17,    12,    50);
          alarmManager.setAlarm(7,      1,      23,   10,     255,    70,   17,    13,    32);

          delay(100);

          alarmManager.printfAlarmsFromEeprom();
          alarmManager.printAlarmTab();

          alarmManager.updateAlarms(rtc.getMinute(), rtc.getHour(), rtc.getWeekday(), rtc.getDay(), rtc.getMonth(), rtc.getYear());

          delay(100);

          alarmManager.printfAlarmsFromEeprom();
          alarmManager.printAlarmTab();
          
          if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
          if(!buzzer.isMuted()) buzzer.playBitSwitchSound();
          
          ////
          /////////KONIEC TESTU ALARMOW
          //////////////////////////////////
          break;
        case REMOTE_NUM_9:
          if( state==STATE_SHOW_TIMER ){
            if(!buzzer.isMuted()) buzzer.playShowAlarms();
            state = STATE_SHOW_ALARMS;
          }else if( state==STATE_SHOW_ALARMS ){
            if(!buzzer.isMuted()) buzzer.playShowTimerSound();
            state = STATE_SHOW_TIMER;
          }
          break;
          
        case REMOTE_NUM_100_PLUS:
          if( state==STATE_SHOW_TIMER ){
            if(!buzzer.isMuted()) buzzer.playShowDateSound();
            state = STATE_SHOW_DATE;
          }else if(state==STATE_SHOW_DATE){
            if(!buzzer.isMuted()) buzzer.playShowTimerSound();
            state = STATE_SHOW_TIMER;
          }
          break;

        case REMOTE_NUM_200_PLUS:
          if( state==STATE_SHOW_TIMER || state==STATE_SHOW_DATE ){
            if(!buzzer.isMuted()) buzzer.playSetDateSound();
            state = STATE_SET_DATE;
          }else if(state==STATE_SET_DATE){
            if(!buzzer.isMuted()) buzzer.playShowTimerSound();
            state = STATE_SHOW_TIMER;
          }
          break; 

                  
      }
      irrecv.resume(); // Receive the next value
  }
  //////////// END REMOTE INPUT  ///////////
  //////////////////////////////////////////




  
  ////////////////////////////
  /////    LOOP STATES   /////
  ////////////////////////////

  ////////////////////////////////////////////
  ///////////// STATE_SHOW_TIMER /////////////
  if( state == STATE_SHOW_TIMER ){
    if ((unsigned long)(currentMillis - previousMillis) >= refresh_interval) {
      
        pcfController.showTime(rtc.getHour(), rtc.getMinute(), rtc.getSecond());
        //photoSensorValue = analogRead(PHOTO_SENSOR_PIN);
        //Serial.println(photoSensorValue);
        //mcp41controll.setToCurrentBrightness(photoSensorValue);
        mcp41controll.setToCurrentBrightness(analogRead(PHOTO_SENSOR_PIN));
      // Use the snapshot to set track time until next event  
      previousMillis = currentMillis;
    }

  //////////////////////////////////////////
  ///////////// STATE_SET_TIME /////////////
  }else if( state == STATE_SET_TIME ){
    pcfController.showTimeWithSelect(rtc.getHour(), rtc.getMinute(), rtc.getSecond(), timerIndex);
    mcp41controll.setToCurrentBrightness(analogRead(PHOTO_SENSOR_PIN));
    delay(50);

    
  //////////////////////////////////////////
  ////////// STATE_PLAY_ANIMATION //////////
  }else if( state == STATE_PLAY_ANIMATION ){
    if ((unsigned long)(currentSoundMillis - previousSoundMillis) >= buzzerInterval){
          if(!buzzer.isMuted()){
            buzzer.playAnimationenBip();
          }
      previousSoundMillis = currentSoundMillis;
    }
    
    if ((unsigned long)(currentMillis - previousMillis) >= animationSpeed) {
      
        animationStep = pcfController.playAnimationen(animationStep);
        if( animationStep%4 ==0 && buzzerInterval>=1){
          buzzerInterval = buzzerInterval/2;
        }
        
        if(animationStep >=30){
          state=STATE_SHOW_TIMER;
          if(!buzzer.isMuted()) buzzer.playShowTimerSound();
        }
        
      // Use the snapshot to set track time until next event  
      previousMillis = currentMillis;
    }
    
  ////////////////////////////////////////////
  ////////// STATE_SHOW_TEMPERATURE //////////
  }else if( state == STATE_SHOW_TEMPERATURE ){
    if ((unsigned long)(currentMillis - previousMillis) >= refresh_interval) {
      
        sensors.requestTemperatures();
        pcfController.showTemperature( sensors.getTempCByIndex(0) );
        pcfController.blinkerFlag = !pcfController.blinkerFlag;
        //Serial.println(sensors.getResolution());
        mcp41controll.setToCurrentBrightness(analogRead(PHOTO_SENSOR_PIN));
      // Use the snapshot to set track time until next event  
      previousMillis = currentMillis;
    }

  //////////////////////////////////////////
  //////////  STATE_EDIT_MEMORY   //////////
  }else if( state == STATE_EDIT_MEMORY ){
    if ((unsigned long)(currentMillis - previousMillis) >= refresh_interval) {
      
        pcfController.showMemoryEdit(eepromByteAddress, eepromWorkingByte, eepromLoadedByte);
        mcp41controll.setToCurrentBrightness(analogRead(PHOTO_SENSOR_PIN));
      // Use the snapshot to set track time until next event  
      previousMillis = currentMillis;
    }
    
 
  ////////////////////////////////////////
  //////////  STATE_SHOW_DATE   //////////
  }else if( state == STATE_SHOW_DATE ){
    if ((unsigned long)(currentMillis - previousMillis) >= refresh_interval) {
      
        if(blinkerFlag){
          pcfController.showTime(rtc.getYear(), rtc.getMonth(), rtc.getDay());
        }else{
          pcfController.showTime(rtc.getYear(), rtc.getMonth(), rtc.getWeekday());
        }
        blinkerFlag=!blinkerFlag;
        
        mcp41controll.setToCurrentBrightness(analogRead(PHOTO_SENSOR_PIN));
        
      // Use the snapshot to set track time until next event  
      previousMillis = currentMillis;
    }
    
 
  ///////////////////////////////////////
  //////////  STATE_SET_DATE   //////////
  }else if( state == STATE_SET_DATE ){
    if ((unsigned long)(currentMillis - previousMillis) >= (refresh_interval/10)) {
      
        if(blinkerFlag){
          pcfController.showTimeWithSelect(rtc.getYear(), rtc.getMonth(), rtc.getDay(), timerIndex);
        }else{
          pcfController.showTimeWithSelect(rtc.getYear(), rtc.getMonth(), rtc.getWeekday(), timerIndex);
        }
        blinkerFlag=!blinkerFlag;
        
        mcp41controll.setToCurrentBrightness(analogRead(PHOTO_SENSOR_PIN));
      // Use the snapshot to set track time until next event  
      previousMillis = currentMillis;
    }
    
  //////////////////////////////////////////
  //////////  STATE_SHOW_ALARMS   //////////
  }else if( state == STATE_SHOW_ALARMS ){
    if ((unsigned long)(currentMillis - previousMillis) >= (refresh_interval/10)) {
      
      pcfController.showTime( 
        activeAlarmIndex,
        alarmManager.getActiveAlarmHourByIndex(activeAlarmIndex),
        alarmManager.getActiveAlarmMinuteByIndex(activeAlarmIndex)
      );
        
        mcp41controll.setToCurrentBrightness(analogRead(PHOTO_SENSOR_PIN));
      // Use the snapshot to set track time until next event  
      previousMillis = currentMillis;
    }
    
 
  }

/*
sprawdzanie czy nie ma alarmu:
-co minutę
-po zmianie godziny

odswierzenie listy alarmów
-po zakończeniu edycji eepromu
-po zmianie daty
-po 24 godzinach


*/

}//end main loop






////////////////////////////
/////    FUNCTIONS     /////
////////////////////////////


void makeSerial(int baudRate){
  Serial.begin(baudRate);
  while(!Serial);
  Serial.println("Serial ready!");
}


void serialPrintTime(){
    Serial.print(rtc.formatTime());
    Serial.print("\n");
    Serial.print(rtc.formatDate());
    Serial.print("\n");
}



byte changeTimerIndex(byte _index, int value){ 
  if( (_index+value) > MAX_TIMER_INDEX ){
    return 0;
  }else if( (_index+value) < 0 ){
    return MAX_TIMER_INDEX;
  }else{
    return _index+value;
  }
}

byte changeActiveAlarmIndex(byte _index, int value){ 
  if( (_index+value) > ALARMS_AMOUNT ){
    return 0;
  }else if( (_index+value) < 0 ){
    return ALARMS_AMOUNT;
  }else{
    return _index+value;
  }
}


void changeTimeByIndex(byte index, int value){
  switch(index){
    case 0:
      rtc.setTime( validateValue(rtc.getHour(),value,24,0), rtc.getMinute(), rtc.getSecond() );
      break;
    case 1:
      rtc.setTime( rtc.getHour(), validateValue(rtc.getMinute(),value,60,0), rtc.getSecond() );
      break;
    case 2:
      rtc.setTime( rtc.getHour(), rtc.getMinute(), validateValue(rtc.getSecond(),value,60,0) );
      break;
      
    default:
      break;
  }
}



//setDate(byte day, byte weekday, byte month, byte century, byte year);
void changeDateByIndex(byte index, int value){
  switch(index){
    case 0:
      rtc.setDate(rtc.getDay(), rtc.getWeekday(), rtc.getMonth(), RTCC_CENTURY, validateValue(rtc.getYear(),value,99,0) );
      break;
    case 1:
      rtc.setDate( rtc.getDay(), rtc.getWeekday(), validateValue(rtc.getMonth(),value,12,1), RTCC_CENTURY, rtc.getYear() );
      break;
    case 2:
      rtc.setDate( validateValue(rtc.getDay(),value,31,1), rtc.getWeekday(), rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
      break;
      
    default:
      break;
  }
}

void changeWeekday(int value){
  rtc.setDate( rtc.getDay(), validateValue(rtc.getWeekday(),value,6,0), rtc.getMonth(), RTCC_CENTURY, rtc.getYear() );
}

byte validateValue( byte _time, int value, int maxValue, int minValue){
  maxValue--;
  if( (value + _time) >maxValue ){
    return minValue;
  }else if( (value + _time) <minValue ){
    return maxValue; 
  }else{
    return _time + value;
  }
}

byte validateValueOld( byte _time, int value, int maxValue){
  maxValue--;
  if( (value + _time) >maxValue ){
    return 0;
  }else if( (value + _time) <0 ){
    return maxValue; 
  }else{
    return _time + value;
  }
}


void initializeAnimation(){
  animationStep= 0;
  animationSpeed= 500;
  buzzerInterval= 2048;
  mcp41controll.setAllToPercentValue(95);

}

int changeEepromByteAddress(int change){
   
  if( (eepromByteAddress + change) > (EEPROM_MAXBYTES-1) ){
    return 0;
  }else if( (eepromByteAddress + change) < 0 ){
    return EEPROM_MAXBYTES-1;
  }else{
    return eepromByteAddress + change;
  }
}


void eepromReadProcedure(){
  eepromLoadedByte = EEPROM.read(eepromByteAddress);
  eepromWorkingByte = eepromLoadedByte;
}

void eepromWriteProcedure(int addres, byte value){
  EEPROM.write(addres, value);
  delay(5);
}

/*
void fillArray(byte tab[][2], int _i, int _j, byte value){
  for(int i=0; i<_i; i++){
    for(int j=0; j<_j; j++){
      tab[i][j]=value;
    }
  }
}
*/


