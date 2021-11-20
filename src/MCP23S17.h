#ifndef __MCP23S17_H
#define __MCP23S17_H

#include "MCP23X17.h"
#include <SPI.h>

class MCP23S17 : public MCP23X17
{
    private:
        uint8_t     _deviceAddr;
        uint8_t     _cs;
        SPIClass    *_bus;

    public:
        MCP23S17(uint8_t address, uint8_t chipSelect);
        MCP23S17(uint8_t address, uint8_t chipSelect, SPIClass *bus);
        ~MCP23S17();
        void writeRegister(MCP23X17Register reg, uint8_t value);
        void writeRegister(MCP23X17Register reg, uint8_t portA, uint8_t portB);
        uint8_t readRegister(MCP23X17Register reg);
        void readRegister(MCP23X17Register reg, uint8_t& portA, uint8_t& portB);
};

#endif