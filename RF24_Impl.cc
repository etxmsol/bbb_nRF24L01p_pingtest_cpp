/*
 * RF24_Impl.cc
 *
 *  Created on: 8 aug 2015
 *      Author: Mikhail
 */
#include <compatibility.h>
#include <RF24_Impl.h>
#include <spi.h>
#include <gpio.h>
#include <nRF24L01.h>

unsigned char  TX_ADDRESS[ADR_WIDTH]= {0xE1,0xF0,0xF0,0xF0,0xF0};
unsigned char  RX_ADDRESS[ADR_WIDTH]= {0xD0,0xD0,0xD0,0xD0,0xD0};

const uint8_t CE_PIN = 115;
const uint8_t CSN_PIN = 117;

RF24_Impl::RF24_Impl() : RF24(TX_ADDRESS, RX_ADDRESS)
{
	GPIO::open(CE_PIN, GPIO::DIRECTION_OUT);
	GPIO::open(CSN_PIN, GPIO::DIRECTION_OUT);
};


void RF24_Impl::delay(uint32_t ms)
{
	__msleep(ms);
}


void RF24_Impl::csnHigh()
{
	GPIO::write(CSN_PIN, 1);
}

void RF24_Impl::csnLow()
{
	GPIO::write(CSN_PIN, 0);
}

void RF24_Impl::ceHigh()
{
	GPIO::write(CE_PIN, 1);
}

void RF24_Impl::ceLow()
{
	GPIO::write(CE_PIN, 0);
}

SPI * spi = NULL;

uint8_t RF24_Impl::spiTransfer(uint8_t v)
{
	if(spi == NULL)
	{
		spi = new SPI;
	}

	return spi->transfer(v);
}

/****************************************************************************/

static const char rf24_datarate_e_str_0[] PROGMEM = "1MBPS";
static const char rf24_datarate_e_str_1[] PROGMEM = "2MBPS";
static const char rf24_datarate_e_str_2[] PROGMEM = "250KBPS";
static const char * const rf24_datarate_e_str_P[] PROGMEM = {
  rf24_datarate_e_str_0,
  rf24_datarate_e_str_1,
  rf24_datarate_e_str_2,
};
static const char rf24_model_e_str_0[] PROGMEM = "nRF24L01";
static const char rf24_model_e_str_1[] PROGMEM = "nRF24L01+";
static const char * const rf24_model_e_str_P[] PROGMEM = {
  rf24_model_e_str_0,
  rf24_model_e_str_1,
};
static const char rf24_crclength_e_str_0[] PROGMEM = "Disabled";
static const char rf24_crclength_e_str_1[] PROGMEM = "8 bits";
static const char rf24_crclength_e_str_2[] PROGMEM = "16 bits" ;
static const char * const rf24_crclength_e_str_P[] PROGMEM = {
  rf24_crclength_e_str_0,
  rf24_crclength_e_str_1,
  rf24_crclength_e_str_2,
};
static const char rf24_pa_dbm_e_str_0[] PROGMEM = "PA_MIN";
static const char rf24_pa_dbm_e_str_1[] PROGMEM = "PA_LOW";
static const char rf24_pa_dbm_e_str_2[] PROGMEM = "LA_MED";
static const char rf24_pa_dbm_e_str_3[] PROGMEM = "PA_HIGH";
static const char * const rf24_pa_dbm_e_str_P[] PROGMEM = {
  rf24_pa_dbm_e_str_0,
  rf24_pa_dbm_e_str_1,
  rf24_pa_dbm_e_str_2,
  rf24_pa_dbm_e_str_3,
};

void RF24_Impl::printDetails(void)
{
  print_status(get_status());

  print_address_register(PSTR("RX_ADDR_P0-1"),RX_ADDR_P0,2);
  print_byte_register(PSTR("RX_ADDR_P2-5"),RX_ADDR_P2,4);
  print_address_register(PSTR("TX_ADDR"),TX_ADDR);

  print_byte_register(PSTR("RX_PW_P0-6"),RX_PW_P0,6);
  print_byte_register(PSTR("EN_AA"),EN_AA);
  print_byte_register(PSTR("EN_RXADDR"),EN_RXADDR);
  print_byte_register(PSTR("RF_CH"),RF_CH);
  print_byte_register(PSTR("RF_SETUP"),RF_SETUP);
  print_byte_register(PSTR("CONFIG"),CONFIG);
  print_byte_register(PSTR("DYNPD/FEATURE"),DYNPD,2);

  printf_P(PSTR("Data Rate\t = %s\r\n"),pgm_read_word(&rf24_datarate_e_str_P[getDataRate()]));
  printf_P(PSTR("Model\t\t = %s\r\n"),pgm_read_word(&rf24_model_e_str_P[isPVariant()]));
  printf_P(PSTR("CRC Length\t = %s\r\n"),pgm_read_word(&rf24_crclength_e_str_P[getCRCLength()]));
  printf_P(PSTR("PA Power\t = %s\r\n"),pgm_read_word(&rf24_pa_dbm_e_str_P[getPALevel()]));
}

/****************************************************************************/

void RF24_Impl::print_status(uint8_t status)
{
  printf_P(PSTR("STATUS\t\t = 0x%02x RX_DR=%x TX_DS=%x MAX_RT=%x RX_P_NO=%x TX_FULL=%x\r\n"),
           status,
           (status & _BV(RX_DR))?1:0,
           (status & _BV(TX_DS))?1:0,
           (status & _BV(MAX_RT))?1:0,
           ((status >> RX_P_NO) & 0b111),
           (status & _BV(TX_FULL))?1:0
          );
}

/****************************************************************************/

void RF24_Impl::print_address_register(const char* name, uint8_t reg, uint8_t qty)
{
  char extra_tab = strlen_P(name) < 8 ? '\t' : 0;
  printf_P(PSTR(PRIPSTR"\t%c ="),name,extra_tab);

  while (qty--)
  {
    uint8_t buffer[5];
    read_register(reg++,buffer,sizeof buffer);

    printf_P(PSTR(" 0x"));
    uint8_t* bufptr = buffer + sizeof buffer;
    while( --bufptr >= buffer )
      printf_P(PSTR("%02x"),*bufptr);
  }

  printf_P(PSTR("\r\n"));
}

/****************************************************************************/

void RF24_Impl::print_byte_register(const char* name, uint8_t reg, uint8_t qty)
{
  char extra_tab = strlen_P(name) < 8 ? '\t' : 0;
  printf_P(PSTR(PRIPSTR"\t%c ="),name,extra_tab);
  while (qty--)
    printf_P(PSTR(" 0x%02x"),read_register(reg++));
  printf_P(PSTR("\r\n"));
}
