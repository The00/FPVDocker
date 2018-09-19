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


uint8_t nbr_cell =2;
uint8_t led_status =0;


void blink(uint8_t nbr, uint8_t mask)
{
	for(uint8_t i = 0;i<nbr; i++)
	{
		PORTB = mask;
		_delay_ms(BLINK_DIS);
		PORTB = 0x0;
		_delay_ms(BLINK_DIS);
	}
	PORTB = 0x0;
}

void blink_err(uint8_t nbr, uint8_t mask)
{
	for(uint8_t i = 0;i<nbr; i++)
	{
		PORTB = mask;
		_delay_ms(BLINK_ERR);
		PORTB = 0x0;
		_delay_ms(BLINK_ERR);
	}
	PORTB = 0x0;
}

void init()
{
	DIDR0 = 0xf; // disable input circuitry on I/O
	DDRB = (1<<LED_HIGH)| (1<<LED_MID) | (1<<LED_LOW); //set PB0 as input, PB1 & PB2 as output
	PORTB = 0x0;
//	SMCR = (1<<SM0); // sleep mode = ADC noise reduction
	ADMUX = (1<<VSENSE_PIN); // PB0 as ADC input
	
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)  ; // control and status register => enable ADC
}


void setup_timer()
{
	ADCSRB = 3; // start ADC conversion on compare A match 
	ADCSRA |= (1<<ADATE) | (1<<ADIE); //enable ADC auto-trigger mode & interrupt
	
	
	// set timer in Clear on Compare mode with OCR0A register and /8 prescaler
	OCR0A = 37500; //compare match every 300ms
	TCCR0A = 0;
	TIMSK0 = (1<< OCIE0A) ; //enable interrupt on channel A match
	TCCR0B = (1<<WGM02) | (1<< CS01);
}

uint8_t adcRead()
{
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

void displayLevel(uint8_t val)
{
	uint8_t cell = val/nbr_cell; 
	if(nbr_cell != ERR_UNKNOW_CELL)
	{
		// FULL THRESHOLD
		if((led_status & LED_HIGH) && (cell < BATT_FULL)) // if all LED are on and cell voltage is below first threshold
		{
			led_status = (1 << LED_LOW) | (1 <<LED_MID);
			PORTB = led_status;		
		} 
		// MID THRESHOLD
		else if(led_status & LED_MID)
		{
			if(cell > (BATT_FULL+HYST)) // check if we need to light up the previous LED.
			{
				led_status = (1 << LED_LOW) | (1 <<LED_MID) | (1 << LED_HIGH);
				PORTB = led_status;	
			}
			else if (cell < BATT_MID)
			{
				led_status = (1 << LED_LOW);
				PORTB = led_status;	
			}
		}	
		// LOW THRESHOLD
		else if(led_status & LED_LOW)
		{
			if(cell > (BATT_MID+HYST)) // check if we need to light up the previous LED.
			{
				led_status = (1 << LED_LOW) | (1 <<LED_MID);
				PORTB = led_status;
			}
			else if (cell < BATT_LOW)
			{
				led_status = 0;
				PORTB = led_status;
			}
		}
		//if none of the LED is ON
		else
		{
			if(cell > (BATT_LOW+HYST)) // check if we need to light up the previous LED.
			{
				led_status = (1 << LED_LOW); 
				PORTB = led_status;
			}
			// Critical THRESHOLD
			else if(cell > BATT_CRITICAL + HYST)
			{
				led_status = 0;
				PORTB = led_status;
			}
			else
			{
				PORTB = ~(PORTB & 0x7); // if below the critical threshold, blink all LED.
			}
		}
	}
}

void displayLevel2(uint8_t val)
{
	uint8_t cell = val/nbr_cell;
	
	if(cell > (BATT_FULL +HYST)) PORTB = (1 << LED_LOW) | (1 <<LED_MID) | (1 << LED_HIGH);
	else if((cell < BATT_FULL) && (cell > (BATT_MID +HYST))) PORTB = (1 << LED_LOW) | (1 <<LED_MID);
	else if((cell < BATT_MID) && (cell > (BATT_LOW +HYST))) PORTB = (1 << LED_LOW);
	else if((cell < BATT_LOW) && (cell > (BATT_CRITICAL +HYST))) PORTB = 0;
	else if(cell < BATT_CRITICAL) PORTB = ~(PORTB & 0x7); // if below the critical threshold, blink all LED. 
	
	// work, but less secure than previous implementation bcz we can jump from any state to any other, and so the blink can be bugged
	// Nop, it won't work with ||, second condition always true !
	// with &&, maybe, but still not bullet proof
}

void displayNbrCell()
{
	if(nbr_cell == ERR_UNKNOW_CELL) 
	{
		while(1)
		{
			blink_err(15, 7);
		}
	}
	else if(nbr_cell == 4) blink(3, 7);
	else blink(3, nbr_cell <<1);
}



ISR(ADC_vect)
{
	//if((TIFR0 & OCF0A) != 0)
	//PORTB ^= 1 << PORTB1; // toggle PB1 
	displayLevel(ADCL);
	TIFR0 = 1<<OCF0A;

}




int main(void)
{	
	init();
	_delay_ms(20);
	detectNbrCell(adcRead());
	displayNbrCell();
	setup_timer();
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		sleep_enable();
		sleep_cpu();
		sleep_disable();
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

