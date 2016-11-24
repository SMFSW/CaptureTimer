/*
	CaptureToAnalogic
	Using interruptible pin for frequency sampling (raw)
	and output samples to DAC (or duty cycle on a PWM)

	This example code is in the public domain.

	created 24 November 2016
	by SMSFSW
*/

#include <CaptureTimer.h>


#define acqPeriod		50		//!< Acquisition period

#define maxTicks		1000	//!< maximum ticks expected in an acquisition window

#define dacOUTPin		DAC0	//!< DAC output pin (can also be a PWM pin, even if there could be less interest)

#define icINPin			2		//!< Pin used for Input Capture (ticks count)

/*** GLOBAL VARS ***/
uint16_t valDAC = 0;


void setup()
{
	Serial.begin(115200);
	while (!Serial);	// Waiting for Serial to be ready

	CaptureTimer::init(icINPin, acqPeriod);

	analogWriteResolution(12);
}

void loop()
{
	uint16_t ticks;

	if (CaptureTimer::getTicks(&ticks) == true)		// new datas acquired (happens once every (acqPeriod)ms)
	{
		ticks = min(ticks, maxTicks);				// clamp value against max expected
		valDAC = map(ticks, 0, maxTicks, 0, 4095);	// map ticks to DAC range
		analogWrite(dacOUTPin, valDAC);				// ~0.58V to ~2.8V is expected on Due DAC for example
	}
}
