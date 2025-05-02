# CaptureTimer [![CodeFactor](https://www.codefactor.io/repository/github/smfsw/capturetimer/badge)](https://www.codefactor.io/repository/github/smfsw/capturetimer)

Arduino Input Capture Library

This library is intended to attach interrupt on a pin for frequency/time sampling.

## Notes

* use of MsTimer2 library for AVR architecture
* use of DueTimer library for SAM architecture: Timer0 (-> needs to be reworked if other arm core)
* use of NONE library for ESP8266 architecture

## Usage

refer to Doxygen generated documentation & example sketches

## Examples included

* [SimpleCapture.ino](examples/SimpleCapture/SimpleCapture.ino): Capture signal and show result on serial
* [CaptureToAnalogic.ino](examples/CaptureToAnalogic/CaptureToAnalogic.ino): Capture signal and output scaled results to DAC / PWM (depending board resources)
* [TriggeredTickCapture.ino](examples/TriggeredTickCapture/TriggeredTickCapture.ino): Using interrupt-able pin for Tick time capture using output pin to generate ticks

## Documentation

Doxygen doc can be generated using "Doxyfile".

See [generated documentation](https://smfsw.github.io/CaptureTimer/)

## Release Notes

See [release notes](ReleaseNotes.md)
