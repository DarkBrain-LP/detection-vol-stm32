/*
 * DAC.c
 *
 *  Created on: Mar 22, 2024
 *      Author: loisb
 */
#include "main.h"
#include "i2c.h"
#include <DAC.h>

uint8_t data[2];
uint8_t buffer[1];
uint8_t sendSize = 2;
uint8_t I2C_DAC_Transmit = 0x94;
uint16_t I2S_buffer[1];
/**
 * @brief Initialisation du composant et de tout ce qui est nécessaire à son fonctionnement
 * @param none
 * @retval none
 * */
void DAC_init(){
	GPIOD->BSRR = 0x00000010; //RST component
	// ecrire 04 dans le registre 0xA0
	data[0]=0x04;
	data[1]=0xA0;
	I2C_Master_Transmit(0x94,data,2);
	// ecrire 06 dans le registre 0x87
	data[0]=0x06;
	data[1]=0x87;
	I2C_Master_Transmit(0x94,data,2);
	// écrire 0x99 dans le registre 0x00
	data[0]=0x00;
	data[1]=0x99;
	I2C_Master_Transmit(I2C_DAC_Transmit,data,sendSize);
	// écrire 0x80 dans registre 0x47
	data[0]=0x47;
	data[1]=0x80;
	I2C_Master_Transmit(I2C_DAC_Transmit,data,sendSize);
	// écrire 1 dans le bit 7 du registre 32
	data[0] = 0x32;
	I2C_Master_Transmit(I2C_DAC_Transmit,data[0],1);
	I2C_Master_Receive(0x95,buffer,1);
	buffer[0] |= 10000000;
	data[1] = buffer[0];
	I2C_Master_Transmit(I2C_DAC_Transmit,data,sendSize);
	// écrire 0 dans le bit 7 du registre 32
	I2C_Master_Transmit(I2C_DAC_Transmit,data[0],1);
	I2C_Master_Receive(0x95,buffer,1);
	buffer[0] &= 01111111;
	data[1]= buffer[0];
	I2C_Master_Transmit(0x94,data,2);
	// ecrire 0 dans le registre 0x0
	data[0]=0;
	data[1]=0;
	I2C_Master_Transmit(0x94,data,2);
	// ecrire 9E dans le registre 0x02
	data[0]=0x02;
	data[1]=0x9E;
	I2C_Master_Transmit(0x94,data,2);
}

/**
 * @brief Change la configurtion du bip généré
 * @param mode 0: aucun bip, 1 : 1 seul bip , 2 : bip alternant, 3 : bip continu
 * @retval none
 * */
void DAC_configurationBeep(uint8_t mode){
	data[0] = 0x1E;
	switch (mode){
	case 0:
		data[1] = 0x00; //aucun bip
		break;
	case 1:
		data[1] = 0x40; //1 seul bip
		break;
	case 2:
		data[1] = 0x80; //bip alternant
		break;
	case 3:
		data[1] = 0xC0; //bip continu
		break;
	default:
		data[1] = 0xC0;
	}
	I2C_Master_Transmit(I2C_DAC_Transmit,data,sendSize);
}

/**
 * @brief change le volume du bip
 * @param 1 : atténuation de 6dB, 0 : augmentation de 6dB
 * @retval none
 * */
void DAC_changeVolume(uint8_t volume){
	data[0] = 0x1D;
	uint8_t buf[8];
	I2C_Master_Transmit(I2C_DAC_Transmit,data,1);
	I2C_Master_Receive(0x95,buf,1);
	if (volume == 0) { //aténuation de 6dB
		buf[0] &= 11100000;
		data[1] = buf[0];
	} else { //augmentation de 6dB
		buf[0] |= 00000110;
		data[1] = buf[0];
	}
	I2C_Master_Transmit(I2C_DAC_Transmit,data,sendSize);
}

/**
 * @brief change la fréquence du signal sonore
 * @param frequency 0 : low-pitch, 1 : medium-pitch, 2 : high-pitch
 * @retval none
 * */
