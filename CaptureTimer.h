/*!\file CaptureTimer.h
** \author SMFSW
** \version v0.2
** \date 2015-2015
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

#include <inttypes.h>

/*!\struct StructCap
** \brief CaptureTimer Structure
**/
typedef volatile struct StructCap{
		uint16_t	perAcq;			//!< acquisition period (in ms)
		uint16_t	cnt;			//!< ticks counter
		uint16_t	ticksData;		//!< number of ticks in the last acquired time window
		boolean		dataReady;		//!< true is ticksData has been updated since last read
} capture;

/*!	\namespace CaptureTimer CaptureTimer.h "CaptureTimer/CaptureTimer.h"
**	\brief namespace containing the required methods for handling Input Capture
**/
namespace CaptureTimer
{
	extern capture _cap;	//!< Capture struct instance

	/*!	\brief Tick trigger interrupt handler
	**	\isr Tick trigger interrupt handler
	**	\return nothing
	**/
	void isrTick_event();

	/*!	\brief Sampling Timer handler
	**	\isr Sampling Timer handler
	**	\return nothing
	**/
	void isrTick_timer();

	/*!	\brief Initialisation routine
	**	\param [in] pin - pin for input capture
	**	\param [in] per - Period of the timer (assuming it represents milliseconds)
	**	\param [in] edge - triggering edge
	**	\return nothing
	**/
	void init(uint8_t pin, uint16_t per, uint8_t edge = FALLING);
	
	/*!	\brief Set a new sampling period
	**	\param [in] per - Period of the timer (assuming it represents milliseconds)
	**	\return nothing
	**/
	void setPeriod(uint16_t per);
	
	/*!	\brief Get ticks count (with dataReady flag set up)
	**	\note if flag cap.dataReady doesn't need to be updated, use xgetTicks() instead
	**	\param [in, out] res - pointer to result of previous acquisition ticks count (uint16 type)
	**	\retval true - new data acquired
	**	\retval false - no new data (new sample pending)
	**/
	boolean getTicks(uint16_t * res);
	
	/*!	\brief Get ticks count SCALED (with dataReady flag set up)
	**	\param [in, out] res - pointer to result of previous acquisition scaled ticks count (uint16 type)
	**	\param [in] scl - result scale
	**	\retval true - new data acquired
	**	\retval false - no new data (new sample pending)
	**/
	boolean getScaledTicks(uint16_t * res, const float scl);
	
	/*!	\brief Get Frequency (with dataReady flag set up)
	**	\param [in, out] res - pointer to result of previous acquisition Frequency (uint16 type)
	**	\retval true - new data acquired
	**	\retval false - no new data (new sample pending)
	**/
	boolean getFreq(uint16_t * res);
	
	/*!	\brief Get the period of the capture timer
	**	\return Period in ms on uint16 type
	**/
	uint16_t getPeriod();
	
	/*!	\brief Is there new data available
	**	\return \b true if new data ready, \b false otherwise
	**/
	boolean isDataReady();
	
	/*!	\brief Get ticks count (without dataReady flag set up)
	**	\warning this inline doesn't update flag cap.dataReady
	**	\return Previous acquisition ticks count on uint16 type
	**/
	uint16_t xgetTicks();
}

#endif /* CaptureTimer_h */

