/*
 * MyAlarmManager
 */
 
 
#include <MyAlarmManager.h>
#include "Arduino.h"
#include <EEPROM.h>


MyAlarmManager::MyAlarmManager(int _firstAlarmAddress){
	firstAlarmAddress = _firstAlarmAddress;
	maxAlarmAddress = ALARM_BYTE_SIZE * ALARMS_AMOUNT + firstAlarmAddress;
	
	for(int i=0; i<ALARMS_AMOUNT; i++){
		//for(int j=0; j<2; j++){		//pętla wykomentowana z powodu optymalizacji rozmiaru programu
			//alarmTab[i][j] = 0;
			alarmTab[i][0] = 0;
			alarmTab[i][1] = 0;
			currentEepromData[i] = 0;
		//}
	}
}
	
void MyAlarmManager::updateAlarms(byte minute, byte hour, byte weekday, byte day, byte month, byte year){
	bool valid = true;
	bool cycled = false;
	byte i=0;
	
	for(int currentAlarmAddres = firstAlarmAddress ;
		currentAlarmAddres < (ALARMS_AMOUNT * ALARM_BYTE_SIZE) + firstAlarmAddress ;
		currentAlarmAddres += ALARM_BYTE_SIZE, i++){
		
		cycled = false;
		valid = true;

		/*currentEepromData = */loadAlarmFromEepromByAddress_Pocedure(currentAlarmAddres);
		valid = isLoadAlarmInvalid_Pocedure();
		
		if(!valid){
			handleInvalidAlarmOnEeprom(currentAlarmAddres);
			continue;
		}
			
		if(currentEepromData[A_CYCLE]>128){
			cycled = true;
		}
		
		
		
		if(currentEepromData[A_ON_OFF]>0 && valid){
			
			//cykliczność
			if(cycled == true){		
				if( compareCycleAndWekday(currentEepromData[A_CYCLE], weekday) ){
					alarmTab[i][0] = currentEepromData[A_HOUR];
					alarmTab[i][1] = currentEepromData[A_MINUTE];
				}
				
				// jak weekday, się zgadza, to przypisać godzinę i minutę do alarmTab
				
			//brak cykliczności
			}else{
				if( currentEepromData[A_YEAR] == year && currentEepromData[A_MONTH] == month && currentEepromData[A_DAY] == day ){
					alarmTab[i][0] = currentEepromData[A_HOUR];
					alarmTab[i][1] = currentEepromData[A_MINUTE];
				}			
				//sprawdzić, year,month,day,hour, minute
				// jak year,month,day się zgadzają, to przypisać godzinę i minutę do alarmTab
			}
			
			
			
		}else{
			continue;
		}
		
		
		
	}

}


byte MyAlarmManager::checkAlarms(byte minute, byte hour){
	for(int i=0; i<ALARMS_AMOUNT; i++){
		if(alarmTab[i][0]==hour && alarmTab[i][1]==minute){
			isActive  = true;
			return i+1;
		}
	}

isActive = false;
return 0;
}

bool MyAlarmManager::isAlarmActive(){
	return isActive;
}

bool MyAlarmManager::setAlarm(byte index, byte on_off, byte hour, byte minute, byte cycle, byte nap, byte year, byte month, byte day){
	bool error = false;
	
	currentEepromData[A_ON_OFF]=	on_off;
	currentEepromData[A_HOUR]=		hour;
	currentEepromData[A_MINUTE]=	minute;
	currentEepromData[A_CYCLE]=		cycle;
	currentEepromData[A_NAP]=		nap;
	currentEepromData[A_YEAR]=		year;
	currentEepromData[A_MONTH]=		month;
	currentEepromData[A_DAY]=		day;
	
	
	error = isLoadAlarmInvalid_Pocedure();
	if(error)writeLoadedAlarmToEeprom_Pocedure(index*ALARM_BYTE_SIZE+firstAlarmAddress);
	if(!error) Serial.println("ERROREN");
	
	return error;
}

