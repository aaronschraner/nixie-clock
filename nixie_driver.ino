#include "Arduino.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void setpin(int pin, bool value)
{
  digitalWrite(pin, value);
  //delay(2);
}
void nshiftOut(byte registers, byte clk, byte MR, byte STCP, byte* datapins, byte* data)
{
  //clear registers
  setpin(MR, LOW);
  setpin(MR, HIGH);

  //shift in 8 bits
  for(byte i=0x80; i; i>>=1)
  {
    setpin(clk, LOW);
    //setpin(STCP, HIGH);
    for(byte r=0; r<registers; r++)
    {
      setpin(datapins[r], data[r] & i);
    }
    setpin(clk, HIGH);
   // setpin(STCP, LOW);
  }
  setpin(STCP, LOW);
  setpin(STCP, HIGH);
  
}

inline byte numToSR(unsigned int num)
{
  //return 0x33;
  return (((num/10)%10) << 4) | (num % 10);
}

unsigned long tm(long t = -1)
{
  static unsigned long offset = millis();
  if(t >= 0)
    offset = t * 1000;
    
  return (millis() - offset) / 1000;
}
void loop() {
  
  // put your main code here, to run repeatedly:
  const byte regs = 3;
  byte clkpin = 9;
  byte MRpin = 11;
  byte STCPpin = 12;
  byte datapins[] = {8, 7, 6};
  byte data[3];
  pinMode(clkpin, OUTPUT);
  pinMode(MRpin, OUTPUT);
  pinMode(STCPpin, OUTPUT);
  for(int i=0; i< regs; i++)
  {
    pinMode(datapins[i], OUTPUT);
  }
  unsigned long dval=0;
  while(true)
  {
    dval = tm(); //millis() / 1000; ;// / 100;
    
    data[0] = numToSR((dval/60/60)%24);
    data[1] = numToSR((dval/60)%60);
    data[2] = numToSR(dval%60);
    //data[0] = data[1] = data[2];
//    data[0] = numToSR(12);
//    data[1] = numToSR(34);
//    data[2] = numToSR(56);
    Serial.println(dval);
    unsigned long tmr = millis();
    while(millis() < tmr + 1000)
    {
      byte dcopy[3];
      for(int j=0; j<6; j++)
      {
        dcopy[0] = (j==0 ?  data[0] & 0xF0 : 0xA0) | (j==1 ? data[0] & 0x0F : 0x0A);
        dcopy[1] = (j==2 ?  data[1] & 0xF0 : 0xA0) | (j==3 ? data[1] & 0x0F : 0x0A);
        dcopy[2] = (j==4 ?  data[2] & 0xF0 : 0xA0) | (j==5 ? data[2] & 0x0F : 0x0A);
        nshiftOut(regs, clkpin, MRpin, STCPpin, datapins, data);
        delay(1);
      }
      
    }
    //dval++;
    
  }

}
