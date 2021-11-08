/***************************************************************************/
/* headerfile for "e2Interface.cpp" module */
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

#ifndef e2Interface_H
#define e2Interface_H
#include "Arduino.h"

// constant definition
//-----------------------------------------------------------------------------
#define ACK 1
#define NAK 0

// definition of structs
//-----------------------------------------------------------------------------
typedef struct stE2Return
{
  short data;
  unsigned char status;
} stE2Return;

enum E2StatusCode
{
    E2_OK = 0,
    E2_ERR_NAK = 1,
    E2_ERR_CKSUM = 2,
    E2_ERR_MEAS = 3
};


// declaration of functions
//-----------------------------------------------------------------------------

class e2Interface
{	

public:
e2Interface(int sda,int scl);
stE2Return readShortFromSlave(unsigned char controlBytes[]); 	
stE2Return readByteFromSlave(unsigned char controlByte);
void start(void);
void stop(void);
void sendByte(unsigned char);
unsigned char readByte(void);
char checkAck(void);
void sendAck(void);
void sendNak(void);
void setSda(void);
void clearSda(void);
bool readSda(void);
void setScl(void);
void clearScl(void);

int E2_DEVICE_ADR = 0;
int E2_SDA = 18;
int E2_SCL = 19;
};

#endif