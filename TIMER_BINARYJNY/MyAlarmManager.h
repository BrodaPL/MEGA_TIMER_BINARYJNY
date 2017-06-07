/*
 * MyAlarmManager
 */
 
 
#ifndef MyAlarmManager_h
#define MyAlarmManager_h
 
#include "Arduino.h"
#include <EEPROM.h>

#define ALARM_BYTE_SIZE 8
#define ALARMS_AMOUNT 8

#define A_ON_OFF 0
#define A_HOUR 1
#define A_MINUTE 2
#define A_CYCLE 3
#define A_NAP 4
#define A_YEAR 5
#define A_MONTH 6
#define A_DAY 7


class MyAlarmManager{
 
	public:
		
		MyAlarmManager(int _firstAlarmAddress );
		void updateAlarms(byte minute, byte hour, byte weekDay, byte day, byte month, byte year);
		byte checkAlarms(byte minute, byte hour);
		bool isAlarmActive();
		bool setAlarm(byte index, byte on_off, byte hour, byte minute, byte cycle, byte nap, byte year, byte month, byte day);
		void FORCEsetAlarm(byte index, byte on_off, byte hour, byte minute, byte cycle, byte nap, byte year, byte month, byte day);
		byte getActiveAlarmHourByIndex(byte index);
		byte getActiveAlarmMinuteByIndex(byte index);
		void printfAlarmsFromEeprom();
		void printAlarmTab();
 
	private:
		byte alarmTab[ALARMS_AMOUNT][2]; //[0]-hour [1]-minute
		byte currentEepromData[ALARM_BYTE_SIZE]={0,0,0,0,0,0,0,0};
		bool isActive=false;
		byte firstAlarmAddress = 0;
		int maxAlarmAddress;// = ALARM_BYTE_SIZE*ALARMS_AMOUNT +_firstAlarmAddress;
		const byte bitMask[8] = {1,2,4,8,16,32,64,128};

		void clearSingleEepromAlarm(int alarmIndex);
		void loadAlarmFromEepromByAddress_Pocedure(int address);
		bool isLoadAlarmInvalid_Pocedure();
		void handleInvalidAlarmOnEeprom(int address);
		void writeLoadedAlarmToEeprom_Pocedure(int address);
		bool compareCycleAndWekday(byte cycle, byte weekday);

		
 }
 ;
 
#endif