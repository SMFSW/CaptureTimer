/*
	CaptureToAnalogic
	Using interruptible pin for frequency sampling (raw)
	and output samples to DAC (or duty cycle on a PWM)

	This example code is in the public domain.

	created 24 November 2016
	latest mod 25 November 2016
	by SMSFSW
*/

#include <CaptureTimer.h>


#define acqPeriod		50			//!< Acquisition period

#define maxTicks		1000		//!< maximum ticks expected in an acquisition window

#if defined (__arm__)
	#define outPin		DAC0		//!< DAC output pin
#elif defined (__AVR__)
	#define outPin		6			//!< PWM output pin (less interest, but demonstrates the use)
#else	// On ESP8266
	#define outPin		D5			//!< PWM output pin (less interest, but demonstrates the use)
#endif

#define icINPin			2			//!< Pin used for Input Capture (ticks count)

/*** GLOBAL VARS ***/
uint16_t maxDAC = 4095;
uint16_t valDAC = 0;


void setup()
{
	Serial.begin(115200);
	while (!Serial);				// Waiting for Serial to be ready (only needed on particular boards)

	CaptureTimer::init(icINPin, acqPeriod);

	#if defined(__arm__)
		analogWriteResolution(12);	// set DAC resolution to 12 bits for higher accuracy
	#elif defined(__AVR__)
		maxDAC = 255;				// If on AVR, most probably using a PWM pin, resolution is 8bits
	#endif
}

void loop()
{
	uint16_t ticks;

	if (CaptureTimer::getTicks(&ticks) == true)			// new datas acquired (happens once every (acqPeriod)ms)
	{
		ticks = min(ticks, maxTicks);					// clamp value against max expected
		valDAC = map(ticks, 0, maxTicks, 0, maxDAC);	// map ticks to DAC/PWMDutyCycle range
		analogWrite(outPin, valDAC);					// ~0.58V to ~2.8V is expected on Due DAC for example
	}
}