void DAC_freq_set(uint8_t frequency){
	uint8_t data[2];
	uint8_t buffer[1];
	switch (frequency){
	case 0: //low-pitch
		I2C_Master_Transmit(0x94,0x1C,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 0x10;
		data[0]= 0x1C;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	case 1: //medium-pitch
		I2C_Master_Transmit(0x94,0x1C,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 0x80;
		data[0]= 0x1C;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	case 2: //high-pitch
		I2C_Master_Transmit(0x94,0x1C,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 0xF0;
		data[0]= 0x1C;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	default : //medium-pitch
		I2C_Master_Transmit(0x94,0x1C,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 0x80;
		data[0]= 0x1C;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	}
}
/**
 * @brief Change la durée de ontime (temps de génération du bip)
 * @param ontime 0 : short, 1 : medium, 2 : long
 * @retval none
 * */
void DAC_OnTime(uint8_t ontime){
	uint8_t data[2];
	uint8_t buffer[1];
	switch (ontime){
	case 0: //short
		I2C_Master_Transmit(0x94,0x1C,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 1;
		data[0]= 0x1C;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	case 1: //medium
		I2C_Master_Transmit(0x94,0x1C,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 0x07;
		data[0]= 0x1C;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	case 2: //long
		I2C_Master_Transmit(0x94,0x1C,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 0x0F;
		data[0]= 0x1C;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	default : //short
		I2C_Master_Transmit(0x94,0x1C,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 1;
		data[0]= 0x1C;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	}
}
/**
 * @brief change la durée de offtime (durée pendant laquelle aucun bip n'est généré)
 * @param ontime 0 : short, 1 : medium, 2 : long
 * @retval none
 * */
void DAC_OffTime(uint8_t offtime){
	uint8_t data[2];
	uint8_t buffer[1];
	switch (offtime){
	case 0: //short
		I2C_Master_Transmit(0x94,0x1D,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] &=0x0F ;
		data[0]= 0x1D;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	case 1: //medium
		I2C_Master_Transmit(0x94,0x1D,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 0x30;
		data[0]= 0x1D;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	case 2: //long
		I2C_Master_Transmit(0x94,0x1D,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] |= 0x70;
		data[0]= 0x1D;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
	default : //short
		I2C_Master_Transmit(0x94,0x1D,1);
		I2C_Master_Receive(0x95,buffer,1);
		buffer[0] &= 0x0F;
		data[0]= 0x1D;
		data[1]= buffer[0];
		I2C_Master_Transmit(0x94,data,2);
		break;
		}
}


/**
 * @brief emission d'un beep pendant la durée du transfert I2S (court, à mettre dans une boucle)
 * @param none
 * @retval none
 * */
void DAC_Run_Alarm(){
	I2S_buffer[0] = 0xAAAA;							//Frame to send on the I2S bus
	HAL_I2S_Transmit(&hi2s3, I2S_buffer, 1, 500);
}
/**
 * @brief emission d'un beep pendant un durée spécifié - ! Fonction bloquante !
 * @param ms_time durée pendant laquelle l'utilisateur veut générer un beep
 * @retval none
 * */
void DAC_timed_alarm(uint16_t ms_time){
	uint32_t current_tick;
	current_tick=HAL_GetTick();
	I2S_buffer[0] = 0xAAAA;
	while(HAL_GetTick()-current_tick<ms_time){
		HAL_I2S_Transmit(&hi2s3, I2S_buffer, 1, ms_time/2);
	}
}
/**
 * @brief change the frequency of the beep during runtime
 * @param pitch pitch ou want to play as described in the documentation
 * @retval none
 * */
void DAC_freq_pitch(uint8_t pitch){
	uint8_t buffer[1];
	uint8_t data[2];
	uint8_t state;
	//Arrêt du beep
	DAC_configurationBeep(0);
	//modification de la fréquence
	I2C_Master_Transmit(0x94,0x1C,1);
	I2C_Master_Receive(0x95,buffer,1);
	buffer[0] |= (pitch<<4);
	data[0]= 0x1C;
	data[1]= buffer[0];
	I2C_Master_Transmit(0x94,data,2);
	//on relance le beep
	DAC_configurationBeep(3);
}
/**
 * @brief Easter-egg : play the song specified in the song tab
 * @param song tab that contains the song, written note by note as described in the documentation
 * @param size size of the tab
 * @retval none
 * */
void DAC_play_song(uint8_t *song, uint8_t size){
	for(int i=0;i<size;i++){
		DAC_freq_pitch(song[i]);
		timed_alarm(500);
		HAL_Delay(500);
	}
}

