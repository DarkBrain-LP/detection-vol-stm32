/*
 * DAC.h
 *
 *  Created on: Mar 22, 2024
 *      Author: loisb
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

void DAC_configurationBeep(uint8_t mode);
void DAC_changeVolume(uint8_t volume);
void DAC_init();
void DAC_freq_set(uint8_t frequency);
void DAC_OnTime(uint8_t ontime);
void DAC_OffTime(uint8_t);
static void MX_I2S3_Init(void);
void init_rst(void);
void DAC_Run_Alarm(void);
void DAC_timed_alarm(uint16_t ms_time);
void DAC_freq_pitch(uint8_t pitch);
void DAC_play_song(uint8_t *song,uint8_t size);

extern I2S_HandleTypeDef hi2s3;


#endif /* INC_DAC_H_ */

