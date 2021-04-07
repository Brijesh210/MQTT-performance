/* PCF8574_DataBus - Use the PCF8574 I2C Port Extender for controlling the Data Bus
 * Copyright (c) 2011 Wim Huiskamp
 *
 * Released under the MIT License: http://mbed.org/license/mit
 *
 * version 0.2 Initial Release
*/
#include "mbed.h"
#include "PCF8574_DataBus.h"


/** Create an PCF8574_DataBus object connected to the specified I2C object and using the specified deviceAddress
 *
 * @param I2C &i2c the I2C port to connect to 
 * @param char deviceAddress the address of the PCF8574
*/
PCF8574_DataBus::PCF8574_DataBus(I2C &i2c, char deviceAddress) : _i2c(i2c) {
   _writeOpcode = deviceAddress & 0xFE; // low order bit = 0 for write
   _readOpcode  = deviceAddress | 0x01; // low order bit = 1 for read
   _init();
}

/** Optimised DataBus write operation.
 * @param byte the datavalue to output on the bus
*/
void PCF8574_DataBus::write(char byte) {
    char data[1];
    
    data[0] = byte;
    if (_i2c.write(_writeOpcode, data, 1)!=0)    // Write datavalue to bus   
        {
        printf("erreur i2C write");
        }
}

/** Optimised DataBus read operation.
 *
 * @returns current data from Databus 
*/
char PCF8574_DataBus::read() {
    char data[1];

//Make sure that databus is enabled for Read
    data[0] = 0xFF;                       // Init Port for datainput by Writing 0xFF 
   _i2c.write(_writeOpcode, data, 1);    // Write to bus   
    
   if( _i2c.read(_readOpcode, data, 1)!=0)      // Read data from bus
     {
        printf("erreur i2C read");
        data[0]=0xAA;
     }  
   return data[0];
}


/** Enable databus for Write or Read
 *
 * @param Bus_Dir bus_dir
*/
void PCF8574_DataBus::busdir (Bus_Dir bus_dir) {

    if (bus_dir == READ) {
      // Make sure that databus is enabled for READ
      //write(0xFF);                          // Init Port as input by Writing 0xFF 
            
    }                
    else {
      // Make sure that databus is enabled for WRITE   
      //write(0xFF);                          // Not really needed, just Init Port to safe settings
    }      
}
void PCF8574_DataBus::output(void)
    {
    busdir (WRITE);
    }
void PCF8574_DataBus::input(void)
    {
    busdir (READ);
    }



/** Init PCF8574_DataBus
 * @param
 * @returns 
 */
void PCF8574_DataBus::_init() {
 
    busdir(WRITE);                          // Init Port as output 
}