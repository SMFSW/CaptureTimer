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


#define samplingPer			50		//!< Acquisition period

#define maxTicks			1000	//!< maximum ticks expected in an acquisition window

#if defined (__arm__)
	#define outPin			DAC0	//!< DAC output pin
	#define outMaxValue		4095	//!< DAC max value
#elif defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) | defined(__AVR_AT90Tiny26__) | defined(__AVR_ATtiny26__)
	#define outPin			3		//!< PWM output pin (less interest, but demonstrates the use)
	#define outMaxValue		255		//!< PWM max value
#elif defined (__AVR__)
	#define outPin			6		//!< PWM output pin (less interest, but demonstrates the use)
	#define outMaxValue		255		//!< PWM max value
#else	// On ESP8266
	#define outPin			14		//!< PWM output pin (LED near ESP chip if builtin)
	#define outMaxValue		1023	//!< PWM max value
#endif

#define icINPin				2		//!< Pin used for Input Capture (ticks count)
// Litteral Pin / Board
// 2 / most arduino boards (or D2)
// D4 / WeMos D1 R2
// #2 / Feather HUZZAH


/*** GLOBAL VARS ***/
uint16_t valOut = 0;
uint16_t maxOut = outMaxValue;


void setup()
{
	CaptureTimer::init(samplingPer, icINPin);

	#if defined(__arm__)
		analogWriteResolution(12);	// set DAC resolution to 12 bits for higher accuracy
	#endif
}

void loop()
{
	uint16_t ticks;

	if (CaptureTimer::getTicks(&ticks) == true)			// new datas acquired (happens once every (samplingPer)ms)
	{
		ticks = min(ticks, maxTicks);					// clamp value against max expected
		valOut = map(ticks, 0, maxTicks, 0, maxOut);	// map ticks to DAC/PWMDutyCycle range
		analogWrite(outPin, valOut);					// ~0.58V to ~2.8V is expected on Due DAC for example
	}
}
