#pragma once

#include <Arduino.h>

#define _MCP23X17_INTERRUPT_SUPPORT_ ///< Enables support for MCP23X17 interrupts.
#define MCP_PORT(pin) ((pin < 8) ? 0 : 1)
#define MCP_PORTPIN(pin) ((pin < 8) ? pin : (pin-8))

enum class MCP23X17Port : uint8_t
{
	A = 0,
	B = 1
};

/**
 * Controls if the two interrupt pins mirror each other.
 * See "3.6 Interrupt Logic".
 */
enum class MCP23X17InterruptMode : uint8_t
{
	Separated = 0,	///< Interrupt pins are kept independent
	Or = 0b01000000	///< Interrupt pins are mirrored
};

/**
 * Registers addresses.
 * The library use addresses for IOCON.BANK = 0.
 * See "3.2.1 Byte mode and Sequential mode".
 */
enum class MCP23X17Register : uint8_t
{
	IODIR_A		= 0x00, 		///< Controls the direction of the data I/O for port A.
	IODIR_B		= 0x01,			///< Controls the direction of the data I/O for port B.
	IPOL_A		= 0x02,			///< Configures the polarity on the corresponding GPIO_ port bits for port A.
	IPOL_B		= 0x03,			///< Configures the polarity on the corresponding GPIO_ port bits for port B.
	GPINTEN_A	= 0x04,			///< Controls the interrupt-on-change for each pin of port A.
	GPINTEN_B	= 0x05,			///< Controls the interrupt-on-change for each pin of port B.
	DEFVAL_A	= 0x06,			///< Controls the default comparaison value for interrupt-on-change for port A.
	DEFVAL_B	= 0x07,			///< Controls the default comparaison value for interrupt-on-change for port B.
	INTCON_A	= 0x08,			///< Controls how the associated pin value is compared for the interrupt-on-change for port A.
	INTCON_B	= 0x09,			///< Controls how the associated pin value is compared for the interrupt-on-change for port B.
	IOCON		= 0x0A,			///< Controls the device.
	GPPU_A		= 0x0C,			///< Controls the pull-up resistors for the port A pins.
	GPPU_B		= 0x0D,			///< Controls the pull-up resistors for the port B pins.
	INTF_A		= 0x0E,			///< Reflects the interrupt condition on the port A pins.
	INTF_B		= 0x0F,			///< Reflects the interrupt condition on the port B pins.
	INTCAP_A	= 0x10,			///< Captures the port A value at the time the interrupt occured.
	INTCAP_B	= 0x11,			///< Captures the port B value at the time the interrupt occured.
	GPIO_A		= 0x12,			///< Reflects the value on the port A.
	GPIO_B		= 0x13,			///< Reflects the value on the port B.
	OLAT_A		= 0x14,			///< Provides access to the port A output latches.
	OLAT_B		= 0x15,			///< Provides access to the port B output latches.
};

inline MCP23X17Register operator+(MCP23X17Register a, MCP23X17Port b) {
	return static_cast<MCP23X17Register>(static_cast<uint8_t>(a) + static_cast<uint8_t>(b));
};

class MCP23X17
{
public:
	/**
	 * Instantiates a new instance to interact with a MCP23X17 at the specified address.
	 */
	MCP23X17() = default;
#ifdef _DEBUG
	void debug();
#endif
	/**
	 * Initializes the chip with the default configuration.
	 * Enables Byte mode (IOCON.BANK = 0 and IOCON.SEQOP = 1).
	 * Enables pull-up resistors for all pins. This will only be effective for input pins.
	 * 
	 * See "3.2.1 Byte mode and Sequential mode".
	 */
	uint8_t init();
	/**
	 * Controls the pins direction on a whole port at once.
	 * 
	 * 1 = Pin is configured as an input.
	 * 0 = Pin is configured as an output.
	 * 
	 * See "3.5.1 I/O Direction register".
	 */
	uint8_t portMode(MCP23X17Port port, uint8_t directions, uint8_t pullups = 0xFF, uint8_t inverted = 0x00);
	/**
	 * Controls a single pin direction. 
	 * Pin 0-7 for port A, 8-15 fo port B.
	 * 
	 * 1 = Pin is configured as an input.
	 * 0 = Pin is configured as an output.
	 *
	 * See "3.5.1 I/O Direction register".
	 * 
	 * Beware!  
	 * On Arduino platform, INPUT = 0, OUTPUT = 1, which is the inverse
	 * of the MCP23X17 definition where a pin is an input if its IODIR bit is set to 1.
	 * This library pinMode function behaves like Arduino's standard pinMode for consistency.
	 * [ OUTPUT | INPUT | INPUT_PULLUP ]
	 */
	uint8_t pinMode(uint8_t pin, uint8_t mode, bool inverted = false);

