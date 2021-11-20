#include "MCP23017.h"


MCP23017::MCP23017(uint8_t address) : MCP23X17() {
	_deviceAddr = address;
	_bus = &Wire;
}

MCP23017::MCP23017(uint8_t address, TwoWire *bus) : MCP23X17() {
	_deviceAddr = address;
	_bus = bus;
}

MCP23017::~MCP23017() {}

bool MCP23017::checkAck() {
    int error;
    _bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(MCP23X17Register::GPIO_A));
	error = _bus->endTransmission();

    if(error != 0){
      return false;
    }else{
      return true;
    }
}

void MCP23017::begin(int sdaPin, int sclPin)
{
	_bus->setPins(sdaPin, sclPin);
	_bus->begin();
}

void MCP23017::writeRegister(MCP23X17Register reg, uint8_t value)
{
	_bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(reg));
	_bus->write(value);
	_bus->endTransmission();
}

void MCP23017::writeRegister(MCP23X17Register reg, uint8_t portA, uint8_t portB)
{
	_bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(reg));
	_bus->write(portA);
	_bus->write(portB);
	_bus->endTransmission();
}


uint8_t MCP23017::readRegister(MCP23X17Register reg)
{
	_bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(reg));
	_bus->endTransmission(false);
	_bus->requestFrom(_deviceAddr, (uint8_t)1);
	return _bus->read();
}

void MCP23017::readRegister(MCP23X17Register reg, uint8_t& portA, uint8_t& portB)
{
	_bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(reg));
	_bus->endTransmission();
	_bus->requestFrom(_deviceAddr, (uint8_t)2);
	portA = _bus->read();
	portB = _bus->read();
}
