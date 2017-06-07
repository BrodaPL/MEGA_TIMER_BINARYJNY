/*
 * MyPCF8574Controller
 */
 
#include "MyPCF8574Controller.h"
#include "Arduino.h"

MyPCF8574Controller::MyPCF8574Controller(byte addresses[]){
	pcfsAmount = sizeof(addresses)+1;
	pcfs = new PCF8574[pcfsAmount];
	pcfsAddresses = addresses;
}

byte MyPCF8574Controller::getPcfsAmount(){
	return pcfsAmount;
}

void MyPCF8574Controller::initialize(){
	for(byte i=0; i<pcfsAmount; i++){
		pcfs[i].begin(pcfsAddresses[i]);
	}
	
	for(byte pcfIndex=0; pcfIndex < pcfsAmount; pcfIndex++){
		for(int i=0; i<=7; i++){
			pcfs[pcfIndex].pinMode(i, OUTPUT);
		}
	}
}

void MyPCF8574Controller::myClear(byte pcfIndex){
	for(int i=0; i<=7; i++){
		pcfs[pcfIndex].digitalWrite(i, LOW);
	}
}

void MyPCF8574Controller::showNumber(byte pcfIndex, byte a){
	pcfs[pcfIndex].write(a);
}

void MyPCF8574Controller::showTime(byte hours, byte minutes, byte seconds){
	pcfs[0].write(hours);
	pcfs[1].write(minutes);
	pcfs[2].write(seconds);
}

void MyPCF8574Controller::showMemoryEdit(byte byteAddress, byte newValue, byte oldValue){
	pcfs[0].write(byteAddress);
	pcfs[1].write(newValue);
	pcfs[2].write(oldValue);
}
  
void MyPCF8574Controller::showTimeWithSelect(byte hours, byte minutes, byte seconds, byte selected){


	
  switch(selected){
    case 0:
     	pcfs[0].write(hours+128);
		pcfs[1].write(minutes);
		pcfs[2].write(seconds);
      break;
      
    case 1:
      	pcfs[0].write(hours);
		pcfs[1].write(minutes+128);
		pcfs[2].write(seconds);
      break;
      
    case 2:
      	pcfs[0].write(hours);
		pcfs[1].write(minutes);
		pcfs[2].write(seconds+128);
      break;
  }
}

void MyPCF8574Controller::initializeAnimationen(){
	anim1= 255;
	anim2= 255;
	anim3= 255;
}


//dodać żeby przesunięcie było parametrem funkcji albo klasy. 
byte MyPCF8574Controller::playAnimationen(byte animationStep){

// wszystko jest przesuniete o 2 aby dac male opoznienie na poczatek
  if( (animationStep>1) && (animationStep<=9)  ){
    anim1 -= bitMask[animationStep-2];
  }else if( (animationStep>9) && (animationStep<=17) ){
    anim2 -= bitMask[animationStep-10];
  }else if( (animationStep>17) && (animationStep<=25) ){
    anim3 -= bitMask[animationStep-18];
  }
  
  pcfs[0].write(anim1);
  pcfs[1].write(anim2);
  pcfs[2].write(anim3);


  
  animationStep++;
	
	return animationStep;

}


/* temperatura  = 38,423
 * tempPart1 = 38		// całkowite
 * tempPart2 = t*100 - p1*100	// po przecinku
 */ 
 //dodac sprawdzanie czy tempPart1 i ..2 nie przekracza 255.
void MyPCF8574Controller::showTemperature(float temperature){
  byte dot=0,tempPart1=0,tempPart2=0;
  
  tempPart1= (int)temperature;
  tempPart2= (int)(temperature*100 - tempPart1*100);

  
  if (blinkerFlag){		//rzeby kropka migała 
    dot=1;
  }else{
    dot=0;
  }
  
	//wyświetla część całkowitą, potem migającą kropkę, a następnie część dziesiętną
   pcfs[0].write(tempPart1);
   pcfs[1].write(dot);
   pcfs[2].write(tempPart2);

}
