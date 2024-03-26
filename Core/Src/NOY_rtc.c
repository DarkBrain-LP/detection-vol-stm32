#include "stm32f4xx_hal.h"
#include "NOY_rtc.h"


RTC_HandleTypeDef our_hrtc;
uint8_t asynch_factor = 0x7F; // 0111.1111
uint8_t synch_factor = 0xFF; // 1111.1111 255

void get_date(RTC_DateTypeDef *date){
	date->Year = (((our_hrtc.Instance->DR >> 20)&0xF)*10) + ((our_hrtc.Instance->DR >> 16)&0xF);
	date->Month = (10*((our_hrtc.Instance->DR >> 12)&1)) + ((our_hrtc.Instance->DR >> 8)&0xF);
	date->Date = (10*((our_hrtc.Instance->DR >> 4)&3)) + (our_hrtc.Instance->DR & 0xF);
}

void get_time(RTC_TimeTypeDef *time){
	time->Seconds = (our_hrtc.Instance->TR & 0xF) + 10*((our_hrtc.Instance->TR >> 4)&7);
	time->Minutes = ((our_hrtc.Instance->TR >> 8)&0xF) + 10*((our_hrtc.Instance->TR >> 12)&7);
	time->Hours = ((our_hrtc.Instance->TR >> 16)&0xF) + 10*(((our_hrtc.Instance->TR >> 20)&3));
}

void rtc_init_configure(){
	our_hrtc.Instance = RTC;
	HAL_StatusTypeDef status = HAL_ERROR;

	status = RTC_EnterInitMode(&our_hrtc);

	    if (status == HAL_OK)
	    {
	      /* Clear RTC_CR FMT, OSEL and POL Bits */
	    	our_hrtc.Instance->CR &= ((uint32_t)~(RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL));
	      /* Set RTC_CR register */
	    	our_hrtc.Instance->CR |= (uint32_t)(RTC_HOURFORMAT_24 | RTC_OUTPUT_DISABLE | RTC_OUTPUT_POLARITY_HIGH);

	      /* Configure the RTC PRER */
	    	our_hrtc.Instance->PRER = (uint32_t)(synch_factor);
	    	our_hrtc.Instance->PRER |= (uint32_t)(asynch_factor << RTC_PRER_PREDIV_A_Pos);

	      /* Exit Initialization mode */
	      status = RTC_ExitInitMode(&our_hrtc);
	    }

	    if (status == HAL_OK)
	    {
	    	our_hrtc.Instance->TAFCR &= (uint32_t)~RTC_OUTPUT_TYPE_PUSHPULL;
	    	our_hrtc.Instance->TAFCR |= (uint32_t)(RTC_OUTPUT_TYPE_OPENDRAIN);
	    }

	// unlocking write protection
	//Write ‘0xCA’ into the RTC_WPR register
	//our_hrtc.Instance->WPR &= ~(0xFF);
	our_hrtc.Instance->WPR |= 0xCA;//1100.1010
	// Write ‘0x53’ into the RTC_WPR register
	//our_hrtc.Instance->WPR &= ~(0xFF);
	our_hrtc.Instance->WPR |= 0x53;


	/*
	//Set INIT bit to 1 in the RTC_ISR register to enter initialization mode
	//our_hrtc.Instance = RTC;
	our_hrtc.Instance->ISR &= ~(1<<ISR_IN_BIT);
	our_hrtc.Instance->ISR |= (1<<ISR_IN_BIT);

	// Poll INITF bit of in the RTC_ISR register. The initialization phase mode is entered when INITF is set to 1. It takes from 1 to 2 RTCCLK clock cycles (due to clock synchronization).
	int i 0;
	while(( our_hrtc.Instance->ISR & (1<<ISR_INITF_BIT) ) == 0){
		i++;
	}

	//  program the synchronous prescaler factor
	our_hrtc.Instance->PRER &= ~RTC_PRER_ASYNC_LSB;
	our_hrtc.Instance->PRER |= synch_factor;

	// program first the asynchronous prescaler factor in RTC_PRER register
	our_hrtc.Instance->PRER &= ~(RTC_PRER_ASYNC_MASK<<RTC_PRER_ASYNC_LSB);
	our_hrtc.Instance->PRER |= asynch_factor<<RTC_PRER_ASYNC_LSB;

	//  configure the time format (24 hours) through the FMT bit in the RTC_CR
	our_hrtc.Instance->CR &= ~(1<<RTC_CR_FMT_MASK);
*/

}

int rtc_check_init_success(){
	return (our_hrtc.Instance->ISR & (1<<4)) != 0;
}

