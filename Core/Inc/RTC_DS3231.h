#ifndef _DS3231_H_
#define _DS3231_H_

#include "stdint.h"


#define DS3231_I2C_ADDR       0xD0
//Registers location
#define DS3231_ADDR_SECOND    0x00
#define DS3231_ADDR_MINUTE    0x01
#define DS3231_ADDR_HOUR      0x02
#define DS3231_ADDR_DAY       0x03
#define DS3231_ADDR_DATE      0x04
#define DS3231_ADDR_MONTH     0x05
#define DS3231_ADDR_YEAR      0x06
#define DS3231_ADDR_CONTROL   0x07

typedef enum{
	MODE_12H,
	MODE_24H
}Mode_enum;

typedef struct
{
   uint8_t seconds;   //Seconds,          00-59
   uint8_t minutes;   //Minutes,          00-59
   uint8_t hours;     //Hours,            00-23
   uint8_t day;       //Day in a week,    01-07
   uint8_t date;      //Day in a month,   01-31
   uint8_t month;     //Month,            01-12
   uint8_t year;      //Year,             00-99
} DS3231_Typedef;

extern DS3231_Typedef DS3231_Timer;

uint8_t RTC_CheckMinMax(uint8_t val, uint8_t min, uint8_t max);

void DS3231_GetDateTime(DS3231_Typedef* Time);
void DS3231_SetTime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year);


#endif
