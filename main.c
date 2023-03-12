/*
 * GccApplication1.c
 *
 * Created: 11/21/2022 2:52:04 PM		edited based on lab 7
 * Author : Mohamed
 */ 


//freq after measure from oscilloscope 121.9hz ?? working
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>


//lab8
																			
#define pi 3.14159265359
unsigned char sinwaveArray[256];
int TimerTop = 255; //timer from 0 until reach 255 (256)

//if needed add global variables (use volatile to avoid compiler optimization)
volatile unsigned int j =0;	//for ISR loop


void sampleValue(){	//the sin wave array
	for (int n=0;n<256;n++)
	{
		sinwaveArray[n]= ((TimerTop-1)/2)*sin((2*pi*n)/TimerTop)+((TimerTop+1)/2);
	}
}

void init_PWM(); //upsettings pins so PWM start


int main(void)
{
	// disable interrupts (clear I bit in SREG)
	cli();
	
	//setting up the  sin wave array
	sampleValue();
	
	//Initialize PWM(fastPWM)
	init_PWM();

	//enable interrupts
	sei();

	while (1){


	} ;
}

void init_PWM()
{
	//this function configures the generated signal at OC0

	//initialize TCNTC0 = 0	
	TCNT0 = 0;
	TCCR0 = 0;	//initialize the Timer/Counter Control Register
	
	DDRB=0xFF;	//output mode
	PORTB=0x08; //pin 3 for oscilloscope

	// Make PB3 (OC0) to array that is dynamic stating from 0
	OCR0= sinwaveArray[0];
	
	//set OCR0 value
	//#define OCR0_VALUE ((F_CPU/2/N0/F_WAVE)-1)				//OCR0= OCR0_VALUE;

	// choose Fast PWM mode															   bit 3    bit 7
	TCCR0|=(1<<WGM01);	//activate Fast PWM from table page 80 in Datasheet Table 14-2. WGM01=1 WGM00=1
	TCCR0|=(1<<WGM00);
	//  Clear OC0 on compare match, set OC0 at BOTTOM,(non-inverting mode)	//COM01 COM00	(COM00 is already 0)
	TCCR0|=(1<<COM01);														//  =1    =0

	//PRESCALER decides the speed of increment of TCNT0
	//prescale 1								//CS02 CS01 CS00			//slid 5 The N variable represents the prescale factor (1,8,64,256,1024)
	TCCR0|=(1<<CS00);								//=0   =0   =1			
													
	// Bit 0 – TOIE0: Timer/Counter0 Overflow Interrupt Enable
	TIMSK|=(1<<TOIE0);	/// TOIE0 timer overflow flag << what is the diff		(last bit in TIMSK – Timer/Counter Interrupt Mask Register) datasheet
						/// OCIE0 timer counter commpate = Timer/Counter0 Compare Match Interrupt Enable
}


//!!!!important note TIMER0_OVF_vect is diff from TIMER0_COMP_vect

ISR(TIMER0_OVF_vect) // using TIMER0 interrupt to start contract sin wave from commutative interrupt
{
	OCR0=sinwaveArray[j];		//j will be 0 first time (the value in array)
	if(j==256){					//each time if j ==265 mean the last value in array so reset
		j=0;
	}
	else{						//the reset
		j++;
	}
}

//class notes
/*

increase the freq so the capasitor make a nearly sin wave (analog signal)
to control the analog of this (use small deyty cycle )

8mhz/...


56


265 is max sampling we can use for PWM signal sin

use calculation in start to make array

then use for loop to make the right array  <=== will use the PWM
timer ISR will getout PWM in pin

every overflow reset ocr

122hz freq of sin wave
*/
