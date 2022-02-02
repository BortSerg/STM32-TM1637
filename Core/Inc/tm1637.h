/*
 * TM1637.h
 *
 *  Created on: 2 февр. 2022 г.
 *      Author: Bort Serg
 */

#ifndef TM1637_H_
#define TM1637_H_

#include "main.h"

#define CLK_PIN      GPIO_PIN_6
#define CLK_PORT     GPIOB
#define DIO_PIN      GPIO_PIN_7
#define DIO_PORT     GPIOB


#define ADDR_AUTO    0x40
#define ADDR_FIXED   0x44
#define DISP_CONTROL 0x80
#define startADDR    0xc0

#define DELAY        5
#define MINUS        0x0C
#define CLEAR_SEG    0x7f

//=====================================================================
//Letter defines for TM1637_DisplayLetter of TM1637_DisplayLetterArray
#define _0 0x3f //0
#define _1 0x06 //1
#define _2 0x5b //2
#define _3 0x4f //3
#define _4 0x66 //4
#define _5 0x6d //5
#define _6 0x7d //6
#define _7 0x07 //7
#define _8 0x7f //8
#define _9 0x6f //9
#define A 119 //A 10

#define B 0x7c //B 11

#define C 0x39 //C 12
#define D 0x5e //d 13
#define E 0x79 //E 14
#define F 0x71 //F 15
#define G 0x3d //G 16
#define H 0x76 //H 17
#define h 0x74 //h 18
#define I 0x30 //I 19
//#define _i 0x10 //i 20
#define J 0x1e //J 21
//#define _j 0x0c //j 22
#define L 0x38 //L 23
#define l 0x18 //l 24
#define M 0x37 //M 25
#define n 0x54 //n 26
#define o 0x5c //o 27
#define P 0x73 //P 28
#define r 0x50 //r 29
#define t 0x78 //t 30
#define U 0x3e //U 31
#define u 0x1c //u 32
#define Y 0x6E //Y 33
//#define _dash 0x40 // "-"    34
#define _degree 0x63 // градус 35
#define _under 0x08 // "_"	    36
#define _empty 0x00 //пробел 37

//============================================================================


//void tm1637_init(int clk, int data);
void writeByte(int8_t wr_data);//write 8bit data to TM1637
void start(void);//send start bits
void stop(void); //send stop bits

void TM1637_Display(uint8_t BitAddr,int8_t DispData); // send number to one segment
void TM1637_DisplayArray(int8_t DispData[]);		// send array to TM1637
void TM1637_DisplayLetter (uint8_t BitAddr, int8_t DispData);
void TM1637_DisplayLetterArray (uint8_t LetterArray[]);
void TM1637_DisplayDec (int16_t value);	// send DEC number to TM1637
void TM1637_RunningString(int8_t DispData[], uint8_t amount, uint16_t delayMs); // send and scroll string
void TM1637_RunningLetterString (int8_t DispData[], uint8_t amount, uint16_t delayMs);

void TM1637_ClearDisplay(void);	// clear segments TM1637
void TM1637_SetBrightness(uint8_t brightness); // Set Brightness 0-8
void TM1637_Point(uint8_t cmd); // show clock point ":"

void displayByteForString (uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3);
void displayByteForLetter(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3);

void coding_Array(int8_t DispData[]);
int8_t coding(int8_t DispData);

#endif /* TM1637_H_ */
