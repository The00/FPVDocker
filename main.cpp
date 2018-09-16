/*
 * Docker_BM.cpp
 *
 * Created: 09/09/2018 10:10:37
 * Author : Theo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
//#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>
#include "lipo.h"

void blink (uint8_t nbr)
{
	for(uint8_t i = 0;i<nbr; i++)
	{
		PORTB = 0x06;
		_delay_ms(200);
		PORTB = 0x0;
		_delay_ms(200);
	}
	PORTB = 0x0;
}

void init()
{
	DDRB = 0x6; //set PB0 as input, PB1 & PB2 as output
	PORTB = 0x0;
	ADMUX = 0; // PB0 as ADC input
	ADCSRB = 3; // start conversion on compare A match 
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADATE) | (1<<ADIE) ; // control and status register => enable ADC
}

uint8_t adcRead()
{
	ADMUX = 0; // PB0 as ADC input
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // control and status register => enable ADC
	ADCSRA |= (1<<ADSC); // start conversion
	while ((ADCSRA & (1<<ADIF))==0) // wait ADIF bit to be set (interrupt flag)
	{
			
	}
	ADCSRA = (1<<ADIF); // clear interrupt flag
	
	return ADCL;
}

void startConv()
{
	//ADMUX = 0; // PB0 as ADC input
	//ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADIE); // control and status register => enable ADC
	ADCSRA |= (1<<ADSC); // start conversion
	
}

uint8_t detectNbrCell(uint8_t val)
{
	if(val > MIN_4S) return 4;
	if(val > MIN_3S) return 3;
	if(val > MIN_2S) return 2;
	
	return ERR_UNKNOW_CELL;
}

void setup_timer()
{
	// set timer in Clear on Compare mode with OCR0A register and /8 prescaler
	OCR0A = 37500; //compare match every 300ms
	TCCR0A = 0;
	//TIMSK0 = (1<< OCIE0A) ; //enable interrupt on channel A match
	TCCR0B = (1<<WGM02) | (1<< CS01);
}

ISR(TIM0_OVF_vect)
{
	//PORTB ^= 1 << PORTB1; // toggle PB1
}

ISR(TIM0_COMPA_vect)
{
	//PORTB ^= 1 << PORTB1; // toggle PB1 
}


ISR(ADC_vect)
{
	//if((TIFR0 & OCF0A) != 0)
	PORTB ^= 1 << PORTB1; // toggle PB1 
	TIFR0 = 1<<OCF0A;

}




int main(void)
{	
	uint8_t nbr_cell =2;
	
	DIDR0 = 0xf; // disable input circuitry on I/O
	
	init();
	setup_timer();
	//blink(3);
	sei();
	
//	if((SREG & (128)) == 128) PORTB = PORTB | (1<< PORTB2);
    /* Replace with your application code */
    while (1) 
    {
		//PORTB ^= 1 << PORTB2;
		///_delay_ms(500);
		//startConv();
	/*nbr_cell = detectNbrCell(adcRead());
	if(nbr_cell != ERR_UNKNOW_CELL)
		{
			blink(2);
			nbr_cell--;
			PORTB = (nbr_cell <<1);
		}
	
	_delay_ms(1000);
	PORTB = 0;
	_delay_ms(1000);
	blink(1);*/
    }
}

