#include "stm32f4xx_hal.h"
#include "NOY_rtc.h"


RTC_HandleTypeDef hrtc;

void rtc_init_configure(){
	uint8_t asynch_factor = 0x7F; // 0111.1111
	uint8_t synch_factor = 0xFF; // 1111.1111 255
	//Set INIT bit to 1 in the RTC_ISR register to enter initialization mode
	hrtc.Instance->ISR &= ~(1<<ISR_IN_BIT);
	hrtc.Instance->ISR |= (1<<ISR_IN_BIT);

	// Poll INITF bit of in the RTC_ISR register. The initialization phase mode is entered when INITF is set to 1. It takes from 1 to 2 RTCCLK clock cycles (due to clock synchronization).
	while(!( hrtc.Instance->ISR & (1<<ISR_INITF_BIT) ));

	//  program the synchronous prescaler factor
	hrtc.Instance->PRER &= ~RTC_PRER_ASYNC_LSB;
	hrtc.Instance->PRER |= synch_factor;

	// program first the asynchronous prescaler factor in RTC_PRER register
	hrtc.Instance->PRER &= ~(RTC_PRER_ASYNC_MASK<<RTC_PRER_ASYNC_LSB);
	hrtc.Instance->PRER |= asynch_factor<<RTC_PRER_ASYNC_LSB;

	//  configure the time format (24 hours) through the FMT bit in the RTC_CR
	hrtc.Instance->CR &= ~(1<<RTC_CR_FMT_MASK);
}

int rtc_check_init_success(){
	return (hrtc.Instance->ISR & (1<<7)) != 0;
}

int rtc_init(){
	rtc_init_configure();

	// exiting the init mode
	hrtc.Instance->ISR &= ~(1<<ISR_IN_BIT);
	return rtc_check_init_success();

}

unsigned char decimal_to_bcd(unsigned char decimal) {
    unsigned char tens = decimal / 10;
    unsigned char units = decimal % 10;
    return (tens << 4) | units;
}

int rtc_init_date(uint8_t month_day, uint8_t month, uint8_t year, uint8_t seconds, uint8_t minutes, uint8_t hour){

	rtc_init_configure();

	/*
	 * @time
	 * */
	// PM/AM annotation : setting to 0 for 24-hour format
	hrtc.Instance->TR &= 0xFFBFFFFF;
	// hour
	unsigned char hour_tens_bcd = decimal_to_bcd(hour/10);
	unsigned char hour_units_bcd = decimal_to_bcd(hour%10);
	hrtc.Instance->TR &= ~(3<<20);
	hrtc.Instance->TR |= hour_tens_bcd<<20;
	hrtc.Instance->TR &= ~(0xF << 16);
	hrtc.Instance->TR |= (hour_units_bcd << 16);

	unsigned char minute_tens_bcd = decimal_to_bcd(minute/10);
	unsigned char minute_units_bcd = decimal_to_bcd(minutes%10);
	hrtc.Instance->TR &= ~(7<<12);
	hrtc.Instance->TR |= minute_tens_bcd << 12;
	hrtc.Instance->TR &= ~(0xF<<8);
	hrtc.Instance->TR |= minute_units_bcd << 8;

	unsigned char second_tens_bcd = decimal_to_bcd(seconds/10);
	unsigned char second_units_bcd = decimal_to_bcd(seconds%10);
	hrtc.Instance->TR &= ~(7<<4);
	hrtc.Instance->TR |= second_tens_bcd << 4;
	hrtc.Instance->TR &= ~0xF;
	hrtc.Instance->TR |= second_units_bcd;


	/*
	 * @date
	 * */
	// setting bits to zero
	hrtc.Instance->DR &= ~(0xFF1F3F);
	unsigned char year_tens_bcd = decimal_to_bcd(year/10);
	unsigned char year_units_bcd = decimal_to_bcd(year%10);
	unsigned char month_tens_bcd = decimal_to_bcd(month/10);
	unsigned char month_units_bcd = decimal_to_bcd(month%10);
	unsigned char day_tens_bcd = decimal_to_bcd(month_day/10);
	unsigned char day_units_bcd = decimal_to_bcd(month_day%10);

	hrtc.Instance->DR |= (year_tens_bcd << 20) | (year_units_bcd << 16) | (month_tens_bcd << 12)
							| (month_units_bcd << 8) | (day_tens_bcd << 4) | day_units_bcd;


	// exiting the init mode
	hrtc.Instance->ISR &= ~(1<<ISR_IN_BIT);

	return rtc_check_init_success();
}

int rtc_init_struct(RTC_DateTypeDef *date, RTC_TimeTypeDef *time){
	return rtc_init_date(date->Date, date->Month, date->Year, time->Seconds, time->Minutes, time->Hours);
}

//void rtc_set_time
