#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// variable initialization //
unsigned char tick=0;
unsigned char seconds=0;
unsigned char minutes=0;
unsigned char hours=0;


// INT0 ISR//
ISR(INT0_vect)
{
	// CLEAR STOP WATCH //
    seconds=0;
    minutes=0;
    hours=0;
}

//INT1 ISR //
ISR(INT1_vect)
{
	// no clock //
	TCCR1B&=~(1<<CS10);
	TCCR1B&=~(1<<CS11);
	TCCR1B&=~(1<<CS12);
}
//INT2 ISR //
ISR(INT2_vect)
{
	TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10); //RESUME clock
}

// timer 1 ISR //
ISR(TIMER1_COMPA_vect)
{
	tick=1;
}

// TIMER 1 initialization //
void timer1_ctc(void){

	TCCR1A=(1<<FOC1A)|(1<<FOC1B); // NON-PWM MODE
	TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10); //  WGM12 FOR CTC MODE - CS11&CS10 FOR PRESCALER FCPU/64
	TCNT1=0; // START VALUE FOR TIMER1
	// FCPU=1MHZ 1S=1000ms prescaler FCPU/64 64/1MHZ = 0,064ms 1S= 1000/0,064=15625.
	OCR1A=15625 ;
	TIMSK|=(1<<OCIE1A); // ENABLE interrupt

}

// EXTERNAL INTERRUPT INT0 TO REST STOP WATCH //
void int0_int(void){
	SREG&=~(1<<7);    // I-BIT DISABLE //
	DDRD&=~(1<<PD2);  // INT0  as Input //
	PORTD|=(1<<PD2);  // ENABLE Internal Pull up //
	GICR|=(1<<INT0);  // INT0 ENABLE //
	MCUCR|=(1<<ISC01); // Trigger interrupt at falling edge //
	SREG|=(1<<7);       // i-bit enable //
}

// EXTERNAL Interrupt INT1 To Pause The STOP WATCH //
void int1_int(void){
	SREG&=~(1<<7);   // I-BIT DISABLE //
	DDRD&=~(1<<PD3); // INT1 As Input //
	GICR|=(1<<INT1); // INT1 ENABLE //
	MCUCR|=(1<<ISC11)|(1<<ISC10); // Trigger interrupt at raising edge //
	SREG|=(1<<7);      // i-bit enable //
}

// EXTERNAL INTERRUPT INT2 To resume the stop watch //
void int2_int(void){
	SREG&=~(1<<7);  // I-BIT DISABLE //
	DDRB&=~(1<<PB2);  // INT2 As Input //
    PORTB|=(1<<PB2);  // ENABLE Internal Pull up //
	GICR|=(1<<INT2);   // INT2 ENABLE //
	MCUCSR&=~(1<<ISC2); // Trigger interrupt at falling edge //
	SREG|=(1<<7);   // i-bit enable //
}


// main //
int main(void){
	DDRC|=0X0F;  // FIRST 4 PIN port c as output pins//
	PORTC&=0XF0; // initialization value for first 4 pins LOW;//

	DDRA|=0X3F;  // FIRST 6 PIN PORT D AS OUTPUT PINS //
	PORTA|=0X3F; // initialization value for first 4 pins HIGH;//

	SREG|=(1<<7);  //Enable global interrupts in MC by setting the I-Bit.

	int0_int();
	int1_int();
	int2_int();
	timer1_ctc();
	while(1)
	{
		if(tick==1)
		{
			seconds++;
			if(seconds==60)
			{
				seconds=0;
				minutes++;
			}
			if(minutes==60)
			{
				minutes=0;
				hours++;
			}
			if(hours>24){
				hours=0;
			}
			tick=0;
		}
			            //Display seconds//
			                 PORTA=(1<<5);
					   PORTC=(PORTC&0XF0)|(seconds%10);
					  	     _delay_ms(2);
					   	    PORTA=(1<<4);
					   PORTC=(PORTC&0XF0)|(seconds/10);
	       		   		   _delay_ms(2);
				       //Display minutes//
	       		   		     PORTA=(1<<3);
	       		   		   PORTC=(PORTC&0XF0)|(minutes%10);
						   	_delay_ms(2);
						   	PORTA=(1<<2);
					   PORTC=(PORTC&0XF0)|(minutes/10);
		    			     _delay_ms(2);
					   //Display hours//
		    			     PORTA=(1<<1);
					   PORTC=(PORTC&0XF0)|(hours%10);
					    	   _delay_ms(2);
					    	   PORTA=(1<<0);
					  PORTC=(PORTC&0XF0)|(hours/10);
						       _delay_ms(2);

 }

}
