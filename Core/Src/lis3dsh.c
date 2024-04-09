#include "lis3dsh.h"
/**
 * @brief Initializes the LIS3DSH accelerometer.
 * 
 * This function initializes the LIS3DSH accelerometer by setting up the necessary registers.
 * It sets the accelerometer to 100Hz, activates all axes, and configures interrupts wich are
 * triggered on the INT1 pin of the LIS3DSH.
 * 
 * @return void
 */
void init_ACC(void){
    //setup
    SPIread(CTRL_REG5 | SPI_READ);
    SPIread(CTRL_REG5 | SPI_READ);

    //CTRL_REG4
    SPIwrite(CTRL_REG4, 0x67); // 100 Hz, tous les axes actifs

    //CTRL_REG5
    SPIwrite(CTRL_REG5, 0x00);//40h

    //INTERRUPT
    SPIwrite(CTRL_REG3, 0x68); // Enable DRDY interrupt on INT1 + IEL

    //SM1 config
    SPIwrite(THRS_SM1_1, 0x50); // Threshold for SM1 (1.25G)
    SPIwrite(MASK1_A, 0x08); // Mask SM1 interrupt (Z axis)
    SPIwrite(MASK1_B, 0x08);
    SPIwrite(SETT1, 0x23);
    //ST1x config
    SPIwrite(0x40,0x05);// NOP next: > THRS1
    SPIwrite(0x41,0x11);// Continue
    SPIwrite(CTRL_REG1, 0x01); // Enable inertial interrupt

    //SM2 config
    SPIwrite(THRS_SM2_1, 0x35); // Threshold for SM2 (0.8G)
    SPIwrite(MASK2_A, 0x08); // Mask SM2 interrupt (Z axis)
    SPIwrite(MASK2_B, 0x08);
    SPIwrite(SETT2, 0x23);
    //ST2x config
    SPIwrite(0x60,0x07);// NOP next: < THRS1
    SPIwrite(0x61,0x11);
    SPIwrite(CTRL_REG2, 0x01); // Enable inertial interrupt

    //setup
    SPIread(CTRL_REG5 | SPI_READ);
    SPIread(CTRL_REG5 | SPI_READ);
}

/**
 * @brief Reads the X acceleration value.
 * 
 * This function reads the X acceleration value from the accelerometer.
 * 
 * @return the X acceleration raw value
 */
int16_t read_ACC_X(void){
    //setup
    SPIread(CTRL_REG5 | SPI_READ);
    SPIread(CTRL_REG5 | SPI_READ);

    //trame de lecture
    int16_t X;
    X = SPIread(OUT_X_H | SPI_READ);
    X = X << 8;
    X |= SPIread(OUT_X_L | SPI_READ);
    return X;
}

/**
 * @brief Reads the Y acceleration value.
 * 
 * This function reads the Y acceleration value from the accelerometer.
 * 
 * @return the Y acceleration raw value
 */
int16_t read_ACC_Y(void){
    //setup
    SPIread(CTRL_REG5 | SPI_READ);
    SPIread(CTRL_REG5 | SPI_READ);

    //trame de lecture
    int16_t Y;
    Y = SPIread(OUT_Y_H | SPI_READ);
    Y = Y << 8;
    Y |= SPIread(OUT_Y_L | SPI_READ);
    return Y;
}

/**
 * @brief Reads the Z acceleration value.
 * 
 * This function reads the Z acceleration value from the accelerometer.
 * 
 * @return the Z acceleration raw value
 */
int16_t read_ACC_Z(void){
    //setup
    SPIread(CTRL_REG5 | SPI_READ);
    SPIread(CTRL_REG5 | SPI_READ);

    //trame de lecture
    int16_t Z;
    Z = SPIread(OUT_Z_H | SPI_READ);
    Z = Z << 8;
    Z |= SPIread(OUT_Z_L | SPI_READ);
    return Z;
}

/**
 * @brief Reads the X acceleration value in g.
 * 
 * This function reads the acceleration value of the X axe converted in g.
 * 
 * @return the acceleration value in g
 */
double get_ACC_X(void){ // get the acceleration of the X axe in g
    return read_ACC_X() / 16.384;
}

/**
 * @brief Reads the Y acceleration value in g.
 * 
 * This function reads the acceleration value of the Y axe converted in g.
 * 
 * @return the acceleration value in g
 */
double get_ACC_Y(void){ // get the acceleration of the Y axe in g
    return read_ACC_Y() / 16.384;
}

/**
 * @brief Reads the Z acceleration value in g.
 * 
 * This function reads the acceleration value of the Z axe converted in g.
 * 
 * @return the acceleration value in g
 */
double get_ACC_Z(void){ // get the acceleration of the Z axe in g
    return read_ACC_Z() / 16.384;
}

/**
 * @brief set the maximum value of the Z axe in g.
 * 
 * This function set the maximum value of the Z axe in g over which the INT1 interrupt of the
 * LIS3DSH will be triggered.
 * 
 * @return void
 */
void set_Zmax(double Zmax){ // set the maximum value of the Z axe in g
    int Zmax_int = (int)(Zmax*64);
    if(Zmax_int > 0x7F){
        Zmax_int = 0x7F;
    } else if (Zmax_int < 0x00)
    {
        Zmax_int = 0x00;
    }
    SPIread(CTRL_REG5 | SPI_READ);
    SPIread(CTRL_REG5 | SPI_READ);

    SPIwrite(THRS_SM1_1, (int)(Zmax_int));
}

/**
 * @brief set the minimum value of the Z axe in g.
 * 
 * This function set the minimum value of the Z axe in g under which the INT1 interrupt of the
 * LIS3DSH will be triggered.
 * 
 * @return void
 */
void set_Zmin(double Zmin){ // set the minimum value of the Z axe in g
    int Zmin_int = (int)(Zmin*64);
    if(Zmin_int > 0x7F){
        Zmin_int = 0x7F;
    } else if (Zmin_int < 0x00)
    {
        Zmin_int = 0x00;
    }
    SPIread(CTRL_REG5 | SPI_READ);
    SPIread(CTRL_REG5 | SPI_READ);
    
    SPIwrite(THRS_SM2_1, (int)(Zmin*64));
}