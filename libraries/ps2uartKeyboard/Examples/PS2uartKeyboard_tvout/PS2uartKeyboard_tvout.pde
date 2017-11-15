/* 
 based on pollserial and PS2Keyboard.
 Combined by Yotson, october 2011
   
 ps2 keyboard library which uses the usart hardware for buffering individual bits into a byte before reading the byte by software.
 Goal was to use a ps2 keyboard in combination with tv-out without image/timing corruption. 
 The interrupt driven version of ps2 keyboard messed with tv out image/timing.
    
 Tested on atmega328. Not compatible with arduino mega/mega2560 as the XCKn pins are not broken out to header/pad.
   
 ps2-Keyboard pin   |  Arduino pin
 -------------------------------------------
 data               |  RX, digital pin 0
 clock              |  XCK, digital pin 4
 ground             |  Ground
 5V                 |  5V   
*/

#define DEBUG               // comment out to disable

#ifdef DEBUG
#include <NewSoftSerial.h>    // this _will_ mess with tvout timings. Used for debugging.
NewSoftSerial mySerial(11, 12);
#endif

#include <TVout.h>
#include <fontALL.h>
#include <PS2uartKeyboard.h>

TVout TV;
PS2uartKeyboard keyboard;

void setup()  {
  TV.begin(_NTSC);
  TV.set_hbi_hook(keyboard.begin()); 
  
#ifdef DEBUG
  mySerial.begin(14400);
  mySerial.println("Hello....? World?");
#else
  TV.select_font(font6x8);
  TV.println("Keyboard Terminal");
  TV.println("-- Version 0.1 --");
#endif
}

void loop() {
  if (keyboard.available()) {
#ifdef DEBUG
    mySerial.print((char)keyboard.read());
#else
    TV.print((char)keyboard.read());
#endif
  }
}


