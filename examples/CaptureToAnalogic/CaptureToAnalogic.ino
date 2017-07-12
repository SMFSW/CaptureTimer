/*
	CaptureToAnalogic
	Using interrupt-able pin for frequency sampling (raw)
	and output samples to DAC (or duty cycle on a PWM)

	This example code is in the public domain.

	created Nov 24 2016
	latest mod Jan 7 2016
	by SMFSW
*/

#include <CaptureTimer.h>


#define samplingPer			50		//!< Acquisition period

#define maxTicks			1000	//!< maximum ticks expected in an acquisition window

#if defined (__arm__)
	#define outPin			DAC0	//!< DAC output pin
	#define outMaxValue		4095	//!< DAC max value
#elif defined(__TINY__)
	#define outPin			1		//!< PWM output pin (less interest, but demonstrates the use)
	#define outMaxValue		255		//!< PWM max value
#elif defined (__AVR__)
	#define outPin			6		//!< PWM output pin (less interest, but demonstrates the use)
	#define outMaxValue		255		//!< PWM max value
#else	// On ESP8266
	#define outPin			14		//!< PWM output pin (LED near ESP chip if builtin)
	#define outMaxValue		1023	//!< PWM max value
#endif

#define tickOUTPin			4		// Pin used as tick generator (for standalone board testing purposes)
// Literal Pin / Board
// 4 / most arduino boards (or D4)
// D2 / WeMos D1 R2
// #4 / Feather HUZZAH
#define icINPin				2		//!< Pin used for Input Capture (ticks count)
// Literal Pin / Board
// 2 / most arduino boards (or D2)
// D4 / WeMos D1 R2
// #2 / Feather HUZZAH


/*** GLOBAL VARS ***/
uint16_t valOut = 0;
uint16_t maxOut = outMaxValue;

void tickGenerator()
{
	// Note that this tick generator stretches following the amount of code runned by cpu cycle & interrupts
	// use a real freq generator of any kind for accurate tick(s) if you want to test library results accuracy
	static const uint16_t ovf = ((uint16_t) -1L) / 50;	// overflow at 2% of full cnt scale
	static const uint16_t thr = ovf / 100;				// threshold at 1% of overflow cnt scale
	static uint16_t cnt = 0;
	
	digitalWrite(tickOUTPin, (cnt >= thr) ? HIGH : LOW);
	if (++cnt > ovf)	{ cnt = 0; }
}


void setup()
{
	#if !defined(__TINY__)
		Serial.begin(115200);
	#endif
	
	CaptureTimer::initCapTicks(samplingPer, icINPin);
	
	#if defined(__arm__)
		analogWriteResolution(12);	// set DAC resolution to 12 bits for higher output accuracy
	#endif
	
	pinMode(tickOUTPin, OUTPUT);
}

void loop()
{
	uint16_t ticks;
	
	tickGenerator();
	
	if (CaptureTimer::getTicks(&ticks) == true)			// new data acquired (happens once every (samplingPer)ms)
	{
		ticks = min(ticks, maxTicks);					// clamp value against max expected
		valOut = map(ticks, 0, maxTicks, 0, maxOut);	// map ticks to DAC/PWMDutyCycle range
		analogWrite(outPin, valOut);					// ~0.58V to ~2.8V is expected on Due DAC for example
		#if !defined(__TINY__)
			Serial.print("Ticks: ");
			Serial.println(ticks);
		#endif
	}
}
