/*!\file CaptureTimer.h
** \author SMFSW
** \version v0.7
** \date 2016-2017
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
// TODO: debug TINY targets and find out what's not working
// TODO: implement some period stretching

#if ARDUINO > 22
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


#if defined(__arm__) && defined(ARDUINO_SAM_DUE)
	// TODO: find appropriate ARM chip to select library (assume ARM is Due board yet)
	#include <DueTimer.h>
#elif defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) | defined(__AVR_AT90Tiny26__) | defined(__AVR_ATtiny26__)
	//#error "TINY needs to be implemented"

	#define USEC_PER_TICK	1000							/* microseconds per clock interrupt tick */
	#define CLK_IT_OVERHEAD	4								/* fudge factor for clock interrupt overhead */
	#define PRESCALER		1024							/* timer clock prescaler */
	#define CLKS_PER_USEC	(F_CPU / PRESCALER / 1000000)	/* timer clocks per microsecond */
	#define INIT_TIMER		(256 - (USEC_PER_TICK * CLKS_PER_USEC) + CLK_IT_OVERHEAD)
#elif defined(__AVR__)
	#include <MsTimer2.h>
#elif defined(ARDUINO_ARCH_ESP8266) | defined(ARDUINO_ESP8266_ESP01)
	#include "user_interface.h"
	os_timer_t TimerESP;
#else
	#error "Unknown architecture"
#endif


#include "CaptureTimer.h"

volatile capture CaptureTimer::_cap = { 0, 0, 0, {0, 0}, {0, 0, false}, 0, 0.0f, defaultFilterSpeed, false, false, false, false };


#if defined(__TINY__)
/*!	\brief Initialisation of Tiny Timer
**	\return nothing
**/
static void CaptureTimer::setTimerTiny(void)
{
	// TODO: Tiny, check why no timer callback
	// setup Timer 1
	cli();
	TCCR1 = 0;							// normal mode
	// Prescaler 1024 (16M/1024 = 64 microseconds per tick)
	// timer interval can range from 64us to 128 microseconds
	TCCR1 = CTC1 | CS13 | CS11 | CS10;	// Auto reload
	TCNT1 = INIT_TIMER;					// set to 1ms time basis
	TIMSK |= TOIE1;						// Timer Overflow Interrupt Enable
	sei();								// enable interrupts
}
#endif


void CaptureTimer::initCapTicks(uint16_t per, uint8_t pin, uint8_t edge, boolean stretch)
{
	_cap.perAcq = per;
	_cap.perStretch = stretch;
	_cap.timeMes = false;
	_cap.freqMes = true;

	pinMode(pin, INPUT_PULLUP);													// ticks counter pin set mode

	#if defined(__TINY__)
		cli();
		GIMSK = 0b00100000;		// turns on pin change interrupts
		//PCMSK = 0b00010111;	// turn on interrupts on pins PB0, PB1, PB2 & PB4
		PCMSK |= 1 << pin;		// turn on interrupts on pin
		sei();
	#else
		attachInterrupt(digitalPinToInterrupt(pin), isrTick_event, edge);	// ticks counter interrupt pin
	#endif

	#if defined(__TINY__)
		setTimerTiny();
	#elif defined(__AVR__)
		MsTimer2::set(_cap.perAcq, isrTick_timer);							// ticks counter timer set period & callback
		MsTimer2::start();													// ticks counter timer start
	#elif defined(ARDUINO_ARCH_ESP8266) | defined(ARDUINO_ESP8266_ESP01)
		os_timer_setfn(&TimerESP, (void (*)(void*)) isrTick_timer, NULL);	// ticks counter timer set callback
		os_timer_arm(&TimerESP, _cap.perAcq, true);							// ticks counter timer set period & auto restart
	#else
		Timer0.attachInterrupt(isrTick_timer);								// ticks counter timer set callback
		Timer0.start(_cap.perAcq * 1000);									// ticks counter timer set period & start
	#endif
}

void CaptureTimer::initCapTime(uint8_t pin, uint8_t edge)
{
	_cap.timeMes = true;
	_cap.freqMes = false;

	pinMode(pin, INPUT_PULLUP);													// ticks counter pin set mode

#if defined(__TINY__)
		cli();
		GIMSK = 0b00100000;		// turns on pin change interrupts
		//PCMSK = 0b00010111;	// turn on interrupts on pins PB0, PB1, PB2 & PB4
		PCMSK |= 1 << pin;		// turn on interrupts on pin
		sei();
#else
	attachInterrupt(digitalPinToInterrupt(pin), isrTick_event, edge);	// ticks counter interrupt pin
#endif
}

