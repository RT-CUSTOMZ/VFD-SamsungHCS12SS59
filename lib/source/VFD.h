// VFD.h, vacuum fluroescent display driver for Samsung HCS-12SS59T, V0.9 171112 qrt@qland.de

#ifndef _VFD_h
#define _VFD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h> // 12 MISO, 13 MOSI, 14 SCK, 15 SS

#define SPIPARS 2000000, LSBFIRST, SPI_MODE3
#define SUPPLYMODE 0 // 0 Vdisp, 1 supply clock
#define SUPPLYCYC 62 // 16E3^-1 / 2 / (16E6^-1 * DIV8)

#define VFD_DCRAM_WR 0x10 // ccccaaaa dddddddd dddddddd ..
#define VFD_CGRAM_WR 0x20 // "
#define VFD_ADRAM_WR 0x30 // ccccaaaa ******dd ******dd ..
#define VFD_DUTY 	 0x50	  	// ccccdddd
#define VFD_NUMDIGIT 0x60 		// "
#define VFD_LIGHTS   0x70	  	// cccc**dd

#define LINORM 0x00 // normal display
#define LIOFF 0x01	// lights OFF
#define LION 0x02	//        ON

#define NUMDIGITS 12 // display digit width
#define BUFSIZE 120	 // display/scroll buffer


const int VFD_VDON = 2;	  // _VDON Vdisp 0=ON 1=OFF

const int VFD_MISO = 0;
const int VFD_RESET = 12; // VFD _RESET
const int VFD_MOSI = 13;	// VFD data
const int VFD_SCK = 14;	// VFD clock
const int VFD_CS = 15;	  // _CS chip select, SPI SS

const int Pin_VFD_SDA = 4; // I2C-data
const int Pin_VFD_SCL = 5; // I2C-clck

const int WAKE_UP = 16; // 

class VFD
{
public:
	int16_t scrLen;
	int16_t scrPos;
	int16_t scrMode;

	void init();
	void supplyOn();
	void supplyOff();
	void write(char *text);
	void scroll(int16_t mode);
	void display();
	void brightness(int light);

protected:
private:
	char buf[BUFSIZE];

	void select(int pin);
	void deSelect(int pin);
	void sendCmd(char cmd, char arg);
	void sendCmdSeq(char cmd, char arg);
	void sendChar(char c);
	char getCode(char c);
};

extern VFD Vfd;

#endif
