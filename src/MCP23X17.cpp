#include "MCP23X17.h"

uint8_t MCP23X17::init()
{
	uint8_t err = 0;
	//BANK = 	0 : sequential register addresses
	//MIRROR = 	0 : use configureInterrupt 
	//SEQOP = 	1 : sequential operation disabled, address pointer does not increment
	//DISSLW = 	0 : slew rate enabled
	//HAEN = 	0 : hardware address pin is always enabled on 23017
	//ODR = 	0 : open drain output
	//INTPOL = 	0 : interrupt active low
	err = writeRegister(MCP23X17Register::IOCON, 0b00100000);
	if (err != 0) {
		return err;
	}

	//enable all pull up resistors (will be effective for input pins only)
	return writeRegister(MCP23X17Register::GPPU_A, 0xFF, 0xFF);
}

uint8_t MCP23X17::portMode(MCP23X17Port port, uint8_t directions, uint8_t pullups, uint8_t inverted)
{
	uint8_t err = 0;
	err = writeRegister(MCP23X17Register::IODIR_A + port, directions);
	if (err != 0) {
		return err;
	}
	err = writeRegister(MCP23X17Register::GPPU_A + port, pullups);
	if (err != 0) {
		return err;
	}
	return writeRegister(MCP23X17Register::IPOL_A + port, inverted);
}

uint8_t MCP23X17::pinMode(uint8_t pin, uint8_t mode, bool inverted)
{
	uint8_t err = 0;
	MCP23X17Port port = (MCP23X17Port)MCP_PORT(pin);
	pin = MCP_PORTPIN(pin);
	MCP23X17Register iodirreg = MCP23X17Register::IODIR_A+port;
	MCP23X17Register pullupreg = MCP23X17Register::GPPU_A+port;
	MCP23X17Register polreg = MCP23X17Register::IPOL_A+port;
	uint8_t iodir, pol, pull;

	err = readRegister(iodirreg, iodir);
	if (err != 0) {
		return err;
	}
	if(mode == INPUT || mode == INPUT_PULLUP) bitSet(iodir, pin);
	else bitClear(iodir, pin);

	err = readRegister(pullupreg, pull);
	if (err != 0) {
		return err;
	}
	if(mode == INPUT_PULLUP) bitSet(pull, pin);
	else bitClear(pull, pin);

	err = readRegister(polreg, pol);
	if (err != 0) {
		return err;
	}
	if(inverted) bitSet(pol, pin);
	else bitClear(pol, pin);

	err = writeRegister(iodirreg, iodir);
	if (err != 0) {
		return err;
	}
	err = writeRegister(pullupreg, pull);
	if (err != 0) {
		return err;
	}
	return writeRegister(polreg, pol);
}

uint8_t MCP23X17::digitalWrite(uint8_t pin, uint8_t state)
{
	MCP23X17Port port = (MCP23X17Port)MCP_PORT(pin);
	pin = MCP_PORTPIN(pin);
	MCP23X17Register gpioreg = MCP23X17Register::GPIO_A+port;
	uint8_t gpio;

	uint8_t err = readRegister(gpioreg, gpio);
	if (err != 0) { return err; }
	if(state == HIGH) bitSet(gpio, pin);
	else bitClear(gpio, pin);

	return writeRegister(gpioreg, gpio);
}

uint8_t MCP23X17::digitalRead(uint8_t pin, uint8_t& buffer)
{
	MCP23X17Port port = (MCP23X17Port)MCP_PORT(pin);
	pin = MCP_PORTPIN(pin);
	MCP23X17Register gpioreg = MCP23X17Register::GPIO_A+port;
	uint8_t gpio;

	uint8_t err = readRegister(gpioreg, gpio);
	if (err != 0) { return err; }
	if(bitRead(gpio, pin)) { 
		buffer = HIGH; 
		return 0;
	}
	buffer = LOW;
	return 0;
}

uint8_t MCP23X17::writePort(MCP23X17Port port, uint8_t value)
{
	return writeRegister(MCP23X17Register::GPIO_A + port, value);
}

uint8_t MCP23X17::write(uint16_t value)
{
	return writeRegister(MCP23X17Register::GPIO_A, lowByte(value), highByte(value));
}

uint8_t MCP23X17::readPort(MCP23X17Port port, uint8_t& buffer)
{
	return readRegister(MCP23X17Register::GPIO_A + port, buffer);
}

uint8_t MCP23X17::read(uint16_t& buffer)
{
	uint8_t a, b;
	uint8_t err = readRegister(MCP23X17Register::GPIO_A, a, b);
	if (err != 0) { return err; }
	buffer = a | b << 8;
	return 0;
}

#ifdef _MCP23X17_INTERRUPT_SUPPORT_

uint8_t MCP23X17::interruptMode(MCP23X17InterruptMode intMode)
{
	uint8_t iocon = 0;
	uint8_t err = readRegister(MCP23X17Register::IOCON, iocon);
	if (err != 0) { return err; }
	if(intMode == MCP23X17InterruptMode::Or) iocon |= static_cast<uint8_t>(MCP23X17InterruptMode::Or);
	else iocon &= ~(static_cast<uint8_t>(MCP23X17InterruptMode::Or));

	return writeRegister(MCP23X17Register::IOCON, iocon);
}

uint8_t MCP23X17::interrupt(MCP23X17Port port, uint8_t mode)
{
	uint8_t err = 0;
	MCP23X17Register defvalreg = MCP23X17Register::DEFVAL_A + port;
	MCP23X17Register intconreg = MCP23X17Register::INTCON_A + port;

	//enable interrupt for port
	err = writeRegister(MCP23X17Register::GPINTEN_A + port, 0xFF);
	if (err != 0) {
		return err;
	}
	switch(mode)
	{
	case CHANGE:
		//interrupt on change
		return writeRegister(intconreg, 0);
	case FALLING:
		//interrupt falling : compared against defval, 0xff
		err = writeRegister(intconreg, 0xFF);
		if (err != 0) { return err; }
		return writeRegister(defvalreg, 0xFF);
	case RISING:
		//interrupt rising : compared against defval, 0x00
		err = writeRegister(intconreg, 0xFF);
		if (err != 0) { return err; }
		return writeRegister(defvalreg, 0x00);
	}
}

uint8_t MCP23X17::interruptedBy(uint8_t& portA, uint8_t& portB)
{
	return readRegister(MCP23X17Register::INTF_A, portA, portB);
}

uint8_t MCP23X17::disableInterrupt(MCP23X17Port port)
{
	return writeRegister(MCP23X17Register::GPINTEN_A + port, 0x00);
}

uint8_t MCP23X17::clearInterrupts()
{
	uint8_t a, b;
	return clearInterrupts(a, b);
}

uint8_t MCP23X17::clearInterrupts(uint8_t& portA, uint8_t& portB)
{
	return readRegister(MCP23X17Register::INTCAP_A, portA, portB);
}

#endif