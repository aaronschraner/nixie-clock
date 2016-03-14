#include <iostream>
#include <wiringPi.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#ifndef byte
typedef uint8_t byte;
#endif

void updateTime(byte*  screen);

void setpin(int pin, bool value)
{
	digitalWrite(pin, value);
	//delay(2);
}

//shift data out to N shift registers simultaneously (designed for 74hc595)
void nshiftOut(byte registers, byte clk, byte MR, byte STCP, byte* datapins, byte* data)
{
	//clear registers
	setpin(MR, LOW);
	setpin(MR, HIGH);

	//shift in 8 bits
	for(byte i=0x80; i; i>>=1)
	{
		//set clock low
		setpin(clk, LOW);

		//send out this bit of each register's desired value
		for(byte r=0; r<registers; r++)
		{
			setpin(datapins[r], data[r] & i);
		}

		//set clock high (clock in data)
		setpin(clk, HIGH);
	}
	//update the output latches 
	setpin(STCP, LOW);
	setpin(STCP, HIGH);

}

//convert a (preferably) 2-digit number 
//into a value that can be sent to a 
//shift register on the driver circuit.
inline byte numToSR(unsigned int num)
{
	       //most sig. digit ...  least sig. digit
	return (((num/10)%10) << 4) | (num % 10);
}

byte num(char c)
{
	return (c <= '9' && c >= '0') ? c - '0' : 0xA;
}

int main(int argc, char* argv[])
{

	//setup wiringPi
	if(wiringPiSetup() == -1)
		return -1;
	
	//number of registers
	const byte regs = 3;

	//pin assignments
	byte MRpin = 7;
	byte clkpin = 2;
	byte STCPpin = 3;
	byte datapins[] = {4,5,6}; //note: rightmost digit is pin[0]

	//place to store data for shift registers
	byte data[3];

	//set pins as outputs
	pinMode(clkpin, OUTPUT);
	pinMode(MRpin, OUTPUT);
	pinMode(STCPpin, OUTPUT);
	for(int i=0; i< regs; i++)
	{
		pinMode(datapins[i], OUTPUT);
	}

	//if a value to display is given
	if(argc > 1)
	{
		uint32_t disp = 0;
		for(int i = 0; i < strlen(argv[1]); i++)
		{
			//turn it into shift code for the driver circuit
			disp <<= 4;
			disp |= num(argv[1][i]);
		}
		//split it into bytes for each register
		data[2] = disp;
		data[1] = disp >> 8;
		data[0] = disp >> 16;

		//send it to the display
		nshiftOut(regs, clkpin, MRpin, STCPpin, datapins, data);

		//exit now
		return 0;
	}

	//loop forever
	while(true)
	{
		//update the data array with the current time
		updateTime(data);

		//send it to the driver circuit
		nshiftOut(regs, clkpin, MRpin, STCPpin, datapins, data);

		//wait 100 milliseconds
		delay(100);

	}

}



//send the current time into the data array
void updateTime(byte*  screen)
{
	static time_t rawtime;
	static struct tm * timeinfo;

	//get current time
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	//hours (12 hr format)
	screen[0] = numToSR((timeinfo->tm_hour + 11 ) % 12 + 1 );
	//minutes
	screen[1] = numToSR(timeinfo->tm_min); 
	//seconds
	screen[2] = numToSR(timeinfo->tm_sec);

}
