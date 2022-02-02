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



//void tm1637_init(int clk, int data);
void writeByte(int8_t wr_data);//write 8bit data to TM1637
void start(void);//send start bits
void stop(void); //send stop bits

void TM1637_DisplayMass(int8_t DispData[]);		// send array to TM1637
void TM1637_Display(uint8_t BitAddr,int8_t DispData); // send number to one segment
void TM1637_DisplayDec (int16_t value);	// send DEC number to TM1637
void TM1637_RunningString(int8_t DispData[], uint8_t amount, uint16_t delayMs); // send and scroll string
void TM1637_ClearDisplay(void);	// clear segments TM1637
void TM1637_SetBrightness(uint8_t brightness); // Set Brightness 0-8
void TM1637_Point(uint8_t cmd); // show clock point ":"

void displayByteForString (uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3);

void coding_mass(int8_t DispData[]);
int8_t coding(int8_t DispData);

#endif /* TM1637_H_ */