void MyAlarmManager::FORCEsetAlarm(byte index, byte on_off, byte hour, byte minute, byte cycle, byte nap, byte year, byte month, byte day){
	//bool error=false;
	
	currentEepromData[A_ON_OFF]=	on_off;
	currentEepromData[A_HOUR]=		hour;
	currentEepromData[A_MINUTE]=	minute;
	currentEepromData[A_CYCLE]=		cycle;
	currentEepromData[A_NAP]=		nap;
	currentEepromData[A_YEAR]=		year;
	currentEepromData[A_MONTH]=		month;
	currentEepromData[A_DAY]=		day;
	
	
	//error = isLoadAlarmInvalid_Pocedure();
	writeLoadedAlarmToEeprom_Pocedure(index*ALARM_BYTE_SIZE+firstAlarmAddress);
	
	//return error;
}




byte MyAlarmManager::getActiveAlarmHourByIndex(byte index){
	return alarmTab[index][0];
}

byte MyAlarmManager::getActiveAlarmMinuteByIndex(byte index){
	return alarmTab[index][1];
}







/////////////////////////
///////  PRIVATE  ///////
/////////////////////////

void MyAlarmManager::clearSingleEepromAlarm(int alarmIndex){

}

void MyAlarmManager::loadAlarmFromEepromByAddress_Pocedure(int address){
	currentEepromData[A_ON_OFF]=	EEPROM.read(address+A_ON_OFF);
	currentEepromData[A_HOUR]=	EEPROM.read(address+A_HOUR);
	currentEepromData[A_MINUTE]=	EEPROM.read(address+A_MINUTE);
	currentEepromData[A_CYCLE]=	EEPROM.read(address+A_CYCLE);
	currentEepromData[A_NAP]=	EEPROM.read(address+A_NAP);
	currentEepromData[A_YEAR]=	EEPROM.read(address+A_YEAR);
	currentEepromData[A_MONTH]=	EEPROM.read(address+A_MONTH);
	currentEepromData[A_DAY]=	EEPROM.read(address+A_DAY);
}

bool MyAlarmManager::isLoadAlarmInvalid_Pocedure(){
	bool valid = true;
	
	if(currentEepromData[A_HOUR]<0 || currentEepromData[A_HOUR]>23){
		valid = false;
		Serial.print(currentEepromData[A_HOUR]);
		Serial.print(" hour error, ");
	}
	
	if(currentEepromData[A_MINUTE]<0 || currentEepromData[A_MINUTE]>59){
		valid = false;
		Serial.print(currentEepromData[A_MINUTE]);
		Serial.print(" minute error, ");
	}
	
	if(currentEepromData[A_CYCLE]==128){	//8-my bit włącaza cykliczność 
		valid = false;
		Serial.print(currentEepromData[A_CYCLE]);
		Serial.print(" cycle error, ");
	}
	
	if(currentEepromData[A_NAP]>60){	
		currentEepromData[A_NAP]=60;	//opcjonalnie przypisać 0 i zwrócić błąd
		//EEPROM.write(address+A_NAP,60);
		//valid = false;
		Serial.print(currentEepromData[A_NAP]);
		Serial.print(" nap too big, ");
	}
	
	if(currentEepromData[A_YEAR]<0 || currentEepromData[A_YEAR]>99){
		valid = false;
		Serial.print(currentEepromData[A_YEAR]);
		Serial.print(" year error, ");
	}
	
	if(currentEepromData[A_MONTH]<0 || currentEepromData[A_MONTH]>12){
		valid = false;
		Serial.print(currentEepromData[A_MONTH]);
		Serial.print(" month error, ");
	}
	
	if(currentEepromData[A_DAY]<0 || currentEepromData[A_DAY]>31){
		valid = false;
		Serial.print(currentEepromData[A_DAY]);
		Serial.print(" day error, ");
	}
	
	Serial.println(" end.");
	return valid;
}

