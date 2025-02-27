# WaterRino

## Overview

A DIY and open source approach to the [Nohrd WaterRower](https://www.nohrd.com/us/waterrower/) using Arduino and spare parts easily available. This project consists of two parts:

1. [WaterRino](https://github.com/adruino-io/waterrino/blob/master/WaterRino.ino) (Ardunio rowing monitor)
2. [DIY rower](https://github.com/adruino-io/waterrino/blob/master/WaterRino_german.pdf)

## WaterRino

The [arduino code](https://github.com/adruino-io/waterrino/blob/master/WaterRino.ino) is derived from the [yun_rower](https://bitbucket.org/giobianchi/yun_rower/src/master/) project by Giordano Bianchi.

Improvements include:
* [moving average](https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningAverage
) (gliding)
* utilising LCD 1602 (16x2) Keypad Shield for variable output (strokes (per minute/average (overall/gliding)), rpm, total strokes, etc.)
* migrating to Arduino Uno

Future changes will include:
- [ ] Storing data (rotations/strokes over time) on SD-card (already possible via serial)
- [ ] Precise adjustment for strokes:meters travelled ratio
- [ ] Pause/Interrupt function
- [ ] Cleaning of code-base (original project was merged for ease of editing)

### DIY rower

The DIY rower is derived from the Nohrd WaterRower (similar dimensions) and uses the same mode of action for generating resistance (water tank). The construction manual can be found [here](https://github.com/adruino-io/waterrino/blob/master/WaterRino_german.pdf).

Future changes will include:
- [ ] Uploading translated version of the construction manual
- [ ] Replacing Nohrd spare parts with off-the-shelf/DIY components
- [ ] Increasing sturdiness of design (e.g. mounting tank to top assembly)
- [ ] Replacing hand drawn notes in the assembly instruction
