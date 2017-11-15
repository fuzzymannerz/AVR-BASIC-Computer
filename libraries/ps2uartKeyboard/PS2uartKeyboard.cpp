/* 
 based on pollserial.cpp and PS2Keyboard.cpp
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
/*
 pollserial.cpp Heavily modified version of:
 
 HardwareSerial.cpp - Hardware serial library for Wiring
 Copyright (c) 2006 Nicholas Zambetti.  All right reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 Modified 23 November 2006 by David A. Mellis
 
 Modified July 2010 by Myles D. Metzler
*/
/*
 PS2Keyboard.cpp - PS2Keyboard library
 Copyright (c) 2007 Free Software Foundation.  All right reserved.
 Written by Christian Weichel <info@32leaves.net>
 
 ** Mostly rewritten Paul Stoffregen <paul@pjrc.com> 2010, 2011
 ** Modified for use beginning with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
 ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **
 
 for more information you can read the original wiki in arduino.cc
 at http://www.arduino.cc/playground/Main/PS2Keyboard
 or http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html
 
 Version 2.1 (May 2011)
 - timeout to recover from misaligned input
 - compatibility with Arduino "new-extension" branch
 - TODO: send function, proposed by Scott Penrose, scooterda at me dot com
 
 Version 2.0 (June 2010)
 - Buffering added, many scan codes can be captured without data loss
 if your sketch is busy doing other work
 - Shift keys supported, completely rewritten scan code to ascii
 - Slow linear search replaced with fast indexed table lookups
 - Support for Teensy, Arduino Mega, and Sanguino added
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "PS2uartKeyboard.h"

#include <avr/pgmspace.h>

#define UARTBUFFER_SIZE 45
static uint8_t buffer[UARTBUFFER_SIZE];
static uint8_t head, tail;
static char ps2Keyboard_CharBuffer=0;

void USART_recieve() {
#if defined ( UDR0 )
  if( UCSR0A & _BV(RXC0)) {
    uint8_t i = head + 1;
    if (i >= UARTBUFFER_SIZE) i = 0;
    if (i != tail) {
      buffer[i] = UDR0;
      head = i;
    }
  }
#else
  if( UCSRA & _BV(RXC)) {
    uint8_t i = head + 1;
    if (i >= UARTBUFFER_SIZE) i = 0;
    if (i != tail) {
      buffer[i] = UDR;
      head = i;
    }
  }
#endif
}

static inline uint8_t get_scan_code(void)
{
  uint8_t c, i;

  i = tail;
  if (i == head) return 0;
  i++;
  if (i >= UARTBUFFER_SIZE) i = 0;
  c = buffer[i];
  tail = i;
  return c;
}

// http://www.quadibloc.com/comp/scan.htm
// http://www.computer-engineering.org/ps2keyboard/scancodes2.html
#define BREAK     0x01
#define MODIFIER  0x02
#define SHIFT_L   0x04
#define SHIFT_R   0x08

// These arrays provide a simple key map, to turn scan codes into ASCII
// output.  If a non-US keyboard is used, these may need to be modified
// for the desired output.
static unsigned char __attribute((section(".progmem.data"))) scan2ascii_noshift[] = {
  0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
  0, 0, 'z', 's', 'a', 'w', '2', 0,
  0, 'c', 'x', 'd', 'e', '4', '3', 0,
  0, ' ', 'v', 'f', 't', 'r', '5', 0,
  0, 'n', 'b', 'h', 'g', 'y', '6', 0,
  0, 0, 'm', 'j', 'u', '7', '8', 0,
  0, ',', 'k', 'i', 'o', '0', '9', 0,
  0, '.', '/', 'l', ';', 'p', '-', 0,
  0, 0, '\'', 0, '[', '=', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '\\', 0, 0,
  0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
  0, '1', 0, '4', '7', 0, 0, 0,
  '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 };

static unsigned char __attribute((section(".progmem.data"))) scan2ascii_shift[] = {
  0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
  0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '~', 0,
  0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
  0, 0, 'Z', 'S', 'A', 'W', '@', 0,
  0, 'C', 'X', 'D', 'E', '$', '#', 0,
  0, ' ', 'V', 'F', 'T', 'R', '%', 0,
  0, 'N', 'B', 'H', 'G', 'Y', '^', 0,
  0, 0, 'M', 'J', 'U', '&', '*', 0,
  0, '<', 'K', 'I', 'O', ')', '(', 0,
  0, '>', '?', 'L', ':', 'P', '_', 0,
  0, 0, '"', 0, '{', '+', 0, 0,
  0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '}', 0, '|', 0, 0,
  0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
  0, '1', 0, '4', '7', 0, 0, 0,
  '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
  PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
  0, 0, 0, PS2_F7 };

static char get_ascii_code(void)
{
  static uint8_t state=0;
  uint8_t s;
  char c;

  while (1) {
    s = get_scan_code();

    if (!s) return 0;
    if (s == 0xF0) {
      state |= BREAK;
    } 
    else if (s == 0xE0) {
      state |= MODIFIER;
    } 
    else {
      if (state & BREAK) {
        if (s == 0x12) {
          state &= ~SHIFT_L;
        } 
        else if (s == 0x59) {
          state &= ~SHIFT_R;
        }
        // CTRL, ALT & WIN keys could be added
        // but is that really worth the overhead?
        state &= ~(BREAK | MODIFIER);
        continue;
      }
      if (s == 0x12) {
        state |= SHIFT_L;
        continue;
      } 
      else if (s == 0x59) {
        state |= SHIFT_R;
        continue;
      }
      c = 0;
      if (state & MODIFIER) {
        switch (s) {
        case 0x70: 
          c = PS2_INSERT;      
          break;
        case 0x6C: 
          c = PS2_HOME;        
          break;
        case 0x7D: 
          c = PS2_PAGEUP;      
          break;
        case 0x71: 
          c = PS2_DELETE;      
          break;
        case 0x69: 
          c = PS2_END;         
          break;
        case 0x7A: 
          c = PS2_PAGEDOWN;    
          break;
        case 0x75: 
          c = PS2_UPARROW;     
          break;
        case 0x6B: 
          c = PS2_LEFTARROW;   
          break;
        case 0x72: 
          c = PS2_DOWNARROW;   
          break;
        case 0x74: 
          c = PS2_RIGHTARROW;  
          break;
        case 0x4A: 
          c = '/';             
          break;
        case 0x5A: 
          c = PS2_ENTER;       
          break;
        default: 
          break;
        }
      } 
      else if (state & (SHIFT_L | SHIFT_R)) {
        if (s < sizeof(scan2ascii_shift))
          c = pgm_read_byte(scan2ascii_shift + s);
      } 
      else {
        if (s < sizeof(scan2ascii_noshift))
          c = pgm_read_byte(scan2ascii_noshift + s);
      }
      state &= ~(BREAK | MODIFIER);
      if (c) return c;
    }
  }
}

bool PS2uartKeyboard::available(){
  if (ps2Keyboard_CharBuffer) return true;
  ps2Keyboard_CharBuffer = get_ascii_code();
  if (ps2Keyboard_CharBuffer) return true;
  return false;
}

int PS2uartKeyboard::read(){
  char result = ps2Keyboard_CharBuffer;
  if (result) {
    ps2Keyboard_CharBuffer = 0;
  } 
  else {
    result = get_ascii_code();
  }
  if (!result) return -1;
  return result;
}

void PS2uartKeyboard::flush(){
  head = tail;
}

pt2Funct PS2uartKeyboard::begin(void) {
  uint16_t baud_setting;
  bool use_u2x;
  long baud = 19200L;

  // U2X mode is needed for baud rates higher than (CPU Hz / 16)
  if (baud > F_CPU / 16) {
    use_u2x = true;
  }
  else {
    // figure out if U2X mode would allow for a better connection

    // calculate the percent difference between the baud-rate specified and
    // the real baud rate for both U2X and non-U2X mode (0-255 error percent)
    uint8_t nonu2x_baud_error = abs((int)(255-((F_CPU/(16*(((F_CPU/8/baud-1)/2)+1))*255)/baud)));
    uint8_t u2x_baud_error = abs((int)(255-((F_CPU/(8*(((F_CPU/4/baud-1)/2)+1))*255)/baud)));

    // prefer non-U2X mode because it handles clock skew better
    use_u2x = (nonu2x_baud_error > u2x_baud_error);
  }
  if (use_u2x) {
#if defined ( UDR0 )
    UCSR0A = _BV(U2X0);
#else
    UCSRA = _BV(U2X);
#endif
    baud_setting = (F_CPU / 4 / baud - 1) / 2;
  }
  else {
#if defined ( UDR0 )
    UCSR0A = 0;
#else
    UCSRA = 0;
#endif
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }
  // ps2 requirements
  // modify settings of serial1
  // 1 start bit, 8 data bits, 1 parity bit (odd parity), 1 stop bit.

  // UCSRnC
  // UMSELn1 UMSELn0
  // 0       0      Asynchronous USART
  // 0       1      Synchronous USART     <-  atmega 328/168, XCK, pin6, Digital pin4 arduino with atmega328
  // 1       0      (Reserved)
  // 1       1      Master SPI (MSPIM)

  // UCSRnC
  // UPMn1 UPMn0 
  // 0    0    Disabled
  // 0    1    Reserved
  // 1    0    Enabled, Even Parity
  // 1    1    Enabled, Odd Parity     <-

  // assign the baud_setting, a.k.a. (USART Baud Rate Register)
#if defined ( UDR0 )
  UBRR0 = baud_setting;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  bitSet(UCSR0C, UMSEL00);
  bitSet(UCSR0C, UPM01);
  bitSet(UCSR0C, UPM00);
#else
  UBRR = baud_setting;
  UCSRB = _BV(RXEN) | _BV(TXEN);
  bitSet(UCSRC, UMSEL0);
  bitSet(UCSRC, UPM1);
  bitSet(UCSRC, UPM0);
#endif

  head = 0;
  tail = 0;
  return &USART_recieve;
}