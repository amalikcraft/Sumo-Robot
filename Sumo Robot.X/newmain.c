//This code implements twoIR sensors and two ultrasonic sensors to detect edges and attack opponent robots
// -Ahmad 5/10/23


#include <avr/io.h>             // This header file includes the appropriate Input/output definitions for the device
#include <util/delay.h>         // to use delay function we need to include this library
#include <stdlib.h>             // we'll be using itoa() function to convert integer to character array that resides in this library
#define _DELAY_BACKWARD_COMPATIBLE_


#define US_ERROR		-1      // we're defining two more variables two know if the ultrasonic sensor is working or not
#define	US_NO_OBSTACLE	-2
#define	US_PIN	PINC

int distance1, distance2;
unsigned int IR0,IR1, threshold;
int stopDelay = 550;
int turnDelay = 350;
int delay = 10;
int go = 5;
void checkyacorners();


unsigned int get_ADC( unsigned char channel ) {
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADMUX = (1<<REFS0);
	ADMUX = (ADMUX & 0xF8)|(channel);
	ADCSRA |= (1<<ADSC);
	while( ADCSRA & (1<<ADSC) );
	return ADC;
}


int USobject1(){
	
	DDRC |=(1<<PC5);
	PORTC |=(1<<PC5);	//high
	_delay_us(15);				//wait 15uS
	PORTC &=~(1<<PC5);	//low

	unsigned int i,result;
	
	for(i=0;i<600000;i++)
	{
		if(!(US_PIN & (1<<PC4)))
		continue;	//Line is still low, so wait
		else
		break;		//High edge detected, so break.
	}

	if(i==600000)
	return US_ERROR;	//Indicates time out
	
	//High Edge Found
	
	// Section -2 : This section is all about preparing the timer for counting the time of the pulse. Timers in microcontrllers is used for timimg operation
	//Setup Timer1
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);	// This line sets the resolution of the timer. Maximum of how much value it should count.
	TCNT1=0x00;			// This line start the counter to start counting time

	// Section -3 : This section checks weather the there is any object or not
	for(i=0;i<600000;i++)                // the 600000 value is used randomly to denote a very small amount of time, almost 40 miliseconds
	{
		if(US_PIN & (1<<PC4))
		{
			if(TCNT1 > 60000) break; else continue;   // if the TCNT1 value gets higher than 60000 it means there is not object in the range of the sensor
		}
		else
		break;
	}
	
	if(i==600000)
	return US_NO_OBSTACLE;	//Indicates time out

	//Falling edge found

	result=TCNT1;          // microcontroller stores the the value of the counted pulse time in the TCNT1 register. So, we're returning this value to the
	// main function for utilizing it later

	//Stop Timer
	TCCR1B=0x00;
	if(result > 60000)
	return US_NO_OBSTACLE;	//No obstacle
	else
	return ((result>>1)*0.034/2.0);
}



int USobject2(){
	
	DDRC |=(1<<PC5);
	PORTC |=(1<<PC5);	//high
	_delay_us(15);				//wait 15uS
	PORTC &=~(1<<PC5);	//low
	unsigned int i,result;
	for(i=0;i<600000;i++)
	{
		if(!(US_PIN & (1<<PC3)))
		continue;	//Line is still low, so wait
		else
		break;		//High edge detected, so break.
	}
	if(i==600000)
	return US_ERROR;	//Indicates time out
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);	// This line sets the resolution of the timer. Maximum of how much value it should count.
	TCNT1=0x00;			// This line start the counter to start counting time

	for(i=0;i<600000;i++)                // the 600000 value is used randomly to denote a very small amount of time, almost 40 miliseconds
	{
		if(US_PIN & (1<<PC3))
		{
			if(TCNT1 > 60000) break; else continue;   // if the TCNT1 value gets higher than 60000 it means there is not object in the range of the sensor
		}
		else
		break;
	}
	if(i==600000)
	return US_NO_OBSTACLE;	//Indicates time out
	result=TCNT1;
	TCCR1B=0x00;
	if(result > 60000)
	return US_NO_OBSTACLE;	//No obstacle
	else
	return ((result>>1)*0.034/2.0);
}









void moveforward(int time){
	int i = 0;
	while(i<time){
        checkyacorners();
		PORTD = 0b00001011; //continue forward
		_delay_ms(5);

		i++;
	}
}

void moveback(int time){
	int i = 0;
	while(i<time){
        checkyacorners();
		PORTD = 0b00010101; //continue back
		_delay_ms(5);
		i++;
	}
	
}

void turnright(int time){
	int i = 0;
	while(i<time){
        checkyacorners();
		PORTD = 0b00010011; //turn right
		_delay_ms(5);
		i++;
	}
}

void turnleft(int time){
	int i = 0;
	while(i<time){
        checkyacorners();
		PORTD = 0b00001101; //turnleft
		_delay_ms(5);
		i++;
	}
	
}


void checkyacorners(){
		IR0 = get_ADC(1);
		IR1 = get_ADC(2);
		if( IR0 > threshold){
			PORTD = 0b00010101; //move back for a split second
			_delay_ms(550);
			PORTD = 0b00001101;  //turn left
			_delay_ms(350);
            PORTD = 0b00000000;
		} 
		if( IR1 > threshold){
            
			PORTD = 0b00010101; //move back for a split second
			_delay_ms(550);
			PORTD = 0b00010011;  //turn right
			_delay_ms(350);
            PORTD = 0b00000000;
		}
}		


int main(){
    
	threshold = get_ADC(0);
	DDRD = 0xFF;
    DDRB = 0b11111111;
	PORTD = 0b00001011; //move forward
    PORTB = 0b00000000;
	_delay_ms(100);
	
	while(1) {


		distance2 = USobject2()*14;//red
        distance1 = USobject1()*14;//green
        
        //RED thing on right
        if(distance2 < 30 && distance2 < distance1 ){
			PORTB = 0b00000001;
            turnleft(5);
			distance2 = USobject2()*14;//red
            distance1 = USobject1()*14;//green

		}
        
        //GREEN thing on left
		if(distance1 < 30 && distance1 < distance2 ){
			PORTB = 0b10000000;
            turnright(5);
            distance2 = USobject2()*14;//red
            distance1 = USobject1()*14;//green

		}
        
        //detected
        if(distance2 < 30 && distance1 < 30 && abs(distance1-distance2)<15 ){
            PORTB = 0b10000001;
            moveforward(50);
            distance2 = USobject2()*14;//red
            distance1 = USobject1()*14;//green

            
           // _delay_ms(1000);
		}
        //continue searching
        else{
           	PORTB = 0b00000000;
			moveforward(5);
            //distance2 = USobject2()*14;//red
            //distance1 = USobject1()*14;//green

        }
        

	}



}