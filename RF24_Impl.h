#ifndef __RF24_IMPL
#define __RF24_IMPL

#define ADR_WIDTH	5


#include <RF24.h>


class RF24_Impl : public RF24
{
public:
	RF24_Impl();
	virtual ~RF24_Impl() {};

public:
	// system interface
	  virtual void delay(uint32_t ms);

	// implementation of the GPIO interface
	  /**
	   * Set chip select pin
	   */
	  virtual void csnHigh();
	  virtual void csnLow();

	  /**
	   * Set chip enable/disable
	   */
	  virtual void ceHigh();
	  virtual void ceLow();

	  // implementation of SPI interface
	  virtual uint8_t spiTransfer(uint8_t);

	  void printDetails(void);
	  void print_status(uint8_t status);

	  /**
	   * Print the name and value of a 40-bit address register to stdout
	   *
	   * Optionally it can print some quantity of successive
	   * registers on the same line.  This is useful for printing a group
	   * of related registers on one line.
	   *
	   * @param name Name of the register
	   * @param reg Which register. Use constants from nRF24L01.h
	   * @param qty How many successive registers to print
	   */
	  void print_address_register(const char* name, uint8_t reg, uint8_t qty=1);

	  /**
	   * Print the name and value of an 8-bit register to stdout
	   *
	   * Optionally it can print some quantity of successive
	   * registers on the same line.  This is useful for printing a group
	   * of related registers on one line.
	   *
	   * @param name Name of the register
	   * @param reg Which register. Use constants from nRF24L01.h
	   * @param qty How many successive registers to print
	   */
	  void print_byte_register(const char* name, uint8_t reg, uint8_t qty = 1);



};


#endif
