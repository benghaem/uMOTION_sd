/*
 * samb11_drv.h
 *
 * Created: 10/2/2016 20:27:09
 *  Author: Ben
 */ 


#ifndef _SAMB11_DRV_H_
#define _SAMB11_DRV_H_

#include <stdint.h>

enum sd_card_type{
	VERSION2_BLOCK = 0,
	VERSION2_BYTE = 1,
	VERSION1 = 2,
	MMC_VERSION3 = 3,
	UNKNOWN_CARD = 4,
};

enum sd_card_type setup_samb11_drv(uint8_t spi_target);
void close_samb11_drv(void);

uint8_t get_response(void);
void get_r7_response(uint8_t* rx_data, uint32_t* rx_r7_data);
int write_byte(enum sd_card_type type, uint8_t byte, unsigned int addr);
int write_byte_array(uint8_t *bytes, unsigned int len, unsigned int addr);
int read_byte(uint8_t *byte, unsigned int addr);
int read_byte_array(uint8_t *bytes, unsigned int len,  unsigned int addr);

void read_in(void);
void write_out(void);


#endif /* _SAMB11_DRV_H_ */