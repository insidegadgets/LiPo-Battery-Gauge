/*
 LiPo Battery Gauge
 Version: 1.2
 Author: Alex from insideGadgets (http://www.insidegadgets.com)
 Created: 13/10/2019
 Last Modified: 1/11/2019
 
*/

#define F_CPU 1000000 // 1 MHz

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "setup.c"

int main (void) {
	setup();
	
	while(1) {
		// Power pin (PD4) low, go to sleep
		if (!(PIND & (1<<POWER_ON_PIN))) {
			_delay_ms(200);
			
			// Turn off LEDs
			PORTD &= ~((1<<LED1) | (1<<LED2) | (1<<LED3) | (1<<LED4) | (1<<LED6) | (1<<LED7) | (1<<LED8));
			PORTB &= ~((1<<LED5) | (1<<LED9) | (1<<LED10));
			
			// Disable ADC/interrupt
			cbi(ADCSRA, ADEN);
			cbi(ADCSRA, ADIE);
			
			// Enable pin change interrupt
			sbi(PCICR, PCIE2);
			system_sleep();
			
			// Enable ADC/interrupt
			sbi(ADCSRA, ADEN);
			sbi(ADCSRA, ADIE);
			
			// Disable pin change interrupt
			cbi(PCICR, PCIE2);
			_delay_ms(100);
			
			animate_leds(); // Animate LEDs at start
		}
		
		
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
		}
		else {
			PORTD &= ~(1<<LED1);
		}
		if (batteryVoltageLevel >= 720) {
			PORTD |= (1<<LED2);
		}
		else {
			PORTD &= ~(1<<LED2);
		}
		if (batteryVoltageLevel >= 750) {
			PORTD |= (1<<LED3);
		}
		else {
			PORTD &= ~(1<<LED3);
		}
		
		// Blue
		if (batteryVoltageLevel >= 825) {
			PORTD |= (1<<LED4);
		}
		else {
			PORTD &= ~(1<<LED4);
		}
		if (batteryVoltageLevel >= 850) {
			PORTB |= (1<<LED5);
		}
		else {
			PORTB &= ~(1<<LED5);
		}
		if (batteryVoltageLevel >= 875) {
			PORTD |= (1<<LED6);
		}
		else {
			PORTD &= ~(1<<LED6);
		}
		if (batteryVoltageLevel >= 890) {
			PORTD |= (1<<LED7);
		}
		else {
			PORTD &= ~(1<<LED7);
		}
		
		// Green
		if (batteryVoltageLevel >= 910) {
			PORTD |= (1<<LED8);
		}
		else {
			PORTD &= ~(1<<LED8);
		}
		if (batteryVoltageLevel >= 920) {
			PORTB |= (1<<LED9);
		}
		else {
			PORTB &= ~(1<<LED9);
		}
		if (batteryVoltageLevel >= 930) {
			PORTB |= (1<<LED10);
		}
		else {
			PORTB &= ~(1<<LED10);
		}
		
		// Sleep for 1 second
		watchdog_sleep(T1S);
	}
	
	return 0;
}

EMPTY_INTERRUPT(ADC_vect);
EMPTY_INTERRUPT(WDT_vect);

ISR(PCINT2_vect) {
	_delay_ms(200);
}
