/*
 * TM1637.c
 *
 *  Created on: 2 февр. 2022 г.
 *      Author: Bort Serg
 */

#include "delay_micros.h"
#include "tm1637.h"

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
#define _A 0x77, //A 10
#define _B 0x7c, //B 11
#define _C 0x39, //C 12
#define _D 0x5e, //d 13
#define _E 0x79, //E 14
#define _F 0x71, //F 15
#define _G 0x3d, //G 16
#define _H 0x76, //H 17
#define _h 0x74, //h 18
#define _I 0x30, //I 19
#define _i 0x10, //i 20
#define _J 0x1e, //J 21
#define _j 0x0c, //j 22
#define _L 0x38, //L 23
#define _l 0x18, //l 24
#define _M 0x37, //M 25
#define _n 0x54, //n 26
#define _o 0x5c, //o 27
#define _P 0x73, //P 28
#define _r 0x50, //r 29
#define _t 0x78, //t 30
#define _U 0x3e, //U 31
#define _u 0x1c, //u 32
#define _Y 0x6E, //Y 33
#define _dash 0x40, // "-"    34
#define _degree 0x63, // градус 35
#define _under 0x08, // "_"	    36
#define _empty 0x00 //пробел 37
//============================================================================

uint8_t Cmd_DispCtrl = 0;
uint8_t point_flag = 0;
uint8_t BlankingFlag = 1;
static int8_t TubeTab[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9



void writeByte(int8_t wr_data)
{
  uint8_t count = 0;

  for(uint8_t i = 0; i < 8; i++)
  {
	  HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_RESET);

    if(wr_data & 0x01)
    {
    	HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_SET);
    }
    else
    	HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_RESET);

    delay_micros(DELAY);
    wr_data >>= 1;
    delay_micros(DELAY);
    HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_SET);
    delay_micros(DELAY);
  }

  HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_RESET);
  delay_micros(DELAY);
  HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_SET);;
  delay_micros(DELAY);
  HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_SET);;
  delay_micros(DELAY);

  while(HAL_GPIO_ReadPin(DIO_PORT, DIO_PIN))
  {
    count += 1;

    if(count == 200)
    {
    	HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_RESET);;
    	count = 0;
    }
  }
}

void start(void)
{
	HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_SET);
	delay_micros(DELAY);
	HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_SET);
	delay_micros(DELAY);
	HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_RESET);
	delay_micros(DELAY);
	HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_RESET);
}

void stop(void)
{
	HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_RESET);
	delay_micros(DELAY);
	HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_RESET);
	delay_micros(DELAY);
	HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_SET);
	delay_micros(DELAY);
	HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_SET);
}

void TM1637_Display(uint8_t BitAddr, int8_t DispData)
{
	int8_t SegData;

	SegData = coding(DispData);
	start();
	writeByte(ADDR_FIXED);
	stop();

	start();
	writeByte(BitAddr | startADDR);
	writeByte(SegData);
	stop();

	start();
	writeByte(Cmd_DispCtrl);
	stop();
}

void TM1637_DisplayArray(int8_t DispData[])
{
	int8_t SegData[4];

	for(uint8_t i = 0; i < 4; i++)
	{
		SegData[i] = DispData[i];
	}

	coding_Array(SegData);

	start();
	writeByte(ADDR_AUTO);
	stop();

	start();
	writeByte(startADDR);
	for(uint8_t i = 0; i < 4; i++)
	{
		writeByte(SegData[i]);
	}
	stop();

	start();
	writeByte(Cmd_DispCtrl);
	stop();
}

void TM1637_DisplayLetter (uint8_t BitAddr, int8_t DispData)
{
	int8_t SegData;

	SegData = DispData;
	start();
	writeByte(ADDR_FIXED);
	stop();

	start();
	writeByte(BitAddr | startADDR);
	writeByte(SegData);
	stop();

	start();
	writeByte(Cmd_DispCtrl);
	stop();
}

void TM1637_DisplayLetterArray (uint8_t LetterArray[])
{
	start();
	writeByte(ADDR_AUTO);
	stop();

	start();
	writeByte(startADDR);
	for(uint8_t i = 0; i < 4; i++)
	{
		writeByte(LetterArray[i]);
	}
	stop();

	start();
	writeByte(Cmd_DispCtrl);
	stop();

}

