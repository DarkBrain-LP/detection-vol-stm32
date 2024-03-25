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







