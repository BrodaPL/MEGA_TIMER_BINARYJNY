/*
 * MyMCP41
 */
 
 
 #ifndef MyMCP41_h
 #define MyMCP41_h
 
 #include "Arduino.h"
 #include <SPI.h>
 
 class MyMCP41{
 
	public:
		MyMCP41(byte _pin, byte _min, byte _max);
		void setAbsoluteValue(byte value);
		void setValue(byte value);
		void setPercentValue(float value);		
		void initialize();
		//void setSlavePin(byte _slavePin);
		//void setMin(byte newMin);
		//void setMax(byte newMax);
		byte getCurrentValue();
		byte getMin();
		byte getMax();
		byte getPin();
		//czy powinienem dodać gdziś " SPI.begin(); "??
		
 
	private:
		byte currentValue = 0;
		byte baseValue;
		byte min = 255;
		byte max = 0;
		byte pin = 0;
		
		void sendData(byte value);
 
 
 }
 ;
 
 
 #endif