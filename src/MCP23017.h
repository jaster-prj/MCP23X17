#ifndef __MCP23017_H
#define __MCP23017_H

#include "MCP23X17.h"
#include <Wire.h>

class MCP23017 : public MCP23X17
{
    private:
        uint8_t _deviceAddr;
        TwoWire *_bus;

    public:
        MCP23017(uint8_t address);
        MCP23017(uint8_t address, TwoWire *bus);
        ~MCP23017();
        void begin(int sdaPin, int sclPin);
        bool checkAck();
        void writeRegister(MCP23X17Register reg, uint8_t value);
        void writeRegister(MCP23X17Register reg, uint8_t portA, uint8_t portB);
        uint8_t readRegister(MCP23X17Register reg);
        void readRegister(MCP23X17Register reg, uint8_t& portA, uint8_t& portB);
};

#endif