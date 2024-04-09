#include "rtc.h"
#include "stm32f4xx_hal_conf.h"
#include "usart2.h"


void init_rtc(void)
{
	uint8_t asynch_factor = 0x7F; // 0111.1111
	uint8_t synch_factor = 0xFF; // 1111.1111 255
	// Low Speed Clock config
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	RCC->CSR |= RCC_CSR_LSION;
	while(!(RCC->CSR & RCC_CSR_LSIRDY)){};
	RCC->BDCR |= 0x8200;

	// enable RTC write
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	// enable init mode
	RTC->ISR |= (1<<ISR_IN_BIT);
	while(!( RTC->ISR & (1<<ISR_INITF_BIT) )){};


	// program first the asynchronous prescaler factor in RTC_PRER register
	RTC->PRER &= ~(RTC_PRER_ASYNC_MASK<<RTC_PRER_ASYNC_LSB);
	RTC->PRER |= asynch_factor<<RTC_PRER_ASYNC_LSB;

	//  program the synchronous prescaler factor
	RTC->PRER &= ~RTC_PRER_SYNC_MASK;
	RTC->PRER |= synch_factor;

	// default time


	RTC->TR &= 0xFF808080;
	RTC->TR |=(0x16<<16) | (0x27<<8) | (0x00<<8);

	// default date

	RTC->DR &= 0xFF0020C0;
	RTC->DR |= (0x24<<16) | (4<<8) | (0x2);

	// disable init mode
	RTC->CR |= RTC_CR_BYPSHAD;
	RTC->ISR &= ~RTC_ISR_INIT;
	PWR->CR &= ~PWR_CR_DBP;

}

void init_rtc_datetime(DateTypeDef date, TimeTypeDef time)
{
	uint8_t asynch_factor = 0x7F; // 0111.1111
	uint8_t synch_factor = 0xFF; // 1111.1111 255
	// Low Speed Clock config
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	RCC->CSR |= RCC_CSR_LSION;
	while(!(RCC->CSR & RCC_CSR_LSIRDY)){};
	RCC->BDCR |= 0x8200;

	// enable RTC write
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	// enable init mode
	RTC->ISR |= (1<<ISR_IN_BIT);
	while(!( RTC->ISR & (1<<ISR_INITF_BIT) )){};


	// program first the asynchronous prescaler factor in RTC_PRER register
	RTC->PRER &= ~(RTC_PRER_ASYNC_MASK<<RTC_PRER_ASYNC_LSB);
	RTC->PRER |= asynch_factor<<RTC_PRER_ASYNC_LSB;

	//  program the synchronous prescaler factor
	RTC->PRER &= ~RTC_PRER_SYNC_MASK;
	RTC->PRER |= synch_factor;

	// default time
	uint8_t hour = (((time.Hours / 10) & 0x3) << 4) | ((time.Hours % 10) & 0xF);
	uint8_t minutes = (((time.Minutes / 10) & 0x7) << 4) | ((time.Minutes % 10) & 0xF);
	uint8_t seconds = (((time.Seconds / 10) & 0x7) << 4) | ((time.Seconds % 10) & 0xF);

	RTC->TR &= 0xFF808080;
	RTC->TR |=(hour<<16) | (minutes<<8) | seconds;

	// default date
	uint8_t year = (((date.Year / 10) & 0xF) << 4) | ((date.Year % 10) & 0xF);
	uint8_t month = (((date.Month / 10) & 0x1) << 4) | ((date.Month % 10) & 0xF);
	uint8_t day = (((date.Date / 10) & 0x3) << 4) | ((date.Date % 10) & 0xF);

	RTC->DR &= 0xFF0020C0;
	RTC->DR |= (year<<16) | (month<<8) | (day);

	// disable init mode
	RTC->CR |= RTC_CR_BYPSHAD;
	RTC->ISR &= ~RTC_ISR_INIT;
	PWR->CR &= ~PWR_CR_DBP;

}



void get_date(DateTypeDef *date){

	date->Year = (((RTC->DR >> 20)&0xF)*10) + ((RTC->DR >> 16)&0xF);
	date->Month = (10*((RTC->DR >> 12)&1)) + ((RTC->DR >> 8)&0xF);
	date->Date = (10*((RTC->DR >> 4)&3)) + (RTC->DR & 0xF);
}

void get_time(TimeTypeDef *time){
	time->Seconds = (RTC->TR & 0xF) + 10*((RTC->TR >> 4)&7);
	time->Minutes = ((RTC->TR >> 8)&0xF) + 10*((RTC->TR >> 12)&7);
	time->Hours = ((RTC->TR >> 16)&0xF) + 10*(((RTC->TR >> 20)&3));
}

/*
//afficher la date
void print_date(void){
	serial_puts("Date: ");
    date_result = int2string(sDate.Date, buffer_date);
    serial_puts(date_result);
    serial_puts("-");
    date_result = int2string(sDate.Month, buffer_date);
    serial_puts(date_result);
    serial_puts("-");
    date_result = int2string(sDate.Year, buffer_date);
    serial_puts(date_result);
    newLine();
}
//afficher l'heure
void print_time(void){
	serial_puts("Time: ");
	time_result = int2string(sTime.Hours, buffer_time);
	serial_puts(time_result);
	serial_puts(":");
	time_result = int2string(sTime.Minutes, buffer_time);
	serial_puts(time_result);
	serial_puts(":");
	time_result = int2string(sTime.Seconds, buffer_time);
	serial_puts(time_result);
	newLine();
}
*/