void CaptureTimer::setPeriod(uint16_t per)
{
	if (!_cap.freqMes)	{ return; }

	_cap.perAcq = per;

	#if defined(__TINY__)
		setTimerTiny(_cap.perAcq);
	#elif defined(__AVR__)
		MsTimer2::stop();							// ticks counter timer stop
		MsTimer2::set(_cap.perAcq, isrTick_timer);	// ticks counter timer set new period & callback
		MsTimer2::start();							// ticks counter timer restart
	#elif defined(ARDUINO_ARCH_ESP8266) | defined(ARDUINO_ESP8266_ESP01)
		os_timer_disarm(&TimerESP);
		os_timer_setfn(&TimerESP, (void (*)(void*)) isrTick_timer, NULL);
		os_timer_arm(&TimerESP, _cap.perAcq, true);
	#else
		Timer0.stop();								// ticks counter timer stop
		Timer0.start(_cap.perAcq * 1000);			// ticks counter timer set new period & restart
	#endif
}

void CaptureTimer::setFilterSpeed(float speed, boolean rst)
{
	//if (!_cap.freqMes)	{ return; }		// Not needed here as it doesn't start anything

	_cap.filtSpeed = speed;
	if (rst)	{ _cap.ticksFiltered = 0.0f; }
}

void CaptureTimer::perStretch()
{
}

void CaptureTimer::startTickCapture()
{
	//if (!_cap.timeMes)	{ return; }	// in this case, check is not that important

	_cap.TickCapture.curTickTime = _cap.TickCapture.prevTickTime = micros();
	_cap.TickCapture.dataReady = false;
}

boolean CaptureTimer::getTickCapture(uint32_t * res)
{
	boolean newData = isTimeDataReady();
	_cap.TickCapture.dataReady = false;

	#if defined(__TINY__) | defined(__AVR__) | defined(ARDUINO_ARCH_ESP8266) | defined(ARDUINO_ESP8266_ESP01)
		cli();//noInterrupts();
	#endif

	*res = _cap.TickCapture.curTickTime - _cap.TickCapture.prevTickTime;

	#if defined(__TINY__) | defined(__AVR__) | defined(ARDUINO_ARCH_ESP8266) | defined(ARDUINO_ESP8266_ESP01)
		sei();//interrupts();
	#endif

	return newData;
}

boolean CaptureTimer::getTicks(uint16_t * res, boolean filt)
{
	boolean newData = isTicksDataReady();							// mem if new data is to be returned
	_cap.dataReady = false;											// erase ISR flag
	*res = (uint16_t) (filt ? _cap.ticksFiltered : _cap.ticksData);	// write ticks count
	return newData;
}

boolean CaptureTimer::getScaledTicks(uint16_t * res, const float scl, boolean filt)
{
	float ticks = (float) (filt ? _cap.ticksFiltered : _cap.ticksData);
	float time_coef = scl / (float) _cap.perAcq;						// determine coefficient based on sampling time
	boolean newData = isTicksDataReady();								// mem if new data is to be returned
	_cap.dataReady = false;												// erase ISR flag
	*res = (uint16_t) (ticks * time_coef);								// write scaled ticks count
	return newData;
}

/*** Interrupt Service Routines ***/

#if defined(__TINY__)
inline	// inlining isrTick_event on ATTiny to avoid call to function from isr
#endif
void CaptureTimer::isrTick_event()
{
	_cap.cnt++;	// increase ticks count

	if (_cap.timeMes)
	{
		_cap.TickCapture.prevTickTime = _cap.TickCapture.curTickTime;
		_cap.TickCapture.curTickTime = micros();
		_cap.TickCapture.dataReady = true;
	}
}

#if defined(__TINY__)
inline	// inlining isrTick_timer on ATTiny to avoid call to function from isr
#endif
void CaptureTimer::isrTick_timer()
{
	_cap.ticksData = _cap.cnt;	// store ticks count for the time window
	_cap.cnt = 0;				// reset ticks counter
	_cap.dataReady = true;		// raise ISR flag
	
	_cap.ticksFiltered += _cap.filtSpeed * (((float) _cap.ticksData - _cap.ticksFiltered) / (float) _cap.perAcq);
	
	if (_cap.perStretch)	{ CaptureTimer::perStretch(); }
}

#if defined(__TINY__)
/*!	\brief Timer1 overflow ISR
**	\isr Timer1 overflow handler
**	\return nothing
**/
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
ISR(TIM1_OVF_vect)
#else
ISR(TIMER1_OVF_vect)
#endif
{
	TCNT1 = INIT_TIMER;
	if (++CaptureTimer::_cap.perCnt >= CaptureTimer::_cap.perAcq)
	{
		CaptureTimer::_cap.perCnt = 0;
		CaptureTimer::isrTick_timer();
	}
}

/*!	\brief Pin Change interrupt ISR
**	\isr Pin Change interrupt handler
**	\return nothing
**/
ISR(PCINT0_vect)
{
	// no need to clear any interrupt flag?
	CaptureTimer::isrTick_event();
}
#endif


/*
// behind the scenes function that is called when data is ready
void CaptureTimer::onEventService(uint32_t data)
{
	if(!user_onEvent)	{ return; }
	// alert user program
	user_onEvent(numBytes);
}

// sets function called on event
void CaptureTimer::onEvent(void (*function)(void))
{
	user_onEvent = function;
}
*/