/*
 * TM1637.c
 *
 *  Created on: 2 февр. 2022 г.
 *      Author: Bort Serg
 */

#include "delay_micros.h"
#include "tm1637.h"

uint8_t Cmd_DispCtrl = 0;
uint8_t point_flag = 0;
uint8_t BlankingFlag = 1;

static int8_t TubeTab[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x40}; //0~9, A(10), b(11), -(12)

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

void TM1637_DisplayMass(int8_t DispData[])
{
	int8_t SegData[4];

	for(uint8_t i = 0; i < 4; i++)
	{
		SegData[i] = DispData[i];
	}

	coding_mass(SegData);

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
	TM1637_DisplayMass(tempArr);
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
	TM1637_DisplayMass(dispArray);
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

void coding_mass(int8_t DispData[])
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
