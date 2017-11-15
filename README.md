# AVR BASIC Computer v0.1
An updated version of https://hackaday.io/project/3537-avr-basic-computer-v01 to work with **Arduino IDE >=1.8**

*Includes libraries: ps2uartKeyboard, SpiEEPROM, TVout, TVoutfonts*

----------
## Usage ##
[Download the zip file of this repo](https://github.com/fuzzymannerz/AVR-BASIC-Computer/archive/master.zip) and unzip into your Arduino directory.
The zip file contains a bootloader for the ATMega 1284P, the Arduino IDE sketch and the relevant libraries.

Open the Arduino IDE and go to *File -> Preferences* and enter `https://mcudude.github.io/MightyCore/package_MCUdude_MightyCore_index.json` as an *Additional Board Manager URL* (you may need to use a comma to seperate them if you already have a URL there) you can then add "*MightyCore*" from the *Boards Manager* window (*Tools -> Boards -> Boards Manager*).

Now you can open the "*AVR_BASIC_Computer_v0.1/AVR_BASIC_Computer_v0.1.ino*" sketch file and then select "*ATMega 1284*" from the board menu (*Tools -> Board*). Then select the "*USBasp*" from the programmer menu (*Tools -> Programmer*) or alternatively "*Arduino as ISP*" if an Arduino is being used.

**Schematic and all other setup instructions are available at:**
**https://hackaday.io/project/3537-avr-basic-computer-v01**

## Issues ##
A warning saying: `AVR_BASIC_Computer_v0.1.ino:1950:25: warning: large integer implicitly truncated to unsigned type [-Woverflow]` may happen during the compile process, however I currently believe it can be safely ignored and does not affect the system.

## Links ##
This project uses code from the following places:

 - https://github.com/BleuLlama/TinyBasicPlus
 - https://hackaday.io/project/3537-avr-basic-computer-v01
 - https://github.com/fakufaku/mighty-1284p

----------


MIT License
-------
Copyright (c) 2012-2017

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