void MyAlarmManager::handleInvalidAlarmOnEeprom(int address){
	EEPROM.write(address+A_ON_OFF,0); // jeżeli błąd, to wylączamy alarm.

	if(currentEepromData[A_HOUR]<0 || currentEepromData[A_HOUR]>23){
		currentEepromData[A_HOUR]=0;
		EEPROM.write(address+A_HOUR,0);
	}
	
	if(currentEepromData[A_MINUTE]<0 || currentEepromData[A_MINUTE]>59){
		currentEepromData[A_MINUTE]=0;
		EEPROM.write(address+A_MINUTE,0);
	}
	
	if(currentEepromData[A_CYCLE]==128){	//8-my bit włącaza cykliczność 
		currentEepromData[A_CYCLE]=0;
		EEPROM.write(address+A_CYCLE,0);
	}
	
	if(currentEepromData[A_NAP]>60){	
		currentEepromData[A_NAP]=60;	//opcjonalnie przypisać 0 i zwrócić błąd
		EEPROM.write(address+A_NAP,60);
	}
	
	if(currentEepromData[A_YEAR]<0 || currentEepromData[A_YEAR]>99){
		currentEepromData[A_YEAR]=0;
		EEPROM.write(address+A_YEAR,0);
	}
	
	if(currentEepromData[A_MONTH]<0 || currentEepromData[A_MONTH]>12){
		currentEepromData[A_MONTH]=0;
		EEPROM.write(address+A_MONTH,0);
	}
	
	if(currentEepromData[A_DAY]<0 || currentEepromData[A_DAY]>31){
		currentEepromData[A_DAY]=0;
		EEPROM.write(address+A_DAY,0);
	}
}

void MyAlarmManager::writeLoadedAlarmToEeprom_Pocedure(int address){
	EEPROM.write(address+A_ON_OFF, currentEepromData[A_ON_OFF]);
	delay(5);
	EEPROM.write(address+A_HOUR, currentEepromData[A_HOUR]);
	delay(5);
	EEPROM.write(address+A_MINUTE, currentEepromData[A_MINUTE]);
	delay(5);
	EEPROM.write(address+A_CYCLE, currentEepromData[A_CYCLE]);
	delay(5);
	EEPROM.write(address+A_NAP, currentEepromData[A_NAP]);
	delay(5);
	EEPROM.write(address+A_YEAR, currentEepromData[A_YEAR]);
	delay(5);
	EEPROM.write(address+A_MONTH, currentEepromData[A_MONTH]);
	delay(5);
	EEPROM.write(address+A_DAY, currentEepromData[A_DAY]);
	delay(5);
}


void MyAlarmManager::printfAlarmsFromEeprom(){
	Serial.println();
	for(int index=0; index<ALARMS_AMOUNT; index++){
		loadAlarmFromEepromByAddress_Pocedure(index*ALARM_BYTE_SIZE+firstAlarmAddress);
		
		Serial.print(currentEepromData[A_ON_OFF]);
		Serial.print(", ");
		Serial.print(currentEepromData[A_HOUR]);
		Serial.print(", ");
		Serial.print(currentEepromData[A_MINUTE]);
		Serial.print(", ");
		Serial.print(currentEepromData[A_CYCLE]);
		Serial.print(", ");
		Serial.print(currentEepromData[A_NAP]);
		Serial.print(", ");
		Serial.print(currentEepromData[A_YEAR]);
		Serial.print(", ");
		Serial.print(currentEepromData[A_MONTH]);
		Serial.print(", ");
		Serial.print(currentEepromData[A_DAY]);
		Serial.println();
	}
}


void MyAlarmManager::printAlarmTab(){
	Serial.println();
	for(int i=0 ; i<ALARMS_AMOUNT ; i++){
			Serial.print(alarmTab[i][0]);
			Serial.print(":");
			Serial.println(alarmTab[i][1]);
	}
	Serial.println();
}

bool MyAlarmManager::compareCycleAndWekday(byte cycle, byte weekday){
	
	if( cycle & bitMask[weekday] ) return true;
	
	return false;
}