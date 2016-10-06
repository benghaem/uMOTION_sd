/*
 * spi_control.c
 *
 * Created: 10/2/2016 20:37:26
 *  Author: Ben
 */ 
 #include <asf.h>
 #include "spi_control.h"

struct spi_module spi_master_instance;
struct spi_slave_inst flash_slave;

volatile uint8_t buffer_trancv_complete = 0;

void configure_spi_master(void){
	struct spi_config config_spi_master;
	struct spi_slave_inst_config flash_slave_dev_config;

	spi_slave_inst_get_config_defaults(&flash_slave_dev_config);

	flash_slave_dev_config.ss_pin = FLASH_SLAVE_SELECT_PIN;

	spi_attach_slave(&flash_slave, &flash_slave_dev_config);
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.clock_divider = 8;
	config_spi_master.transfer_mode = SPI_TRANSFER_MODE_0;

	config_spi_master.pin_number_pad[0] = EXT1_SPI_PIN_PAD0;
	config_spi_master.pinmux_sel_pad[0] = EXT1_SPI_MUX_PAD0;

	config_spi_master.pin_number_pad[1] = EXT1_SPI_PIN_PAD1;
	config_spi_master.pinmux_sel_pad[1] = EXT1_SPI_MUX_PAD1;

	config_spi_master.pin_number_pad[2] = EXT1_SPI_PIN_PAD2;
	config_spi_master.pinmux_sel_pad[2] = EXT1_SPI_MUX_PAD2;

	config_spi_master.pin_number_pad[3] = EXT1_SPI_PIN_PAD3;
	config_spi_master.pinmux_sel_pad[3] = EXT1_SPI_MUX_PAD3;

	spi_init(&spi_master_instance, EXT1_SPI_MODULE, &config_spi_master);
	spi_enable(&spi_master_instance);
}

void configure_spi_master_callbacks(void){
	spi_register_callback(&spi_master_instance, callback_spi_master_trancv, SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_master_instance, SPI_CALLBACK_BUFFER_TRANSCEIVED);
}

void callback_spi_master_trancv(struct spi_module *const module){
	buffer_trancv_complete = 1;
}