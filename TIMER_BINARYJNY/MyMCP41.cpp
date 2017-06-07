/*
 * MyMCP41
 */
 
 #include "Arduino.h"
 #include "MyMCP41.h"
 #include <SPI.h>
 
 
	
MyMCP41::MyMCP41(byte _pin, byte _min, byte _max){
	pin = _pin;
	min = _min;
	max = _max;
}

void MyMCP41::setAbsoluteValue(byte value){
	currentValue = value;
	sendData(currentValue);
}

void MyMCP41::setValue(byte value){
	if(value>min){
		currentValue=min;
	}else if(value<max){
		currentValue=max;
	}else{
		currentValue = value;
	}
	sendData(currentValue);
}

void MyMCP41::setPercentValue(float value){
	if(value>100){
		value=100;
	}else if(value<0){
		value=0;
	}

	byte full = min - max;
	float onePercent = full/100.0;
	float result =0.0;
	//currentValue = (int)( (full/100) * value + 0.4) + max;
	result = ( onePercent * value + 0.4) + max;
	currentValue = (int)result;
	sendData(currentValue);
	
	/*
	Serial.print(pin);
	Serial.print(" ");
	Serial.print(value);
	Serial.print(" ");
	Serial.print(result);
	Serial.print(" ");
	Serial.println(currentValue);
	*/
}

void MyMCP41::initialize(){
	pinMode(pin, OUTPUT);
	digitalWrite(pin, HIGH);
}

//void MyMCP41::setSlavePin(byte _slavePin){
//}

//void MyMCP41::setMin(byte newMin){
//}

//void MyMCP41::setMax(byte newMax){
//}

byte MyMCP41::getCurrentValue(){
	return currentValue;
}

byte MyMCP41::getMin(){
	return min;
}

byte MyMCP41::getMax(){
	return max;
}

byte MyMCP41::getPin(){
	return pin;
}

void MyMCP41::sendData(byte value){
	digitalWrite(pin, LOW);
	SPI.transfer(B00010001);
	delay(5);
	SPI.transfer(value);
	delay(5);
	digitalWrite(pin, HIGH);
}