void TM1637_DisplayDec (int16_t value)
{
	uint8_t tempArr[4] = {0,0,0,0};

	if (value > 9999) return;
	if (value < -999) return;

	if (value >= 0)
	{
		tempArr[0] = value / 1000;
		value %= 1000;
		tempArr[1] = value / 100;
		value %= 100;
		tempArr[2] = value / 10;
		tempArr[3] = value % 10;

		if(tempArr[0] == 0)
	    {
			tempArr[0] = CLEAR_SEG;
			if(tempArr[1] == 0)
			{
				tempArr[1] = CLEAR_SEG;
				if(tempArr[2] == 0)
				{
					tempArr[2] = CLEAR_SEG;
				}
			}
		}
	}

	if (value < 0)
	{
		value = abs(value);
		tempArr[0] = MINUS;
		tempArr[1] = value / 100;
	    value %= 100;
	    tempArr[2] = value / 10;
		tempArr[3] = value %= 10;

	    if(tempArr[1] == 0)
		{
	    	tempArr[1] = CLEAR_SEG;
		    if(tempArr[2] == CLEAR_SEG)
		    {
		    	tempArr[2] = CLEAR_SEG;
		    }
		}

	}
	TM1637_DisplayArray(tempArr);
}

void TM1637_RunningString(int8_t DispData[], uint8_t amount, uint16_t delayMs)
{
	  uint8_t segm_data[amount + 8];

	  for (uint8_t i = 0; i < 4; i++)
	  {
	    segm_data[i] = CLEAR_SEG;
	  }

	  for (uint8_t i = 0; i < amount; i++)
	  {
	    segm_data[i + 4] = DispData[i];
	  }

	  for (uint8_t i = amount + 4; i < amount + 8; i++)
	  {
	    segm_data[i] = CLEAR_SEG;
	  }

	  for (uint8_t i = 0; i <= amount + 4; i++)
	  {
	    displayByteForString (segm_data[i], segm_data[i + 1], segm_data[i + 2], segm_data[i + 3]);
	    HAL_Delay(delayMs);
	  }
}

void displayByteForString (uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3)
{
	uint8_t dispArray[] = {bit0, bit1, bit2, bit3};
	TM1637_DisplayArray(dispArray);
}


void TM1637_RunningLetterString (int8_t DispData[], uint8_t amount, uint16_t delayMs)
{
	  uint8_t segm_data[amount + 8];

	  for (uint8_t i = 0; i < 4; i++)
	  {
	    segm_data[i] = 0x00;
	  }

	  for (uint8_t i = 0; i < amount; i++)
	  {
	    segm_data[i + 4] = DispData[i];
	  }

	  for (uint8_t i = amount + 4; i < amount + 8; i++)
	  {
	    segm_data[i] = 0x00;
	  }

	  for (uint8_t i = 0; i <= amount + 4; i++)
	  {
	    displayByteForLetter (segm_data[i], segm_data[i + 1], segm_data[i + 2], segm_data[i + 3]);
	    HAL_Delay(delayMs);
	  }
}

void displayByteForLetter(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3)
{
	uint8_t dispArray[] = {bit0, bit1, bit2, bit3};
	TM1637_DisplayLetterArray(dispArray);
}


void TM1637_ClearDisplay(void)
{
	TM1637_Display(0x00, CLEAR_SEG);
	TM1637_Display(0x01, CLEAR_SEG);
	TM1637_Display(0x02, CLEAR_SEG);
	TM1637_Display(0x03, CLEAR_SEG);
}

void TM1637_SetBrightness(uint8_t brightness)
{
	Cmd_DispCtrl = 0x88 + brightness;
}

void TM1637_Point(uint8_t cmd)
{
	if(cmd == 0)
	{
		point_flag = (~point_flag) & 0x01;
	}
	else
		point_flag = 1;
}

void coding_Array(int8_t DispData[])
{
	uint8_t PointData;

	if(point_flag == 1)
	{
		PointData = DISP_CONTROL;
	}
	else
		PointData = 0;

	for(uint8_t i = 0; i < 4; i++)
	{
		if(DispData[i] == CLEAR_SEG)
		{
			DispData[i] = 0x00;
		}
		else
			DispData[i] = TubeTab[DispData[i]] + PointData;
	}
}

int8_t coding(int8_t DispData)
{
	uint8_t PointData;

	if(point_flag == 1)
	{
		PointData = DISP_CONTROL;
	}
	else
		PointData = 0;

	if(DispData == CLEAR_SEG)
	{
		DispData = 0x00 + PointData;
	}
	else
		DispData = TubeTab[DispData] + PointData;

	return DispData;
}
