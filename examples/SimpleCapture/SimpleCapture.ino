/*
	SimpleCapture
	Using interrupt-able pin for frequency sampling (raw)

	This example code is in the public domain.

	created Nov 22 2016
	latest mod Jan 7 2016
	by SMFSW
*/

#include <CaptureTimer.h>


#define tickOUTPin		4		// Pin used as tick generator (for standalone board testing purposes)// Literal Pin / Board
// 4 / most arduino boards (or D4)
// D2 / WeMos D1 R2
// #4 / Feather HUZZAH
#define ctINPin			2		// Pin used for Input Capture (ticks count)
// Literal Pin / Board
// 2 / most arduino boards (or D2)
// D4 / WeMos D1 R2
// #2 / Feather HUZZAH

#define samplingPer		500


void tickGenerator()
{
	// Note that this tick generator stretches following the amount of code runned by cpu cycle & interrupts
	// use a real freq generator of any kind for accurate tick(s) if you want to test library results accuracy
	static const uint16_t ovf = ((uint16_t) -1L) / 10;	// overflow at 10% of full cnt scale
	static const uint16_t thr = ovf / 100;				// threshold at 1% of overflow cnt scale
	static uint16_t cnt = 0;
	
	digitalWrite(tickOUTPin, (cnt >= thr) ? HIGH : LOW);
	if (++cnt > ovf)	{ cnt = 0; }
}


void setup()
{
	Serial.begin(115200);
	
	// initialize the capture timer pin & period
	CaptureTimer::initCapTicks(samplingPer, ctINPin);
	
	pinMode(tickOUTPin, OUTPUT);
}


void loop()
{
	uint16_t ticks;
	
	tickGenerator();
	
	if (CaptureTimer::getTicks(&ticks) == true)
	{	// Only print on serial if new data is available
		Serial.print("Ticks: ");
		Serial.print(ticks);
		Serial.print("\tFreq: ");
		(void) CaptureTimer::getFreq(&ticks);
		Serial.print(ticks);
		Serial.println("Hz");
	}
}
