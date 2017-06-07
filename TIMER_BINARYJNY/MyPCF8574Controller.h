/*
 * MyPCF8574Controller
 */
 
 #ifndef MyPCF8574Controller_h
 #define MyPCF8574Controller_h
 
#include "Arduino.h"
#include <PCF8574.h>
 
class MyPCF8574Controller{
	
	public:
		boolean  blinkerFlag = false;
	
		MyPCF8574Controller(byte addresses[]);
		byte getPcfsAmount();
		void initialize();
		void initializeAnimationen();
		void myClear(byte pcfIndex);
		void showNumber(byte pcfIndex, byte a);
		void showTime(byte hours, byte minutes, byte seconds);
		void showMemoryEdit(byte byteAddress, byte newValue, byte oldValue);  
		void showTimeWithSelect(byte hours, byte minutes, byte seconds, byte selected);
		byte playAnimationen(byte animationStep);
		void showTemperature(float temperature);
		
	private:
		byte anim1 = 255;
		byte anim2 = 255;
		byte anim3 = 255;
		PCF8574 *pcfs = NULL;
		byte pcfsAmount = 0;
		byte *pcfsAddresses;
		const byte bitMask[8] = {1,2,4,8,16,32,64,128};

}
;
 
 
#endif