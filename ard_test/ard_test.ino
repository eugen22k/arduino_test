// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       ard_test.ino
    Created:	
    Author:     eugen22k
*/

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include "Ucglib.h"
#include <TimeLib.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t


Adafruit_BMP085 bmp;
Ucglib_ILI9163_18x128x128_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);

float temp = 0;

void setup() {
	Serial.begin(9600);
	while (!Serial); // Needed for Leonardo only
	setSyncProvider(RTC.get);   // the function to get the time from the RTC
	if (timeStatus() != timeSet)
		Serial.println("Unable to sync with the RTC");
	else
		Serial.println("RTC has set the system time");

	bmp.begin();
	delay(1000);
	ucg.begin(UCG_FONT_MODE_SOLID);
	ucg.clearScreen();
}

void loop() {
	if (Serial.available()) {
		time_t t = processSyncMessage();
		if (t != 0) {
			RTC.set(t);   // set the RTC and the system time to the received value
			setTime(t);
		}
	}
	digitalClockDisplay();

	ucg.setRotate180();
	ucg.setFont(ucg_font_helvB08_tr);
	ucg.setColor(255, 255, 255);

	ucg.setPrintPos(0, 25);
	ucg.print("Temp = ");
	ucg.print(bmp.readTemperature());
	ucg.println(" *C");

	ucg.setPrintPos(0, 50);
	ucg.print("Pressure = ");
	ucg.print(bmp.readPressure());
	ucg.println(" Pa");
}

void digitalClockDisplay() {
	ucg.setPrintPos(0, 75);
	// digital clock display of the time
	ucg.print(hour());
	printDigits(minute());
	// printDigits(second());
	ucg.setPrintPos(0, 90);
	ucg.print(day());
	ucg.print(" ");
	ucg.print(month());
	ucg.print(" ");
	ucg.print(year());
}

void printDigits(int digits) {
	// utility function for digital clock display: prints preceding colon and leading 0
	ucg.print(":");
	if (digits < 10)
		ucg.print('0');
	ucg.print(digits);
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
	unsigned long pctime = 0L;
	const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

	if (Serial.find(TIME_HEADER)) {
		pctime = Serial.parseInt();
		return pctime;
		if (pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
			pctime = 0L; // return 0 to indicate that the time is not valid
		}
	}
	return pctime;
}
