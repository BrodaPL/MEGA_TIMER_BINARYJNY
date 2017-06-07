/*
 * MyMCP41Controller
 */
 
 
#ifndef MyMPC41Controller_h
#define MyMPC41Controller_h
 
#include "Arduino.h"
#include <MyMCP41.h>
#include <SPI.h>

class MyMCP41Controller{
 
	public:
		MyMCP41Controller(MyMCP41 _tab[]);
		//setValueByIndex(int index, byte value);
		void setToCurrentBrightness(int lightLevel);
		void setAllToPercentValue(float percent);
		void initialize();
 
 
 
	private:
		MyMCP41 *tab;
		byte mcp41Amount = 0;
 
 
 }
 ;
 
#endif