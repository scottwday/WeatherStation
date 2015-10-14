#include "ublox.h"
#include "bma180.h"
#include "imu.h"
#include <Wire.h>
#include "bmp085.h"
#include <EspSoftSerialRx.h>
#include <CircularBuffer.h>
#include "bma180.h"

Adafruit_BMP085 bmp085;
BMA180 bma180;

EspSoftSerialRx gps;
String nmeaLine;

bool gotGprmc;
String gpsTime;
String gpsDate;
String gpsLat;
String gpsLong;

String getNextNmeaToken(String& line, int& start)
{
	String result;
	int i = start;

	while (i < line.length())
	{
		char c = line[i];

		if (c == ',')
		{
			if (start < i)
				result = line.substring(start, i);
			else
				result = "";
			start = i+1;
			break;
		}
		++i;
	}

	return result;
}

void parseNmeaLine()
{
	//GPS Fix message
	if (nmeaLine.startsWith("$GPRMC,"))
	{
		Serial.println(nmeaLine);

		int i = 7;
		gpsTime = getNextNmeaToken(nmeaLine, i);
		String okStr = getNextNmeaToken(nmeaLine, i);

		if (okStr == "A")
		{
			gpsLat = getNextNmeaToken(nmeaLine, i);
			String lnsStr = getNextNmeaToken(nmeaLine, i);
			gpsLat += lnsStr;

			gpsLong = getNextNmeaToken(nmeaLine, i);
			String lewStr = getNextNmeaToken(nmeaLine, i);
			gpsLong += lewStr;

			String spdStr = getNextNmeaToken(nmeaLine, i);
			String crsStr = getNextNmeaToken(nmeaLine, i);
			gpsDate = getNextNmeaToken(nmeaLine, i);
			String magStr = getNextNmeaToken(nmeaLine, i);
		}
		else
		{
			gpsLat = "";
			gpsLong = "";
			gpsTime = "";
			gpsDate = "";
		}
		
		gotGprmc = true;

		/*
		Serial.print("t=");
		Serial.print(datStr);
		Serial.print(";");
		Serial.print(timeStr);
		Serial.print(",");
		Serial.print(latStr);
		Serial.print(lnsStr);
		Serial.print(",");
		Serial.print(lonStr);
		Serial.print(lewStr);
		Serial.print(",ok=");
		Serial.print(okStr);
		Serial.print(",m=");
		Serial.print(magStr);
		Serial.println("");
		*/
	}
	/*
	//GPS Fix message
	if (nmeaLine.startsWith("$GPGGA,"))
	{
		int i = 7;
		String timeStr = getNextNmeaToken(nmeaLine, i);
		String latStr = getNextNmeaToken(nmeaLine, i);
		String lnsStr = getNextNmeaToken(nmeaLine, i);
		String lonStr = getNextNmeaToken(nmeaLine, i);
		String lewStr = getNextNmeaToken(nmeaLine, i);
		String fixStr = getNextNmeaToken(nmeaLine, i);
		String numStr = getNextNmeaToken(nmeaLine, i);
		String hdpStr = getNextNmeaToken(nmeaLine, i);
		String altStr = getNextNmeaToken(nmeaLine, i);

		Serial.print("g=");
		Serial.print(timeStr);
		Serial.print(",");
		Serial.print(latStr);
		Serial.print(lnsStr);
		Serial.print(",");
		Serial.print(lonStr);
		Serial.print(lewStr);
		Serial.print(",f=");
		Serial.print(fixStr);
		Serial.print(",n=");
		Serial.print(numStr);
		Serial.print(",h=");
		Serial.print(hdpStr);
		Serial.print(",a=");
		Serial.print(altStr);
		Serial.println("");
	}
	*/
}

void setup()
{
	Serial.begin(115200);

	pinMode(4, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP);

	bmp085.begin(BMP085_ULTRAHIGHRES);

	bma180.SetFilter(BMA180::FILTER::F10HZ);
	bma180.setGSensitivty(BMA180::G1);

	//gps.begin(9600, 12);

}

void loop()
{

  /* add main program code here */
	Serial.println("A");
	gps.setEnabled(false);

	int h = analogRead(A0);
	
	int t = (int)(bmp085.readTemperature() * 10);

	int p = bmp085.readPressure();

	bma180.readAccel();
//	Serial.printf("a=%d,%d,%d\n", (int16_t)bma180.x, (int16_t)bma180.y, (int16_t)bma180.z);
	float rx = atan2f((int16_t)bma180.x, (int16_t)bma180.y) * 180 / 3.141592f;
	float ry = atan2f((int16_t)bma180.x, (int16_t)bma180.z) * 180 / 3.141592f;
	
	gps.setEnabled(true);
	gps.reset();
	int timeout = 0;
	while (!gotGprmc)
	{
		delay(50);

		byte c;
		while (gps.read(c))
		{
			Serial.print((char)c);
			if (c > 13)
			{
				nmeaLine += (char)c;
			}
			else
			{
				parseNmeaLine();
				nmeaLine = "";
			}
		}

		timeout++;
		if ((timeout > 10) && (!gotGprmc))
			break;
	}
	gotGprmc = false;
	Serial.println("B");

	//Serial.print(nmeaLine);
	//nmeaLine = "";
	/*
	Serial.print("rh=");
	Serial.println(h);

	Serial.print("t=");
	Serial.print(t / 10);
	Serial.print(".");
	Serial.println(t % 10);

	Serial.print("p=");
	Serial.print(p / 100);
	Serial.print(".");
	Serial.println(p % 100);

	bma180.readAccel();
	Serial.print("a=");
	Serial.print((int16_t)bma180.x);
	Serial.print(",");
	Serial.print((int16_t)bma180.y);
	Serial.print(",");
	Serial.println((int16_t)bma180.z);
	
	Serial.print("r=");
	Serial.print(rx);
	Serial.print(",");
	Serial.println(ry);
	*/
	Serial.printf("%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d\n", 
		gpsDate.c_str(),
		gpsTime.c_str(),
		gpsLat.c_str(),
		gpsLong.c_str(),
		t,
		p,
		h,
		(int16_t)bma180.x,
		(int16_t)bma180.y,
		(int16_t)bma180.z,
		(int)(rx*1000),
		(int)(ry*1000));

	gps.service();

}