	/**
	 * Writes a single pin state.
	 * Pin 0-7 for port A, 8-15 for port B.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	uint8_t digitalWrite(uint8_t pin, uint8_t state);
	/**
	 * Reads a single pin state.
	 * Pin 0-7 for port A, 8-15 for port B.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */ 
	uint8_t digitalRead(uint8_t pin, uint8_t& buffer);

	/**
	 * Writes pins state to a whole port.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	uint8_t writePort(MCP23X17Port port, uint8_t value);
	/**
	 * Writes pins state to both ports.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	uint8_t write(uint16_t value);

	/**
	 * Reads pins state for a whole port.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	uint8_t readPort(MCP23X17Port port, uint8_t& buffer);
	/**
	 * Reads pins state for both ports. 
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	uint8_t read(uint16_t& buffer);

	/**
	 * Writes a single register value.
	 */
	virtual uint8_t writeRegister(MCP23X17Register reg, uint8_t value);
	/**
	 * Writes values to a register pair.
	 * 
	 * For portA and portB variable to effectively match the desired port,
	 * you have to supply a portA register address to reg. Otherwise, values
	 * will be reversed due to the way the MCP23X17 works in Byte mode.
	 */
	virtual uint8_t writeRegister(MCP23X17Register reg, uint8_t portA, uint8_t portB);
	/**
	 * Reads a single register value.
	 */
	virtual uint8_t readRegister(MCP23X17Register reg, uint8_t& buffer);
	/**
	 * Reads the values from a register pair.
	 * 
	 * For portA and portB variable to effectively match the desired port,
	 * you have to supply a portA register address to reg. Otherwise, values
	 * will be reversed due to the way the MCP23X17 works in Byte mode.
	 */
	virtual uint8_t readRegister(MCP23X17Register reg, uint8_t& portA, uint8_t& portB);

#ifdef _MCP23X17_INTERRUPT_SUPPORT_

	/**
	 * Controls how the interrupt pins act with each other.
	 * If intMode is SEPARATED, interrupt conditions on a port will cause its respective INT pin to active.
	 * If intMode is OR, interrupt pins are OR'ed so an interrupt on one of the port will cause both pints to active.
	 * 
	 * Controls the IOCON.MIRROR bit. 
	 * See "3.5.6 Configuration register".
	 */
	uint8_t interruptMode(MCP23X17InterruptMode intMode);
	/**
	 * Configures interrupt registers using an Arduino-like API.
	 * mode can be one of CHANGE, FALLING or RISING.
	 */
	uint8_t interrupt(MCP23X17Port port, uint8_t mode);
	/**
	 * Reads which pin caused the interrupt.
	 */
	uint8_t interruptedBy(uint8_t& portA, uint8_t& portB);
	/**
	 * Disable interrupts for the specified port.
	 */
	uint8_t disableInterrupt(MCP23X17Port port);
	/**
	 * Clears interrupts on both ports.
	 */
	uint8_t clearInterrupts();
	/**
	 * Clear interrupts on both ports. Returns port values at the time the interrupt occured.
	 */
	uint8_t clearInterrupts(uint8_t& portA, uint8_t& portB);

#endif
};