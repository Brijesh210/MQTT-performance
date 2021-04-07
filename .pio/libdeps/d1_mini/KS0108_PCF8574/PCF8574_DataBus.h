/* PCF8574_DataBus - Use the PCF8574 I2C Port Extender for controlling the Data Bus
 * Copyright (c) 2011 Wim Huiskamp
 *
 * Released under the MIT License: http://mbed.org/license/mit
 *
 * version 0.2 Initial Release
*/
#ifndef _PCF8574_DATABUS_H
#define _PCF8574_DATABUS_H

//Pin Defines for PCF8574 Data Bus
#define D_D0                   0x01
#define D_D1                   0x02
#define D_D2                   0x04
#define D_D3                   0x08
#define D_D4                   0x10
#define D_D5                   0x20
#define D_D6                   0x40
#define D_D7                   0x80

#define D_DATA_MSK             0xFF

//Enums for Data Bus
#include "BusEnums.h"


/** Create an PCF8574_DataBus object connected to the specified I2C object and using the specified deviceAddress
 *
 * @param I2C &i2c the I2C port to connect to 
 * @param char deviceAddress the address of the PCF8574
*/
class PCF8574_DataBus {
public:
    PCF8574_DataBus(I2C &i2c, char deviceAddress);
    char read();
    void write(char byte);
    void busdir (Bus_Dir bus_dir); 
    void output(void);
    void input(void);  
protected:
    I2C &_i2c;
    char _readOpcode;
    char _writeOpcode; 
    void _init(); 
};

#endif