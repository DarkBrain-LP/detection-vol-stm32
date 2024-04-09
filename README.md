- Ajouter le header et le fichier source (rtc et usart)
- Dans le main, lire l'heure et la date à partir du script python
Le script utilise le package serial. Installez le si vous ne l'avez pas :
```
pip install pyserial
```
Mettre à jour le fichier si la connexion usart n'est pas effectuée sur le port 'COM10'
puis : 
```
py rtc_init.py
```
Exemple d'utilisation: vous pouvez attendre la lecture de l'heure avant exécution de votre projet :
#### les variables utilisées:
```
GPIO_TypeDef* portA = GPIOA;
GPIO_PinState btn_poussoir_state;
int interupt = 0;

TimeTypeDef time = {0};  
DateTypeDef date = {0};
char time_string[20];
```
#### reception de la date via le port serie
Assurez-vous d'exécuter le script python. En effet, le code attends la présence de donnée sur le port série.
```
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
```
-  initier le rtc avec la data et l'heure

```
	init_rtc_datetime(date, time);
```

## Lire et afficher l'heure
Dans la boucle while :
```
    btn_poussoir_state = HAL_GPIO_ReadPin(portA, GPIO_PIN_0);
	  if(interupt == 1)
	  {
		  get_time(&time);
		  get_date(&date);
		  sprintf(time_string, "%02d-%02d-%04d %02d:%02d:%02d\r\n", date.Date, date.Month, 2000 + date.Year, time.Hours, time.Minutes, time.Seconds);
		  serial_puts(&time_string);
		  newLine();
		  //DAC_Run_Alarm();
		  interupt = 0;
	  }
```