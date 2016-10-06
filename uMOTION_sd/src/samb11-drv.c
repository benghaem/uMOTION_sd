/*
 * samb11_drv.c
 *
 * Created: 10/2/2016 20:26:44
 *  Author: Ben
 */ 

 #include "samb11-drv.h"
 #include "spi_control.h"
 #include "board.h"
 #include <asf.h>


static uint8_t CMD0[6] = {0x40,0x00,0x00,0x00,0x00,0x95};
static uint8_t CMD1[6] = {0x41,0x00,0x00,0x00,0x00,0xF9};
static uint8_t CMD8[6] = {0x48,0x00,0x00,0x01,0xAA,0x87};
static uint8_t CMD16[6] = {0x50,0x00,0x00,0x02,0x00,0xFF};
static uint8_t CMD41_HCS[6] = {0x69,0x40,0x00,0x00,0x00,0xFF}; 
static uint8_t CMD41[6] = {0x69,0x00,0x00,0x00,0x00,0xFF}; 
static uint8_t CMD55[6] = {0x77,0x00,0x00,0x00,0x00,0x65};
static uint8_t CMD58[6] = {0x7A,0x00,0x00,0x00,0x00,0xFF};

 enum sd_card_type setup_samb11_drv(uint8_t spi_target){
	uint8_t rx_data = 0xFF;
	uint32_t rx_r7_data = 0;
	//POWER ON CARD
	struct gpio_config sd_power;
	sd_power.direction = GPIO_PIN_DIR_OUTPUT;
	sd_power.input_pull = GPIO_PIN_PULL_NONE;
	
	gpio_pin_set_config(EXT1_PIN_5, &sd_power);

	gpio_pin_set_output_level(EXT1_PIN_5,LOW);
	gpio_pin_set_output_level(EXT1_PIN_5,HIGH);

	for (uint32_t d = 2000; d > 0; d--){
		asm(" NOP");
	}
	
	//card high	
	spi_select_slave(&spi_master_instance, &flash_slave, false);
	// MOSI HIGH
	// Dummy Clocks
	for (int i = 10; i > 0; i--){
		if (spi_write(&spi_master_instance, 0xFF) == STATUS_BUSY){
			for(int j = 8; j > 0; j--){
				asm(" NOP");
			}
		};
	}
	//LOW//
	spi_select_slave(&spi_master_instance, &flash_slave, true);
	do{	
		spi_write_buffer_wait(&spi_master_instance, CMD0, 6);	
		rx_data = get_response();
	} while (rx_data != 0x01);
	spi_select_slave(&spi_master_instance, &flash_slave, false);
	spi_write(&spi_master_instance, 0xFF);
	//HIGH//


	//LOW//
	spi_select_slave(&spi_master_instance, &flash_slave, true);
	spi_write_buffer_wait(&spi_master_instance, CMD8, 6);
	get_r7_response(&rx_data, &rx_r7_data);
	spi_select_slave(&spi_master_instance, &flash_slave, false);	
	spi_write(&spi_master_instance, 0xFF);
	//HIGH//

	//if rx == 0x01 we are ready to check for HC card and voltage
	if (rx_data == 0x01){
		if ((rx_r7_data & 0x00000FFF) == 0x000001AA){
			//matched expected value
			//good works with voltage 
		
			//LOW//	
			spi_select_slave(&spi_master_instance, &flash_slave, true);		
			spi_write_buffer_wait(&spi_master_instance, CMD55, 6);
			rx_data = get_response();
			spi_select_slave(&spi_master_instance, &flash_slave, false);
			spi_write(&spi_master_instance, 0xFF);
			//HIGH//

			//LOW//
			spi_select_slave(&spi_master_instance, &flash_slave, true);
			spi_write_buffer_wait(&spi_master_instance, CMD41_HCS, 6);
			rx_data = get_response();
			spi_select_slave(&spi_master_instance, &flash_slave, false);
			spi_write(&spi_master_instance, 0xFF);
			//HIGH//

			//If we are busy repeat the above
			if (rx_data == 0x01){
				do {	
					//LOW//
					spi_select_slave(&spi_master_instance, &flash_slave, true);
					spi_write_buffer_wait(&spi_master_instance, CMD55, 6);
					rx_data = get_response();
					spi_select_slave(&spi_master_instance, &flash_slave, false);
					spi_write(&spi_master_instance, 0xFF);
					//HIGH//

					//LOW//
					spi_select_slave(&spi_master_instance, &flash_slave, true);		
					spi_write_buffer_wait(&spi_master_instance, CMD41_HCS, 6);
					rx_data = get_response();
					spi_select_slave(&spi_master_instance, &flash_slave, false);
					spi_write(&spi_master_instance, 0xFF);
					//HIGH//
				} while (rx_data == 0x01);
			}

			//We did not hang which means we can now check for HC card bit
			if (rx_data == 0x00){
				do {
					//LOW//
					spi_select_slave(&spi_master_instance, &flash_slave, true);
					spi_write_buffer_wait(&spi_master_instance, CMD58, 6);
					get_r7_response(&rx_data, &rx_r7_data);
					spi_select_slave(&spi_master_instance, &flash_slave, false);
					spi_write(&spi_master_instance, 0xFF);
					//HIGH//
				} while (rx_data == 0x01);
			
				//If this bit is set we are a version 2 block card
				if ((rx_r7_data & 0x40000000) >> 30 == 0x01){
					return VERSION2_BLOCK;
				} else {
					// otherwise we are byte addressed card which needs to be 
					// set to use a block size of 512 bytes
					
					//LOW//
					spi_select_slave(&spi_master_instance, &flash_slave, true);
					spi_write_buffer_wait(&spi_master_instance, CMD16, 6);
					rx_data = get_response();
					if (rx_data != 0x00){
						return UNKNOWN_CARD;
					}
					spi_select_slave(&spi_master_instance, &flash_slave, false);
					spi_write(&spi_master_instance, 0xFF);
					//HIGH//

					return VERSION2_BYTE;
				}
			} else {
				//An error occured
				return UNKNOWN_CARD;
			}
		} else {
			//we did not match the expected value
			return UNKNOWN_CARD;
		} 
	} else {		
		//CMD8 output an error
		//Thus we are either a MMC or V1 Card
		//The code below is not well tested
		int count = 0;
		do{

			spi_select_slave(&spi_master_instance, &flash_slave, true);	
			spi_write_buffer_wait(&spi_master_instance, CMD55, 6);
			spi_write_buffer_wait(&spi_master_instance, CMD41, 6);
			rx_data = get_response();
			
			spi_select_slave(&spi_master_instance, &flash_slave, false);
			spi_write(&spi_master_instance, 0xFF);
	
			count++;
		} while(rx_data == 0x01 && count < 50000);

		if (rx_data == 0x00){
			return VERSION1;
		}
		else{
		
			do {
				spi_select_slave(&spi_master_instance, &flash_slave, true);	
				spi_write_buffer_wait(&spi_master_instance, CMD1, 6);
				rx_data = get_response();
				spi_select_slave(&spi_master_instance, &flash_slave, false);	
				spi_write(&spi_master_instance, 0xFF);

			} while (rx_data == 0x01);

			if (rx_data == 0x00){
				return MMC_VERSION3;
			}
			return UNKNOWN_CARD;
		}

	}

	
 }

 uint8_t get_response(void){
	uint8_t tx_data = 0xFF;
	uint8_t rx_data;

	do{
		spi_write(&spi_master_instance, tx_data);
		spi_read(&spi_master_instance, &rx_data);
	} while ((rx_data & 0x80) != 0x00);
	//Extra time to setup for next command
	spi_write(&spi_master_instance, 0xFF);
	return rx_data;
 };

 void get_r7_response(uint8_t* rx_data, uint32_t* rx_r7_data){
	uint8_t tx_data = 0xFF;
	uint8_t local_rx = 0x00;
	*rx_data = 0x00;
	*rx_r7_data = 0x00;

	do{
		spi_write(&spi_master_instance, tx_data);
		spi_read(&spi_master_instance, rx_data);
	} while ((*rx_data & 0x80) != 0x00);

	spi_transceive_wait(&spi_master_instance, &tx_data, &local_rx);
	*rx_r7_data = *rx_r7_data + (local_rx << 24);

	spi_transceive_wait(&spi_master_instance, &tx_data, &local_rx);
	*rx_r7_data = *rx_r7_data + (local_rx << 16);

	spi_transceive_wait(&spi_master_instance, &tx_data, &local_rx);
	*rx_r7_data = *rx_r7_data + (local_rx << 8);

	spi_transceive_wait(&spi_master_instance, &tx_data, &local_rx);
	*rx_r7_data = *rx_r7_data + local_rx;

	spi_write(&spi_master_instance, tx_data);	
 }

 int write_byte(enum sd_card_type type, uint8_t byte, unsigned int addr){
 	if (type == VERSION1){
 		
 	}
	spi_select_slave(&spi_master_instance, &flash_slave, true);
	spi_write(&spi_master_instance, byte);	
 }

		