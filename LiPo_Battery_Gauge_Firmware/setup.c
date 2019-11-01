/*
 LiPo Battery Gauge
 Version: 1.2
 Author: Alex from insideGadgets (http://www.insidegadgets.com)
 Created: 13/10/2019
 Last Modified: 1/11/2019

 */
 
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define LOW 0
#define HIGH 1
#define false 0
#define true 1

// Watchdog timeouts to values
#define T16MS 0
#define T32MS 1
#define T64MS 2
#define T128MS 3
#define T250MS 4
#define T500MS 5
#define T1S 6
#define T2S 7
#define T4S 8
#define T8S 9

#define POWER_ON_PIN PD4
#define batteryADCPin 3
#define LED1 PD0
#define LED2 PD1
#define LED3 PD2
#define LED4 PD3
#define LED5 PB7
#define LED6 PD5
#define LED7 PD6
#define LED8 PD7
#define LED9 PB0
#define LED10 PB1

// ADC Refs
#define REF_1_1V 0
#define REF_AVCC 1

#define DELAY_TIME 50

// Sleep
void system_sleep(void) {
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode
	sleep_mode(); // System sleeps here
}

// Setup watchdog timed sequence
void setup_watchdog(uint8_t wdDelay) {
	if (wdDelay > 9) { 
		wdDelay = 9;
	}
	
	uint8_t bb = wdDelay & 7;
	if (wdDelay > 7) { 
		bb |= (1<<5);
	}
	
	cli(); // Turn interrupts off
	asm("WDR");
	
	MCUSR &= ~(1<<WDRF);
	// Start timed sequence
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	// Set new watchdog timeout value
	WDTCSR = bb | (1<<WDIE);
	
	sei(); // Turn interrupts back on
}

// Turn off the Watchdog
void turn_off_watchdog(void) {
	cli(); // Turn interrupts off
	asm("WDR");
	MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDIE);
	WDTCSR = 0;
	sei(); // Turn interrupts back on
}

// Watchdog sleep function - setup, sleep and turn off
void watchdog_sleep(uint8_t timeout) { 
	setup_watchdog(timeout);
	system_sleep();
	turn_off_watchdog();
}

// Read the ADC
int adc_read(uint8_t pin, uint8_t reference) {
	uint8_t low, high;
	
	// Set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits)
	if (reference == REF_1_1V) {
		ADMUX = (pin & 0x0F); // 1.1V reference
	}
	else {
		ADMUX = (1<<REFS0) | (pin & 0x0F); // AVCC Ref
	}
	
	set_sleep_mode(SLEEP_MODE_ADC); // Set sleep mode ADC
	sleep_mode(); // System sleeps here
	
	// Read ADCL first
	low = ADCL;
	high = ADCH;
	
	// combine the two bytes
	return (high << 8) | low;
}

// Animate LEDs at start-up
void animate_leds(void) {
	_delay_ms(100);
	
	// 5 samples over 25ms
	uint16_t batteryVoltageLevel = 0;
	for (uint8_t x = 0; x < 5; x++) {
		batteryVoltageLevel += adc_read(batteryADCPin, REF_1_1V);
		_delay_ms(5);
	}
	batteryVoltageLevel = batteryVoltageLevel / 5;
	
	// Red
	if (batteryVoltageLevel >= 675) {
		PORTD |= (1<<LED1);
		_delay_ms(DELAY_TIME);
	}
	if (batteryVoltageLevel >= 720) {
		PORTD |= (1<<LED2);
		_delay_ms(DELAY_TIME);
	}
	if (batteryVoltageLevel >= 750) {
		PORTD |= (1<<LED3);
		_delay_ms(DELAY_TIME);
	}
	
	// Blue
	if (batteryVoltageLevel >= 825) {
		PORTD |= (1<<LED4);
		_delay_ms(DELAY_TIME);
	}
	if (batteryVoltageLevel >= 850) {
		PORTB |= (1<<LED5);
		_delay_ms(DELAY_TIME);
	}
	if (batteryVoltageLevel >= 875) {
		PORTD |= (1<<LED6);
		_delay_ms(DELAY_TIME);
	}
	if (batteryVoltageLevel >= 890) {
		PORTD |= (1<<LED7);
		_delay_ms(DELAY_TIME);
	}
	
	// Green
	if (batteryVoltageLevel >= 910) {
		PORTD |= (1<<LED8);
		_delay_ms(DELAY_TIME);
	}
	if (batteryVoltageLevel >= 920) {
		PORTB |= (1<<LED9);
		_delay_ms(DELAY_TIME);
	}
	if (batteryVoltageLevel >= 930) {
		PORTB |= (1<<LED10);
	}
}

// Setup
void setup(void) {
	// Turn on pull ups
	PORTB |= (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5) | (1<<PB6); 
	PORTC |= (1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7); 
	
	// Setup outputs
	DDRD |= (1<<LED1) | (1<<LED2) | (1<<LED3) | (1<<LED4) | (1<<LED6) | (1<<LED7) | (1<<LED8);
	DDRB |= (1<<LED5)  | (1<<LED9) | (1<<LED10);
	
	// Blink LEDs
	//PORTD |= (1<<LED1) | (1<<LED2) | (1<<LED3) | (1<<LED4) | (1<<LED6) | (1<<LED7) | (1<<LED8);
	//PORTB |= (1<<LED5) | (1<<LED9) | (1<<LED10);
	//_delay_ms(200);
	//PORTD &= ~((1<<LED1) | (1<<LED2) | (1<<LED3) | (1<<LED4) | (1<<LED6) | (1<<LED7) | (1<<LED8));
	//PORTB &= ~((1<<LED5) | (1<<LED9) | (1<<LED10));
	
	// Set ADC prescale factor to 8
	// 1 MHz / 8 = 125 KHz, inside the desired 50-200 KHz range.
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);
	
	// Enable ADC/interrupt
	sbi(ADCSRA, ADEN);
	sbi(ADCSRA, ADIE);
	
	// Enable pin change interrupt on PD4
	sbi(PCMSK2, PCINT20);
	
	sei(); // Turn on interrupts
}
