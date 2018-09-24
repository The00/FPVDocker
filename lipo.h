/*
 * lipo.h
 *
 * Created: 09/09/2018 10:30:28
 *  Author: Theo
 */ 


#ifndef LIPO_H_
#define LIPO_H_

#define ERR_UNKNOW_CELL 0xFF

#define MIN_2S	0x62
#define MAX_2S	0x80

#define MIN_3S	0x93
#define MAX_3S	0xC1

#define MIN_4S	0xC4
#define MAX_4S	0xFF

//value are for 1 cell
#define BATT_FULL  61// 4V/cell
#define BATT_MID   59// 3.85V/cell
#define BATT_LOW	56// 3.65V/cell
#define BATT_CRITICAL	50// 3.3V/cell

#define VSENSE_PIN	PORTB0
#define LED_HIGH	PORTB0 //highest battery led
#define LED_MID		PORTB1
#define LED_LOW		PORTB2	//lowest battery led
#define ALL_LED		7

#define HYST	5

#define BLINK_ERR	80
#define BLINK_DIS	500

#define HIGH		0
#define MID			1
#define LOW			2
#define CRITICAL	4



#endif /* LIPO_H_ */