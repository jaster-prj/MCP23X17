#include "MCP23S17.h"

MCP23S17::MCP23S17(uint8_t address, uint8_t chipSelect) : MCP23X17() {
    _deviceAddr = address;
	_cs = chipSelect;
    pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	_bus = &SPI;
}

MCP23S17::MCP23S17(uint8_t address, uint8_t chipSelect, SPIClass *bus) : MCP23X17() {
    _deviceAddr = address;
	_cs = chipSelect;
    pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	_bus = bus;
}

MCP23S17::~MCP23S17() {}

uint8_t MCP23S17::writeRegister(MCP23X17Register reg, uint8_t value)
{
    uint8_t cmd = 0x40 | ((_deviceAddr & 0b111) << 1);
    digitalWrite(_cs, LOW);
    _bus->transfer(cmd);
    _bus->transfer(static_cast<uint8_t>(reg));
    _bus->transfer(value);
    digitalWrite(_cs, HIGH);
    return 0;
}

uint8_t MCP23S17::writeRegister(MCP23X17Register reg, uint8_t portA, uint8_t portB)
{
    uint8_t cmd = 0x40 | ((_deviceAddr & 0b111) << 1);
    digitalWrite(_cs, LOW);
    _bus->transfer(cmd);
    _bus->transfer(static_cast<uint8_t>(reg));
    _bus->transfer(portA);
    _bus->transfer(portB);
    digitalWrite(_cs, HIGH);
    return 0;
}


uint8_t MCP23S17::readRegister(MCP23X17Register reg, uint8_t& buffer)
{
	uint8_t cmd = 0x41  | ((_deviceAddr & 0b111) << 1);
    digitalWrite(_cs, LOW);
    _bus->transfer(cmd);
    _bus->transfer(static_cast<uint8_t>(reg));
    buffer = _bus->transfer(0xFF);
    digitalWrite(_cs, HIGH);
	return 0;
}

uint8_t MCP23S17::readRegister(MCP23X17Register reg, uint8_t& portA, uint8_t& portB)
{
	uint8_t cmd = 0x41  | ((_deviceAddr & 0b111) << 1);
    digitalWrite(_cs, LOW);
    _bus->transfer(cmd);
    _bus->transfer(static_cast<uint8_t>(reg));
    portA = _bus->transfer(0xFF);
    portB = _bus->transfer(0xFF);
    digitalWrite(_cs, HIGH);
    return 0;
}