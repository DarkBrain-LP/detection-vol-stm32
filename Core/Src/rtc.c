#include "rtc.h"
#include "stm32f4xx_hal_conf.h"
#include "usart2.h"


char buffer_time[30];
char buffer_date[30];
char *time_result;
char *date_result;

// Fonction pour vérifier si une année est bissextile
uint8_t isLeapYear(uint16_t year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

// Fonction pour obtenir le nombre maximal de jours dans un mois donné
uint8_t maxDaysInMonth(uint8_t month, uint8_t year) {
    uint8_t daysInMonth[] = {31, 28 + isLeapYear(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return daysInMonth[month - 1];
}

void actualise_time(void){
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	// Ajouter une seconde à l'heure
	sTime.Seconds++;
    // Vérifier si nous avons dépassé 59 secondes
    if (sTime.Seconds > 59) {
        sTime.Seconds = 0;
        sTime.Minutes++;
        // Vérifier si nous avons dépassé 59 minutes
        if (sTime.Minutes > 59) {
        	sTime.Minutes = 0;
	        sTime.Hours++;
	        // Vérifier si nous avons dépassé 23 heures
	        if (sTime.Hours > 23) {
	            sTime.Hours = 0;
	            if (sTime.Minutes == 0) {
	               // Avancer d'un jour
	                sDate.Date++;
	                // Mettre à jour la date dans le RTC
	                HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	                // Vérifier si nous avons dépassé le nombre maximal de jours pour le mois actuel
	                if (sDate.Date > maxDaysInMonth(sDate.Month, sDate.Year)) {
	                	sDate.Date = 1; // Réinitialiser le jour à 1
	                    sDate.Month++; // Passer au mois suivant
	                    // Vérifier si nous avons dépassé décembre
	                    if (sDate.Month > 12) {
	                    	sDate.Month = 1; // Réinitialiser le mois à janvier
	                    	sDate.Year++; // Passer à l'année suivante
	                    }
	                }

	            }
	        }
	    }
	    // Mettre à jour l'heure dans le RTC
	    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	}
    // Lire la date actuelle
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    // Mettre à jour la date dans le RTC
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    HAL_Delay(1000);
}

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

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}


void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}



void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x00;
  sTime.Minutes = 0x00;
  sTime.Seconds = 0x00;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
  sDate.Month = RTC_MONTH_MARCH;
  sDate.Date = 0x24;
  sDate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

