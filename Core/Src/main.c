/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart2.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
} DateTypeDef;


RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

typedef struct{
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
} TimeTypeDef;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ISR_IN_BIT 7
#define ISR_INITF_BIT 6
#define RTC_PRER_ASYNC_LSB 16
#define RTC_PRER_SYNC_MSB 14
#define RTC_PRER_SYNC_MASK 0x7FFF
#define RTC_PRER_ASYNC_MASK 0x7F
#define RTC_CR_FMT_MASK 6
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void get_date(DateTypeDef *date);
void get_time(TimeTypeDef *time);

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

GPIO_TypeDef* portA = GPIOA;
GPIO_PinState btn_poussoir_state;
int interupt = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void init_rtc(void);
static void init_rtc_datetime(DateTypeDef date, TimeTypeDef time);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  TimeTypeDef time = {0};  // HAL_RTC_DST_Add1Hour(&hrtc)
  DateTypeDef date = {0};
  char time_string[20];
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  init_rtc();
  init_usart();
  /* USER CODE END 2 */

  uint8_t data = 0;
  int input_nb = 6;
  uint8_t time_tab[6] = {0};

  for(int i = 0; i < input_nb; i++){
	  while(!(USART2->SR & USART_SR_RXNE)){};
	  data = USART2->DR;
	  time_tab[i] = data; // Convert ASCII value to numerical value
	  USART2->DR = 'A';
  }

  date.Year = time_tab[0];
  date.Month = time_tab[1];
  date.Date = time_tab[2];

  time.Hours = time_tab[3];
  time.Minutes = time_tab[4];
  time.Seconds = time_tab[5];

  // init rtc with input date
  init_rtc_datetime(date, time);



  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  btn_poussoir_state = HAL_GPIO_ReadPin(portA, GPIO_PIN_0);
	  if(interupt == 1)
	  {
		  get_time(&time);
		  get_date(&date);
		  sprintf(time_string, "%02d-%02d-%04d %02d:%02d:%02d\r\n", date.Date, date.Month, 2000 + date.Year, time.Hours, time.Minutes, time.Seconds);
		  serial_puts(&time_string);
		  newLine();
		  interupt = 0;
	  }


    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */

/* USER CODE BEGIN 4 */
static void init_rtc(void)
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

static void init_rtc_datetime(DateTypeDef date, TimeTypeDef time)
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
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
