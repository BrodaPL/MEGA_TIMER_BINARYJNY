/*
 * MyMCP41Controller
 */
 
#include "MyMCP41Controller.h"
#include "Arduino.h"
#include <MyMCP41.h>
 //#include <SimpleList.h>
 
 
#define SUPER_BRIGHT 700
#define BRIGHT 550
#define DARK 250
#define SUPER_DARK 150

#define MCP41_BRIGHT_LVL5 100
#define MCP41_BRIGHT_LVL4 85
#define MCP41_BRIGHT_LVL3 65
#define MCP41_BRIGHT_LVL2 50
#define MCP41_BRIGHT_LVL1 30
#define MCP41_BRIGHT_LVL0 0
 
 
 MyMCP41Controller::MyMCP41Controller(MyMCP41 _tab[]){
	tab = _tab;
	mcp41Amount = sizeof(tab)+1;//
	
 }
 
 /*
 void MyMCP41Controller::setToCurrentBrightness(int lightLevel){
	switch(lightLevel){
		
		case lightLevel > SUPER_BRIGHT:
			setAllToPercentValue(MCP41_BRIGHT_LVL5);
			break;
		
		case ((lightLevel <= SUPER_BRIGHT) && (lightLevel > BRIGHT)):
			setAllToPercentValue(MCP41_BRIGHT_LVL4);
			break;
		
		case ((lightLevel <= BRIGHT) && (lightLevel > DARK)):
			setAllToPercentValue(MCP41_BRIGHT_LVL3);
			break;
		
		case ((lightLevel <= DARK) && (lightLevel > SUPER_DARK)):
			setAllToPercentValue(MCP41_BRIGHT_LVL2);
			break;
		
		case lightLevel <= SUPER_DARK:
			setAllToPercentValue(MCP41_BRIGHT_LVL1);
			break;
	}
	
 }
 */
 
 void MyMCP41Controller::setToCurrentBrightness(int lightLevel){
	if(lightLevel > SUPER_BRIGHT){
		setAllToPercentValue(MCP41_BRIGHT_LVL5);
		
	}else if( (lightLevel <= SUPER_BRIGHT) && (lightLevel > BRIGHT) ){
		setAllToPercentValue(MCP41_BRIGHT_LVL4);
	
	}else if( (lightLevel <= BRIGHT) && (lightLevel > DARK) ){
		setAllToPercentValue(MCP41_BRIGHT_LVL3);
	
	}else if( (lightLevel <= DARK) && (lightLevel > SUPER_DARK) ){
		setAllToPercentValue(MCP41_BRIGHT_LVL2);
	
	}else if(lightLevel <= SUPER_DARK){
		setAllToPercentValue(MCP41_BRIGHT_LVL1);

	}
	//else{
	//	setAllToPercentValue(MCP41_BRIGHT_LVL0);
	//}
	
 }
 
 void MyMCP41Controller::setAllToPercentValue(float percent){
	for(byte i=0; i<mcp41Amount; i++){
		tab[i].setPercentValue(percent);
	}
 }
 
void MyMCP41Controller::initialize(){
	for(byte i=0; i<mcp41Amount; i++){
		tab[i].initialize();
	}
}
 
 
