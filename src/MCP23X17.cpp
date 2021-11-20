#include "MCP23X17.h"

void MCP23X17::init()
{
	//BANK = 	0 : sequential register addresses
	//MIRROR = 	0 : use configureInterrupt 
	//SEQOP = 	1 : sequential operation disabled, address pointer does not increment
	//DISSLW = 	0 : slew rate enabled
	//HAEN = 	0 : hardware address pin is always enabled on 23017
	//ODR = 	0 : open drain output
	//INTPOL = 	0 : interrupt active low
	writeRegister(MCP23X17Register::IOCON, 0b00100000);

	//enable all pull up resistors (will be effective for input pins only)
	writeRegister(MCP23X17Register::GPPU_A, 0xFF, 0xFF);
}

void MCP23X17::portMode(MCP23X17Port port, uint8_t directions, uint8_t pullups, uint8_t inverted)
{
	writeRegister(MCP23X17Register::IODIR_A + port, directions);
	writeRegister(MCP23X17Register::GPPU_A + port, pullups);
	writeRegister(MCP23X17Register::IPOL_A + port, inverted);
}

void MCP23X17::pinMode(uint8_t pin, uint8_t mode, bool inverted)
{
	MCP23X17Port port = (MCP23X17Port)MCP_PORT(pin);
	pin = MCP_PORTPIN(pin);
	MCP23X17Register iodirreg = MCP23X17Register::IODIR_A+port;
	MCP23X17Register pullupreg = MCP23X17Register::GPPU_A+port;
	MCP23X17Register polreg = MCP23X17Register::IPOL_A+port;
	uint8_t iodir, pol, pull;

	iodir = readRegister(iodirreg);
	if(mode == INPUT || mode == INPUT_PULLUP) bitSet(iodir, pin);
	else bitClear(iodir, pin);

	pull = readRegister(pullupreg);
	if(mode == INPUT_PULLUP) bitSet(pull, pin);
	else bitClear(pull, pin);

	pol = readRegister(polreg);
	if(inverted) bitSet(pol, pin);
	else bitClear(pol, pin);

	writeRegister(iodirreg, iodir);
	writeRegister(pullupreg, pull);
	writeRegister(polreg, pol);
}

void MCP23X17::digitalWrite(uint8_t pin, uint8_t state)
{
	MCP23X17Port port = (MCP23X17Port)MCP_PORT(pin);
	pin = MCP_PORTPIN(pin);
	MCP23X17Register gpioreg = MCP23X17Register::GPIO_A+port;
	uint8_t gpio;

	gpio = readRegister(gpioreg);
	if(state == HIGH) bitSet(gpio, pin);
	else bitClear(gpio, pin);

	writeRegister(gpioreg, gpio);
}

uint8_t MCP23X17::digitalRead(uint8_t pin)
{
	MCP23X17Port port = (MCP23X17Port)MCP_PORT(pin);
	pin = MCP_PORTPIN(pin);
	MCP23X17Register gpioreg = MCP23X17Register::GPIO_A+port;
	uint8_t gpio;

	gpio = readRegister(gpioreg);
	if(bitRead(gpio, pin)) return HIGH;
	return LOW;
}

void MCP23X17::writePort(MCP23X17Port port, uint8_t value)
{
	writeRegister(MCP23X17Register::GPIO_A + port, value);
}

void MCP23X17::write(uint16_t value)
{
	writeRegister(MCP23X17Register::GPIO_A, lowByte(value), highByte(value));
}

uint8_t MCP23X17::readPort(MCP23X17Port port)
{
	return readRegister(MCP23X17Register::GPIO_A + port);
}

uint16_t MCP23X17::read()
{
	uint8_t a, b;
	readRegister(MCP23X17Register::GPIO_A, a, b);
	return a | b << 8;
}

#ifdef _MCP23X17_INTERRUPT_SUPPORT_

void MCP23X17::interruptMode(MCP23X17InterruptMode intMode)
{
	uint8_t iocon = readRegister(MCP23X17Register::IOCON);
	if(intMode == MCP23X17InterruptMode::Or) iocon |= static_cast<uint8_t>(MCP23X17InterruptMode::Or);
	else iocon &= ~(static_cast<uint8_t>(MCP23X17InterruptMode::Or));

	writeRegister(MCP23X17Register::IOCON, iocon);
}

void MCP23X17::interrupt(MCP23X17Port port, uint8_t mode)
{
	MCP23X17Register defvalreg = MCP23X17Register::DEFVAL_A + port;
	MCP23X17Register intconreg = MCP23X17Register::INTCON_A + port;

	//enable interrupt for port
	writeRegister(MCP23X17Register::GPINTEN_A + port, 0xFF);
	switch(mode)
	{
	case CHANGE:
		//interrupt on change
		writeRegister(intconreg, 0);
		break;
	case FALLING:
		//interrupt falling : compared against defval, 0xff
		writeRegister(intconreg, 0xFF);
		writeRegister(defvalreg, 0xFF);
		break;
	case RISING:
		//interrupt rising : compared against defval, 0x00
		writeRegister(intconreg, 0xFF);
		writeRegister(defvalreg, 0x00);
		break;
	}
}

void MCP23X17::interruptedBy(uint8_t& portA, uint8_t& portB)
{
	readRegister(MCP23X17Register::INTF_A, portA, portB);
}

void MCP23X17::disableInterrupt(MCP23X17Port port)
{
	writeRegister(MCP23X17Register::GPINTEN_A + port, 0x00);
}

void MCP23X17::clearInterrupts()
{
	uint8_t a, b;
	clearInterrupts(a, b);
}

void MCP23X17::clearInterrupts(uint8_t& portA, uint8_t& portB)
{
	readRegister(MCP23X17Register::INTCAP_A, portA, portB);
}

#endif