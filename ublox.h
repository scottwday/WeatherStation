// ublox.h

#ifndef _UBLOX_h
#define _UBLOX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <EspSoftSerialRx.h>

class Ublox
{
public:
	void begin();
	void Ublox::service();
	void Ublox::send(byte cls, byte id, byte len, byte* payload);

private:
	EspSoftSerialRx serialRx;

};

#endif

