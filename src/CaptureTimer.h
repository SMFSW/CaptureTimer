/*!\file CaptureTimer.h
** \author SMFSW
** \version v0.8
** \date 2016-2017
** \copyright GNU Lesser General Public License v2.1
** \brief Arduino Input Capture Library
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

#ifndef CaptureTimer_h
#define CaptureTimer_h
#define CaptureTimer_ver	"v0.8"

#if defined(DOXY)
	// Define gcc __attribute__ as void when Doxygen runs
	#define __attribute__(a)	//!< GCC attribute (ignored by Doxygen)
#endif

#include <inttypes.h>

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) | defined(__AVR_AT90Tiny26__) | defined(__AVR_ATtiny26__)
#define		__TINY__
#endif

#define defaultFilterSpeed		2.0f	//!< Filter at normal speed is somewhat slow, speed x2 choosen by default

/*!\struct StructCap
** \brief CaptureTimer Structure
**/
typedef volatile struct StructCap{
		uint16_t		perAcq;			//!< acquisition period (in ms)
		uint16_t		perCnt;			//!< ms timer counter (only used on ATTiny yet)
		uint16_t		cnt;			//!< ticks counter
		struct StructStretch {
			uint16_t	perLow;			//!< period limit low (if Stretching is used)
			uint16_t	perHigh;		//!< period limit high (if Stretching is used)
		} Stretch;
		struct StructTickCapture {
			uint32_t	curTickTime;	//!< last tick time (in us)
			uint32_t	prevTickTime;	//!< previous than last tick time (in us)
			boolean		dataReady;		//!< true if new data is returned
		} TickCapture;
		uint16_t		ticksData;		//!< number of ticks in the last acquired time window
		float			ticksFiltered;	//!< number of ticks (integrated)
		float			filtSpeed;		//!< integration speed coeficient
		boolean			dataReady;		//!< true is ticksData has been updated since last read
		boolean			perStretch;		//!< true if period adaptation for higher output accuracy is enabled
		boolean			freqMes;		//!< true if freq measurement is enabled
		boolean			timeMes;		//!< true if time measurement between ticks is enabled
} capture;


/*!	\namespace CaptureTimer CaptureTimer.h "CaptureTimer/CaptureTimer.h"
**	\brief namespace containing the required methods for handling Input Capture
**/
namespace CaptureTimer
{
	//static void (*user_onEvent)(int);	// TODO: Implement callbacks, then switch to class for instanciation
	//static void onEventService(int);

	extern capture _cap;	//!< Capture struct instance

	/*!	\brief Initialisation routine (for frequency measurement)
	**	\param [in] per - Period of the timer (assuming it represents milliseconds)
	**	\param [in] pin - pin for input capture
	**	\param [in] edge - triggering edge
	**	\param [in] stretch - period stretching
	**	\return nothing
	**/
	void initCapTicks(const uint16_t per, const uint8_t pin, const uint8_t edge = FALLING, const boolean stretch = false);

	/*!	\brief Initialisation routine (for tick delay measurement)
	**	\param [in] pin - pin for input capture
	**	\param [in] edge - triggering edge
	**	\return nothing
	**/
	void initCapTime(const uint8_t pin, const uint8_t edge = FALLING);

	/*!	\brief Set a new sampling period
	**	\param [in] per - Period of the timer (assuming it represents milliseconds)
	**	\return nothing
	**/
	void setPeriod(const uint16_t per);

	/*!	\brief Set a new ticks filtering speed
	**	\param [in] speed - new speed multiplier (float 1.0f being normal speed)
	**	\param [in] restart - if set to true, starts filtered value from 0
	**	\return nothing
	**/
	void setFilterSpeed(const float speed, const boolean restart = false);

	/*!	\brief Check & perform period stretching if needed
	**	\return nothing
	**/
	void perStretch();

	/*!	\brief Start timer counting time for next tick
	**	\return nothing
	**/
	void startTickCapture(void);

	/*!	\brief Get the period between the last 2 ticks or since startTickSample called (in microseconds)
	**	\param [in, out] res - pointer to result of previous tick period in us (uint32 type)
	**	\return true if result returned is relevant
	**/
	boolean getTickCapture(uint32_t * res);

	/*!	\brief Get ticks count (with dataReady flag set up)
	**	\note if flag cap.dataReady doesn't need to be updated, use xgetTicks() instead
	**	\param [in, out] res - pointer to result of previous acquisition ticks count (uint16 type)
	**	\param [in] filt - result uses filtered value if true (latest acq value otherwise)
	**	\retval true - new data acquired
	**	\retval false - no new data (new sample pending)
	**/
	boolean getTicks(uint16_t * res, const boolean filt = false);

	/*!	\brief Get ticks count SCALED (with dataReady flag set up)
	**	\param [in, out] res - pointer to result of previous acquisition scaled ticks count (uint16 type)
	**	\param [in] scl - result scale
	**	\param [in] filt - result uses filtered value if true (latest acq value otherwise)
	**	\retval true - new data acquired
	**	\retval false - no new data (new sample pending)
	**/
	boolean getScaledTicks(uint16_t * res, const float scl, const boolean filt = false);

	/*!	\brief Get Frequency (with dataReady flag set up)
	**	\param [in, out] res - pointer to result of previous acquisition Frequency (uint16 type)
	**	\param [in] filt - result uses filtered value if true (latest acq value otherwise)
	**	\retval true - new data acquired
	**	\retval false - no new data (new sample pending)
	**/
	inline boolean __attribute__((__always_inline__)) getFreq(uint16_t * res, const boolean filt = false) {
		return getScaledTicks(res, 1000.0f, filt); }	// call getScaledTicks with a 1000ms time basis (means get Frequency)

	/*!	\brief Get the period of the capture timer
	**	\return Period in ms on uint16 type
	**/
	inline uint16_t __attribute__((__always_inline__)) getPeriod(void) {
		return _cap.perAcq; }		// get sampling period from capture struct

	/*!	\brief Is there new ticks data available
	**	\return \b true if new data ready, \b false otherwise
	**/
	inline boolean __attribute__((__always_inline__)) isTicksDataReady(void) {
		return _cap.dataReady & _cap.freqMes; }	// get dataReady flag from capture struct

	/*!	\brief Is there new time data available
	**	\return \b true if new data ready, \b false otherwise
	**/
	inline boolean __attribute__((__always_inline__)) isTimeDataReady(void) {
		return _cap.TickCapture.dataReady & _cap.timeMes; }	// get dataReady flag from capture struct

	/*!	\brief Get ticks count (without dataReady flag set up)
	**	\warning this inline doesn't update flag cap.dataReady
	**	\return Previous acquisition ticks count on uint16 type
	**/
	inline uint16_t __attribute__((__always_inline__)) xgetTicks(void) {
		return _cap.ticksData; }	// get ticks count from capture struct

	#if defined(__TINY__)
	void setTimerTiny(const uint16_t per);
	#endif

	/*!	\brief Sampling Timer handler
	**	\isr Sampling Timer handler
	**	\return nothing
	**/
	void isrTick_timer(void);

	/*!	\brief Tick trigger interrupt handler
	**	\isr Tick trigger interrupt handler
	**	\return nothing
	**/
	void isrTick_event(void);
}

#endif /* CaptureTimer_h */

