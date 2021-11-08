/***************************************************************************/
/* sourcefile for "e2Interface.h" module */
/***************************************************************************/
/*
Implementation of E2 interface via bitbanging.

Copyright 2021 E+E Elektronik Ges.m.b.H.

Disclaimer:
This application example is non-binding and does not claim to be complete with regard
to configuration and equipment as well as all eventualities. The application example
is intended to provide assistance with the EE894 sensor module design-in and is provided "as is".
You yourself are responsible for the proper operation of the products described.
This application example does not release you from the obligation to handle the product safely
during application, installation, operation and maintenance. By using this application example,
you acknowledge that we cannot be held liable for any damage beyond the liability regulations
described.

We reserve the right to make changes to this application example at any time without notice.
In case of discrepancies between the suggestions in this application example and other E+E
publications, such as catalogues, the content of the other documentation takes precedence.
We assume no liability for the information contained in this document.
*/

// Includes
//-----------------------------------------------------------------------------
#include "e2Interface.h"
#include <Arduino.h>


e2Interface::e2Interface(int sda,int scl)
{
  E2_SDA = sda;
  E2_SCL = scl;
  pinMode(E2_SCL,OUTPUT);
}


stE2Return e2Interface::readShortFromSlave(unsigned char controlBytes[])	//read word from slave 
{ 
  stE2Return e2Return;
  unsigned char lowByte, highByte;
  e2Return.data = 0;
  e2Return = readByteFromSlave(controlBytes[0]);
  lowByte = e2Return.data;
  if (e2Return.status == E2_OK)
  { 
	e2Return = readByteFromSlave(controlBytes[1]);
    highByte = e2Return.data;
    if (e2Return.status == E2_OK)
	{ 
	  e2Return.data = (lowByte + (short)(highByte) * 256);
	}
  }
  return e2Return;
}


stE2Return e2Interface::readByteFromSlave(unsigned char controlByte)	//read byte from slave with controlbyte
{ 
  unsigned char checksum;
  stE2Return  e2Return;
  e2Return.status = E2_OK;
  e2Return.data = 0x00;
  unsigned char ctrlByteAddr = controlByte | (E2_DEVICE_ADR << 1);
  
  
  start();
  sendByte(ctrlByteAddr);
  if (checkAck() == ACK)
  {
    e2Return.data = readByte();
    sendAck();
    checksum = readByte();
    sendNak();
    if ( ( ( ctrlByteAddr + e2Return.data ) % 0x100 ) != checksum )
	{
      e2Return.status = E2_ERR_CKSUM;
	}
  }
  else 
  {
	e2Return.status = E2_ERR_NAK;
  }
  stop();
  return e2Return;
}


void e2Interface::start(void) 					//send start condition to E2-Interface
{ 
  pinMode(E2_SDA,OUTPUT);
  setSda();
  setScl();
  delayMicroseconds(300);
  clearSda();
  delayMicroseconds(300);
}


void e2Interface::stop(void) 					//send stop condition to E2-Interface
{ 
  pinMode(E2_SDA,OUTPUT);
  clearScl();
  delayMicroseconds(200);
  clearSda();
  delayMicroseconds(200);
  setScl();
  delayMicroseconds(200);
  setSda();
}


void e2Interface::sendByte(unsigned char value) //send byte to E2-Interface
{
  pinMode(E2_SDA,OUTPUT);
  unsigned char mask;
  for ( mask = 0x80; mask > 0; mask >>= 1)
  {
    clearScl();
    delayMicroseconds(100);
    if ((value & mask) != 0)
    {
      setSda();
    }
    else
    {
      clearSda();
    }
    delayMicroseconds(200);
    setScl();
    delayMicroseconds(300);
    clearScl();
  }
  setSda();
}


unsigned char e2Interface::readByte(void) 		//read byte from E2-Interface
{
  unsigned char data_in = 0x00;
  unsigned char mask = 0x80;
  for (mask = 0x80; mask > 0; mask >>= 1)
  {
    pinMode(E2_SDA,INPUT_PULLUP);
    clearScl();
    delayMicroseconds(300);
    setScl();
    delayMicroseconds(150);
    if (readSda())
    { 
		data_in |= mask;
    }
    delayMicroseconds(150);
    clearScl();
  }
  return data_in;
}


char e2Interface::checkAck(void) 				//check for acknowledge
{
  pinMode(E2_SDA,INPUT_PULLUP);
  bool input;
  clearScl();
  delayMicroseconds(300);
  setScl();
  delayMicroseconds(150);
  input = readSda();
  delayMicroseconds(150);
  if (input == 1) 								//SDA = LOW ==> ACK, SDA = HIGH ==> NAK
    return NAK;
  else
    return ACK;
}


void e2Interface::sendAck(void) 				//send acknowledge(ACK)
{
  pinMode(E2_SDA,OUTPUT);
  clearScl();
  delayMicroseconds(150);
  clearSda();
  delayMicroseconds(150);
  setScl();
  delayMicroseconds(280);
  clearScl();
  delayMicroseconds(20);
  setSda();
}


void e2Interface::sendNak(void) 				//send not-acknowledge(NAK)
{
  pinMode(E2_SDA,OUTPUT);
  clearScl();
  delayMicroseconds(150);
  setSda();
  delayMicroseconds(150);
  setScl();
  delayMicroseconds(300);
  setScl();
}



void e2Interface::setSda(void)
{ 
  digitalWrite(E2_SDA, HIGH); 					//set port-pin (SDA)
}


void e2Interface::clearSda(void)
{
  digitalWrite(E2_SDA, LOW); 					//clear port-pin (SDA)
}


bool e2Interface::readSda(void)
{
  return digitalRead(E2_SDA); 					//read SDA-pin status
}


void e2Interface::setScl(void)
{
  //digitalWrite(E2_SCL, HIGH);					//set port-pin (SCL)
  pinMode(E2_SCL,INPUT_PULLUP);
  bool input = digitalRead(E2_SCL);
  
  while(input == 0)
  {
    delayMicroseconds(1);
	input = digitalRead(E2_SCL);
  }
}


void e2Interface::clearScl(void)
{
  pinMode(E2_SCL,OUTPUT);
  digitalWrite(E2_SCL, LOW);					//clear port-pin (SCL)
}


/*
Disclaimer:
This application example is non-binding and does not claim to be complete with regard to configuration and equipment as well as all eventualities. The
application example is intended to provide assistance with EE894 Sensor Module design-in and is provided ”as is”.
You yourself are responsible for the proper operation of the products described.
This application example does not release you from the obligation to handle the product safely during application,
installation, operation and maintenance. By using this application example, you acknowledge that we cannot be held liable for any
damage beyond the liability regulations described. We reserve the right to make changes to this application example at any time without notice. In
case of discrepancies between the suggestions in this application example and other E+E publications, such as catalogues, the content of the other
documentation takes precedence.
We assume no liability for the information contained in this document.
*/