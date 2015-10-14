// 
// 
// 

#include "ublox.h"

void Ublox::send(byte cls, byte id, byte len, byte* payload)
{
	byte crcA = 0;
	byte crcB = 0;
	
	crcA += cls;
	crcB += crcA;

	crcA += id;
	crcB += crcA;

	crcA += (len >> 0) & 0xFF; //LSB first
	crcB += crcA;
	crcA += (len >> 8) & 0xFF;
	crcB += crcA;

	for (int i = 0; i < len; i++)
	{
		crcA += payload[i];
		crcB += crcA;
	}

	Serial1.write(0xB5); //mu
	Serial1.write(0x62); //B
	Serial1.write(cls);
	Serial1.write(id);
	Serial1.write((len >> 0) & 0xFF);
	Serial1.write((len >> 8) & 0xFF);
	Serial1.write(payload, len);
	Serial1.write(crcA);
	Serial1.write(crcB);
}

void Ublox::service()
{
	byte c;
	byte lastc;
	String rx;
	while (serialRx.read(c))
	{
		if ((lastc == 0xB5) && (c == 0x62))
		{
			Serial.printf("ubx\n");
		}

		rx += (char)c;
		lastc = c;
	}


}

void Ublox::begin()
{
	
	Serial1.begin(38400);

	//send UBX41 message to GPS to set baud, protocol etc
	Serial1.printf("$PUBX,41,1,0003,0003,9600,0*30\r\n");

	Serial1.begin(9600);

	Serial1.printf("$PUBX,41,1,0003,0003,9600,0*30\r\n");

	serialRx.begin(38400, 12);

}