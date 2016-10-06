/*
 * spi_control.h
 *
 * Created: 10/2/2016 20:37:39
 *  Author: Ben
 */ 


#ifndef SPI_CONTROL_H_
#define SPI_CONTROL_H_

#define FLASH_SLAVE_SELECT_PIN EXT1_PIN_SPI_SS_0;

#include "samb11_xplained_pro.h"


extern struct spi_module spi_master_instance;
extern struct spi_slave_inst flash_slave;

void configure_spi_master(void);

void configure_spi_master_callbacks(void);

void callback_spi_master_trancv(struct spi_module *const module);


#endif /* SPI_CONTROL_H_ */