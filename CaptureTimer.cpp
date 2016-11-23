/*!\file CaptureTimer.h
** \author SMFSW
** \version v0.1
** \date 2015-2015
** \copyright GNU Lesser General Public License v2.1
** \brief Arduino Input Capture Library
** \note use of MsTimer2 library for AVR architecture
** \note use of DueTimer library for SAM architecture: Timer0 (-> needs to be reworked if other arm core)
** \note use of NONE library for ESP8266 architecture (-> needs to be implemented)
** \details This library is intended to attach interrupt on a pin for frequency sampling
			
			Doxygen doc can be generated for the class using doxyfile
			
			Feel free to share your thoughts @ xgarmanboziax@gmail.com about:
				- issues encountered
				- optimisations
				- improvements & new functionalities
			
			
			This library is free software; you can redistribute it and/or
			modify it under the terms of the GNU Lesser General Public
			License as published by the Free Software Foundation; either
			version 2.1 of the License, or (at your option) any later version.
			
			This library is distributed in the hope that it will be useful,
			but WITHOUT ANY WARRANTY; without even the implied warranty of
			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
			Lesser General Public License for more details.
**/


#if ARDUINO > 22
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif


#if defined(__arm__) && defined(ARDUINO_SAM_DUE)
	// TODO: find appropriate ARM chip to select library (assume ARM is Due board yet)
	#include <DueTimer.h>
#elif defined(__AVR__)
	#include <MsTimer2.h>
#elif defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ESP8266_ESP01)
	// TODO: find appropriate timer library for ESP8266
	#error "ESP8266 not yet implemented"
#else
	#error "Unknown architecture"
#endif


#include "CaptureTimer.h"

volatile capture CaptureTimer::_cap = {100, 0, 0, false};

void CaptureTimer::init(uint8_t pin, uint16_t per, uint8_t edge)
{
	_cap.perAcq = per;

	pinMode(pin, INPUT_PULLUP);											// pulses counter pin set mode
	attachInterrupt(digitalPinToInterrupt(pin), isrIncPulses, edge);	// pulses counter interrupt pin
	#ifdef __AVR__
		MsTimer2::set(_cap.perAcq, isrGetPulses);						// pulses counter timer set period & callback
		MsTimer2::start();												// pulses counter timer start
	#else
		Timer0.attachInterrupt(isrGetPulses);							// pulses counter timer set callback
		Timer0.start(_cap.perAcq * 1000);								// pulses counter timer set period & start
	#endif
}

void CaptureTimer::setPeriod(uint16_t per)
{
	_cap.perAcq = per;

	#ifdef __AVR__
		MsTimer2::stop();							// pulses counter timer stop
		MsTimer2::set(_cap.perAcq, isrGetPulses);	// pulses counter timer set new period & callback
		MsTimer2::start();							// pulses counter timer restart
	#else
		Timer0.stop();								// pulses counter timer stop
		Timer0.start(_cap.perAcq * 1000);			// pulses counter timer set new period & restart
	#endif
}

boolean CaptureTimer::getPulses(uint16_t * res)
{
	boolean mem = _cap.dataReady;	// memorize if new datas is to be returned
	*res = _cap.pulsesData;			// write pulses count
	_cap.dataReady = false;			// erase ISR flag
	return mem;
}

boolean CaptureTimer::getScaledPulses(uint16_t * res, const float scl)
{
	float time_coef = scl / (float) _cap.perAcq;				// determine coefficient based on sampling time
	boolean mem = _cap.dataReady;								// memorize if new datas is to be returned
	*res = (uint16_t) ((float) _cap.pulsesData * time_coef);	// write scaled pulses count
	_cap.dataReady = false;										// erase ISR flag
	return mem;
}

boolean CaptureTimer::getFreq(uint16_t * res)
{
	return getScaledPulses(res, 1000.0f);	// call getScaledPulses with a 1000ms time basis (means get Frequency)
}


uint16_t CaptureTimer::getPeriod()
{
	return _cap.perAcq;	// get sampling period from capture struct
}

boolean CaptureTimer::isDataReady()
{
	return _cap.dataReady;	// get dataReady flag from capture struct
}

uint16_t CaptureTimer::xgetPulses()
{
	return _cap.pulsesData;	// get pulses count from capture struct
}


void CaptureTimer::isrIncPulses()
{
	_cap.cnt++;	// increase events count
}

void CaptureTimer::isrGetPulses()
{
	_cap.pulsesData = _cap.cnt;	// store pulses count
	_cap.cnt = 0;				// reset pulses counter
	_cap.dataReady = true;		// raise ISR flag
}

