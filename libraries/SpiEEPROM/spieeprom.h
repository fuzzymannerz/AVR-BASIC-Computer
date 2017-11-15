/*
 * spieeprom.h - library for SPI EEPROM IC's
 * https://bitbucket.org/trunet/spieeprom/
 * 
 * This library is based on code by Heather Dewey-Hagborg
 * available on http://www.arduino.cc/en/Tutorial/SPIEEPROM
 * 
 * by Wagner Sartori Junior <wsartori@gmail.com>
 */

#ifndef SPIEEPROM_h
#define SPIEEPROM_h

#include <Arduino.h>
#include <SPI.h> // relies on arduino SPI library

#define SLAVESELECT 4 // SPI SS Pin
                       // on MEGA2560 should be PIN 53
					   // change it if you want to use another pin

//opcodes
#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2

class SPIEEPROM
{
  private:
	long address;
	byte eeprom_type;
	
	void send_address(long addr);
	void start_write();
	bool isWIP(); // is write in progress?
	
  public:
	SPIEEPROM(); // default to type 0
    SPIEEPROM(byte type); // type=0: 16-bits address
						  // type=1: 24-bits address
						  // type>1: defaults to type 0
						
	void setup();

	void write(long addr, byte data);
	void write(long addr, byte data[], int arrLength);
	//void write(long addr, char data);
	void write(long addr, char data[], int arrLength);
	//void write(long addr, int data);
	//void write(long addr, long data);
	//void write(long addr, float data);
	
	byte  read_byte (long addr);
	//void  read_byte_array (long addr, byte data[]);
	char  read_char (long addr);
	//void  read_char_array (long addr, char data[]);
	//int   read_int  (long addr);
	//long  read_long (long addr);
	//float read_float(long addr);
};

#endif // SPIEEPROM_h
