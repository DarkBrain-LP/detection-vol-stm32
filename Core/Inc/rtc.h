#ifndef RTC_H_
#define RTC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart2;


uint8_t maxDaysInMonth(uint8_t month, uint8_t year);
uint8_t isLeapYear(uint16_t year);
void actualise_time(void);
void print_date(void);
void print_time(void);



void SystemClock_Config(void);
/*static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_GPIO_Init(void);
void Error_Handler(void);
*/
#endif


