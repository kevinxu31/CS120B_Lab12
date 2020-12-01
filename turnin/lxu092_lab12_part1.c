/*	Author: lab
 *  Partner(s) Name: Luofeng xu
 *	Lab Section:
 *	Assignment: Lab 12  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: Youtube URL>https://youtu.be/iTl0UlzNmTM
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


typedef struct task{
        int state;
        unsigned long period;
        unsigned long elapsedTime;
        int(*TickFct)(int);
}task;

task tasks[1];
const unsigned short tasksNum=1;
const unsigned long tasksPeriod=100;

volatile unsigned char TimerFlag = 0; 
unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void TimerOn() {
	TCCR1B 	= 0x0B;	
	OCR1A 	= 125;	
	TIMSK1 	= 0x02; 
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;	
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B 	= 0x00; 
}

void TimerISR() {
	unsigned char i;
	for(i=0;i<tasksNum;++i){
		if(tasks[i].elapsedTime>=tasks[i].period){
			tasks[i].state=tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime=0;
		}
		tasks[i].elapsedTime+=tasksPeriod;
	}
}
ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) { 	
		TimerISR(); 				
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

#define A0 (~PINA&0x01)
#define A1 ((~PINA>>1)&0x01)
#define A2 ((~PINA>>2)&0x01)
#define A3 ((~PINA>>3)&0x01)


enum Shift_States {start,init,up,down};
int Shift_Tick(int state) {

	static unsigned char pattern = 0xFF;	
	static unsigned char row = 0xFB;  	
						
						
	switch (state) {
		case start:
			state=init;
			pattern=0xFF;
			row=0xFB;
			break;
		case init:
			if(A0&&(!A1)){
				state=up;
				if(row!=0xFE){
					row=(row>>1)|0x80;
				}
			}
			else if(A1&&(!A0)){
				state=down;
				if(row!=0xEF){
                                        row=(row<<1)|0x01;
                                }
			}
			else{
				state=init;
			}
			break;
		case up:
			if(A0&&(!A1)){
				state=up;
			}
			else if((!A0)&&(!A1)){
				state=init;
			}
			break;

		case down:
			if(A1&&(!A0)){
                                state=down;
                        }
                        else if((!A0)&&(!A1)){
                                state=init;
                        }
                        break;
		default:
			break;
	}
	switch (state) {
		case start:
                        break;
                case init:
			break;
                case up:
			break;
                case down:
			break;
		default:
			break;
	}
	PORTC = pattern;	// Pattern to display
	PORTD = row;		// Row(s) displaying pattern	
	return state;
}


int main(void) {
	DDRA=0x00;PORTA=0xFF;
	DDRC=0xFF;PORTB=0x00;
	DDRD=0xFF;PORTC=0x00;
	unsigned char i=0;
	tasks[i].state=start;
	tasks[i].period=100;
	tasks[i].elapsedTime=0;
	tasks[i].TickFct=&Shift_Tick;
	TimerSet(100);
	TimerOn();
	while (1) {
    	}
	return 1;
}
