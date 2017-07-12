# CaptureTimer

Arduino Input Capture Library

This library is intended to attach interrupt on a pin for frequency/time sampling.

## Notes

* use of MsTimer2 library for AVR architecture
* use of DueTimer library for SAM architecture: Timer0 (-> needs to be reworked if other arm core)
* use of NONE library for ESP8266 architecture

## Usage

refer to Doxygen generated documentation & example sketches

## Examples included

SimpleCapture.ino: Capture signal and show result on serial
CaptureToAnalogic.ino: Capture signal and output scaled results to DAC / PWM (depending board ressources)
TriggeredTickCapture.ino: Using interrupt-able pin for Tick time capture using output pin to generate ticks

## Misc

Doxygen doc can be generated for the class using doxyfile.

Feel free to share your thoughts @ xgarmanboziax@gmail.com about:

- issues encountered
- optimisations
- improvements & new functionalities
