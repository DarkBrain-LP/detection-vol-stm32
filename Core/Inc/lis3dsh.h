#ifndef LIS3DSH_H_
#define LIS3DSH_H_
#endif /* LIS3DSH_H_ */

#include "spi.h"

// LIS3DSH Register Map
#define CTRL_REG1 0x21
#define CTRL_REG2 0x22
#define CTRL_REG3 0x23
#define CTRL_REG4 0x20
#define CTRL_REG5 0x24
#define STATUS_REG 0x27
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
#define STAT 0x18
#define SETT1 0x5B
#define SETT2 0x7B
#define TIM1_1_H 0x55
#define TIM1_1_L 0x54
#define TIM2_1_H 0x53
#define TIM2_1_L 0x52
#define MASK1_A 0x5A
#define MASK1_B 0x59
#define MASK2_A 0x7A
#define MASK2_B 0x79
#define PEAK1 0x19
#define PEAK2 0x1A
#define OUTS1 0x5F
#define OUTS2 0x7F
#define THRS3 0x1F
#define THRS_SM1_1 0x57
#define THRS_SM1_2 0x56
#define THRS_SM2_1 0x77
#define THRS_SM2_2 0x76

// SPI Communication with LIS3DSH
#define SPI_READ 0x80
#define SPI_WRITE 0x00

// LIS3DSH Functions
void init_ACC(void);
int16_t read_ACC_X(void);
int16_t read_ACC_Y(void);
int16_t read_ACC_Z(void);
double get_ACC_X(void);
double get_ACC_Y(void);
double get_ACC_Z(void);
void set_Zmax(double Zmax);
void set_Zmin(double Zmin);
