/*
 * MyBuzzer
 */
 
#include "MyBuzzer.h"
#include "Arduino.h"

#define INT16_MAX_VALUE 32767


MyBuzzer::MyBuzzer(int _buzzer_pin){
	if(_buzzer_pin >= 0 && _buzzer_pin <= INT16_MAX_VALUE){
		buzzer_pin = _buzzer_pin;
	}else{
		buzzer_pin=0;
	}
	buzzer_pin = _buzzer_pin;
	//pinMode(buzzer_pin, OUTPUT);
	muted = false;
}

void MyBuzzer::initialize(){
	pinMode(buzzer_pin, OUTPUT);
}
 
int MyBuzzer::setBuzzer_pin(int _buzzer_pin){
	if(_buzzer_pin >= 0 && _buzzer_pin <= INT16_MAX_VALUE){
		buzzer_pin = _buzzer_pin;
	}else{
		buzzer_pin=0;
	}
	return buzzer_pin;
}

int MyBuzzer::getBuzzer_pin(){
	return buzzer_pin;
}


bool MyBuzzer::isMuted(){
	return muted;
}

void MyBuzzer::mute(){
	muted = true;
}

void MyBuzzer::unmute(){
	muted = false;
}

bool MyBuzzer::reverseMute(){
	muted = !muted;
	return muted;
}

void MyBuzzer::playBuzzerBip(int toneHz, int interval){
	tone(buzzer_pin, toneHz);
	delay(interval);
	noTone(buzzer_pin);
}

void MyBuzzer::playAnimationenBip(){
	tone(buzzer_pin, 1000);
	delay(50);
	noTone(buzzer_pin);
}

void MyBuzzer::playMuteSound(){
	int toneHz=2200;
	for(int i=0;i<=360;i++){
		tone(buzzer_pin, toneHz-i*6);
		delay(1);
	}
	noTone(buzzer_pin);
}

void MyBuzzer::playUnmuteSound(){
	int toneHz=40;
	for(int i=0;i<=360;i++){
		tone(buzzer_pin, toneHz+i*6);
		delay(1);
	}
	noTone(buzzer_pin);
}

void MyBuzzer::playShowTimerSound(){
	playBuzzerBip(600,50);
	playBuzzerBip(200,50);
	playBuzzerBip(1000,50);
}

void MyBuzzer::playSetTimeSound(){
	playBuzzerBip(1500,20);
	playBuzzerBip(300,40);
	playBuzzerBip(600,60);
	playBuzzerBip(1000,10);
	playBuzzerBip(500,40);
}

void MyBuzzer::playShowTemperatureSound(){
	playBuzzerBip(400,50);
	playBuzzerBip(0,50);
	playBuzzerBip(400,50);
}

void MyBuzzer::playInitializeAnimationenSound(){
	playBuzzerBip(600,100);
	playBuzzerBip(2000,50);
}

void MyBuzzer::playEditMemorySound(){
	playBuzzerBip(600,30);
	playBuzzerBip(900,30);
	playBuzzerBip(1200,30);
	playBuzzerBip(900,30);
	playBuzzerBip(600,30);
}

void MyBuzzer::playWriteMemorySound(){
	playBuzzerBip(3000,70);
	playBuzzerBip(2000,30);
	playBuzzerBip(2500,50);
}

void MyBuzzer::playBitSwitchSound(){
	playBuzzerBip(666,50);
}

void MyBuzzer::playShowDateSound(){
	playBuzzerBip(1000,20);
	playBuzzerBip(3000,40);
	playBuzzerBip(1000,80);
}

void MyBuzzer::playSetDateSound(){
	playBuzzerBip(1000,80);
	playBuzzerBip(3000,40);
	playBuzzerBip(5000,20);
}

void MyBuzzer::playShowAlarms(){
	playBuzzerBip(2500,20);
	playBuzzerBip(1000,30);
	playBuzzerBip(500,50);
	playBuzzerBip(1000,30);
	playBuzzerBip(2500,20);
}

