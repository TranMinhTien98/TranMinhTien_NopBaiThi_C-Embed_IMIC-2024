#include <RTC_DS3231.h>
#include "main.h"
extern I2C_HandleTypeDef hi2c1;
DS3231_Typedef DS3231_Timer;
/*
 * 0b1011 1111
 * The high 4 bits are the first 4 bits: 1011
 * The low 4 bits are the last 4 bits: 1111
 */
static unsigned char Bin_To_Dec(unsigned char data)
{
	unsigned char high,low;
	high = (data >> 4) & 0x0F;
	low = data & 0x0F;
	return ((high * 10) + low);
}
/******************************************************************************/
static unsigned char Dec_To_Bin(unsigned char data)
{
	unsigned char high,low;
	high = ((data / 10) << 4) & 0xF0;
	low = (data % 10) & 0x0F;
	return ((high)|(low));
}
void DS3231_GetDateTime(DS3231_Typedef* Time)
{
	unsigned char cmd = 0x00;
	unsigned char data[7];

	HAL_I2C_Master_Transmit(&hi2c1, DS3231_I2C_ADDR, &cmd, 1, 500);
	HAL_I2C_Master_Receive(&hi2c1, DS3231_I2C_ADDR, data, 7, 500);

	Time->seconds = Bin_To_Dec(data[0] & 0x7F);
	Time->minutes = Bin_To_Dec(data[1] & 0x7F);

	if((data[2] & 0x40) != 0){
	   Time->hours = Bin_To_Dec(data[2] & 0x1F);
	}
	else{
	   Time->hours = Bin_To_Dec(data[2] & 0x3F);
	}

	Time->day = Bin_To_Dec(data[3] & 0x07);
	Time->date = Bin_To_Dec(data[4] & 0x3F);
	Time->month = Bin_To_Dec(data[5] & 0x1F);
	Time->year = Bin_To_Dec(data[6] & 0xFF);
}
void DS3231_SetTime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
  unsigned char data[8];
  data[0] = 0x00;  //CMD DS3231_ADDR_SECOND
  data[1] = Dec_To_Bin(seconds);
  data[2] = Dec_To_Bin(minutes);
  data[3] = Dec_To_Bin(hours);
  data[3] &= ~0x40;
  data[4] = Dec_To_Bin(day);
  data[5] = Dec_To_Bin(date);
  data[6] = Dec_To_Bin(month);
  data[7] = Dec_To_Bin(year);
  //start i2c
  HAL_I2C_Master_Transmit(&hi2c1, DS3231_I2C_ADDR, data, 8, 500);
}
