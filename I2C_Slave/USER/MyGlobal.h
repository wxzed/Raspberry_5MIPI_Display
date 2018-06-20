#ifndef __MYGLOBAL_H
#define __MYGLOBAL_H 
#include "stdint.h"
#define ReadTouch1				1
#define ReadTouch2				2
#define ReadTouch3				3
#define ReadTouch4				4
#define ReadTouch5				5
typedef struct
{
	uint8_t xLow[5];
	uint8_t xHigh[5];
	uint8_t yLow[5];
	uint8_t yHigh[5];
	uint8_t finger[5];
	uint8_t point;
	uint8_t event;
}TouchData;
extern TouchData MyTouchData;
#endif
