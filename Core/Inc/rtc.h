#ifndef RTC_H_
#define RTC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

/*extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;
extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart2;
*/

#define ISR_IN_BIT 7
#define ISR_INITF_BIT 6
#define RTC_PRER_ASYNC_LSB 16
#define RTC_PRER_SYNC_MSB 14
#define RTC_PRER_SYNC_MASK 0x7FFF
#define RTC_PRER_ASYNC_MASK 0x7F
#define RTC_CR_FMT_MASK 6


typedef struct {
	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
} DateTypeDef;

typedef struct{
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
} TimeTypeDef;

/*void print_date(void);
void print_time(void);
*/
void get_date(DateTypeDef *date);
void get_time(TimeTypeDef *time);

void init_rtc(void);
void init_rtc_datetime(DateTypeDef date, TimeTypeDef time);



#endif