int rtc_init(){
	// Low Speed Clock config
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	RCC->CSR |= RCC_CSR_LSION;
	while((RCC->CSR & RCC_CSR_LSIRDY) != 0){};
	RCC->BDCR |= 0x8200;

	// enable RTC write
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	// enable init mode
	RTC->ISR |= (1<<ISR_IN_BIT);
	while(!( RTC->ISR & (1<<ISR_INITF_BIT) )){};


	// program first the asynchronous prescaler factor in RTC_PRER register
	our_hrtc.Instance->PRER &= ~(RTC_PRER_ASYNC_MASK<<RTC_PRER_ASYNC_LSB);
	our_hrtc.Instance->PRER |= asynch_factor<<RTC_PRER_ASYNC_LSB;

	//  program the synchronous prescaler factor
	RTC->PRER &= ~RTC_PRER_SYNC_MASK;
	RTC->PRER |= synch_factor;

	// default time
	RTC->TR &= 0xFF808080;
	RTC->TR |=(7<<16) | (0x55<<8);

	// default date
	RTC->DR &= 0xFF0020C0;
	RTC->DR |= (0x24<<16) | (3<<8) | (0x24);

	// disable init mode
	RTC->CR |= RTC_CR_BYPSHAD;
	RTC->ISR &= ~RTC_ISR_INIT;
	PWR->CR &= ~PWR_CR_DBP;

	return 1;
	/*
	RTC_TimeTypeDef sTime = {0};
	  RTC_DateTypeDef sDate = {0};

	our_hrtc.Instance = RTC;
	  our_hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	  our_hrtc.Init.AsynchPrediv = 127;
	  our_hrtc.Init.SynchPrediv = 255;
	  our_hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	  our_hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	  our_hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	  if (HAL_RTC_Init(&our_hrtc) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  if (HAL_RTC_SetTime(&our_hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
	  sDate.Month = RTC_MONTH_MARCH;
	  sDate.Date = 24;
	  sDate.Year = 24;

	  if (HAL_RTC_SetDate(&our_hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  return 1;
	/*
	rtc_init_configure();

	// exiting the init mode
	our_hrtc.Instance->ISR &= ~(1<<ISR_IN_BIT);
	return rtc_check_init_success();
*/

}

unsigned char decimal_to_bcd(unsigned char decimal) {
    unsigned char tens = decimal / 10;
    unsigned char units = decimal % 10;
    return (tens << 4) | units;
}

int rtc_init_date(uint8_t month_day, uint8_t month, uint8_t year, uint8_t seconds, uint8_t minutes, uint8_t hour){

/*
	rtc_init_configure();

	 // @time

	// PM/AM annotation : setting to 0 for 24-hour format
	our_hrtc.Instance->TR &= 0xFFBFFFFF;
	// hour
	unsigned char hour_tens_bcd = decimal_to_bcd(hour/10);
	unsigned char hour_units_bcd = decimal_to_bcd(hour%10);
	our_hrtc.Instance->TR &= ~(3<<20);
	our_hrtc.Instance->TR |= hour_tens_bcd<<20;
	our_hrtc.Instance->TR &= ~(0xF << 16);
	our_hrtc.Instance->TR |= (hour_units_bcd << 16);

	unsigned char minute_tens_bcd = decimal_to_bcd(minutes/10);
	unsigned char minute_units_bcd = decimal_to_bcd(minutes%10);
	our_hrtc.Instance->TR &= ~(7<<12);
	our_hrtc.Instance->TR |= minute_tens_bcd << 12;
	our_hrtc.Instance->TR &= ~(0xF<<8);
	our_hrtc.Instance->TR |= minute_units_bcd << 8;

	unsigned char second_tens_bcd = decimal_to_bcd(seconds/10);
	unsigned char second_units_bcd = decimal_to_bcd(seconds%10);
	our_hrtc.Instance->TR &= ~(7<<4);
	our_hrtc.Instance->TR |= second_tens_bcd << 4;
	our_hrtc.Instance->TR &= ~0xF;
	our_hrtc.Instance->TR |= second_units_bcd;


	// @date
	// setting bits to zero
	our_hrtc.Instance->DR &= ~(0xFF1F3F);
	unsigned char year_tens_bcd = decimal_to_bcd(year/10);
	unsigned char year_units_bcd = decimal_to_bcd(year%10);
	unsigned char month_tens_bcd = decimal_to_bcd(month/10);
	unsigned char month_units_bcd = decimal_to_bcd(month%10);
	unsigned char day_tens_bcd = decimal_to_bcd(month_day/10);
	unsigned char day_units_bcd = decimal_to_bcd(month_day%10);

	our_hrtc.Instance->DR |= (year_tens_bcd << 20) | (year_units_bcd << 16) | (month_tens_bcd << 12)
							| (month_units_bcd << 8) | (day_tens_bcd << 4) | day_units_bcd;


	// exiting the init mode
	our_hrtc.Instance->ISR &= ~(1<<ISR_IN_BIT);

	return rtc_check_init_success();
	*/


	RTC_TimeTypeDef sTime = {0};
	  RTC_DateTypeDef sDate = {0};

	our_hrtc.Instance = RTC;
	  our_hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	  our_hrtc.Init.AsynchPrediv = 127;
	  our_hrtc.Init.SynchPrediv = 255;
	  our_hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	  our_hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	  our_hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	  if (HAL_RTC_Init(&our_hrtc) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  sTime.Hours = hour;
	  sTime.Minutes = minutes;
	  sTime.Seconds = seconds;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  if (HAL_RTC_SetTime(&our_hrtc, &sTime, RTC_FORMAT_BCDb) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
	  sDate.Month = month;
	  sDate.Date = month_day;
	  sDate.Year = year;

	  if (HAL_RTC_SetDate(&our_hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  return 1;
}

int rtc_init_struct(RTC_DateTypeDef *date, RTC_TimeTypeDef *time){

	return rtc_init_date(date->Date, date->Month, date->Year, time->Seconds, time->Minutes, time->Hours);
}



//void rtc_set_time
