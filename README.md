# CaptureTimer

[![author](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/SMFSW/a9a2b2a02fda1b33461d53ddfe69d649/raw/auth_SMFSW.json)](https://github.com/SMFSW)
![badge](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/SMFSW/a9a2b2a02fda1b33461d53ddfe69d649/raw/CaptureTimer_status_badge.json)
[![license](https://img.shields.io/badge/License-GNU_LGPL-darkgreen.svg)](LICENSE)
[![CodeFactor](https://www.codefactor.io/repository/github/smfsw/capturetimer/badge)](https://www.codefactor.io/repository/github/smfsw/capturetimer)

![platform](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/SMFSW/a9a2b2a02fda1b33461d53ddfe69d649/raw/platform_INO.json)

[![doxy](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/SMFSW/a9a2b2a02fda1b33461d53ddfe69d649/raw/tool_DOXY.json)](https://smfsw.github.io/CaptureTimer)
[![re_note](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/SMFSW/a9a2b2a02fda1b33461d53ddfe69d649/raw/tool_RN.json)](ReleaseNotes.md)

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
