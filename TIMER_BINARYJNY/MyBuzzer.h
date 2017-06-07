/*
 * MyBuzzer
 */
 
 #ifndef MyBuzzer_h
 #define MyBuzzer_h
 
 #include "Arduino.h"

 
 class MyBuzzer{
	public:
		MyBuzzer(int _buzzer_pin);
		int setBuzzer_pin(int _buzzer_pin);
		int getBuzzer_pin();
		void initialize();
		bool isMuted();
		void mute();
		void unmute();
		bool reverseMute();
		void playBuzzerBip(int toneHz, int interval);
		
		void playAnimationenBip();
		void playMuteSound();
		void playUnmuteSound();
		void playShowTimerSound();
		void playSetTimeSound();
		void playShowTemperatureSound();
		void playInitializeAnimationenSound();
		void playEditMemorySound();
		void playWriteMemorySound();
		void playBitSwitchSound();
		void playShowDateSound();
		void playSetDateSound();
		void playShowAlarms();
 
	private:
		int buzzer_pin;
		boolean muted;
		
 }
 ;
 
 #endif
 