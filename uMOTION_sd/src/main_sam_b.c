/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

/*
  Since we do not properly generate system init code, we define DONT_USE_CMSIS_INIT
  before including the device headers. We also comment out the call to SystemInit().
*/


#define DONT_USE_CMSIS_INIT
#include <asf.h>
#include "spi_control.h"
#include "samb11-drv.h"

struct spi_module spi_master_instance;
volatile bool transrev_complete_spi_master = false;

int main(void)
{
    /* Initialize the SAM system */
    system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_3_25_MHZ);

	configure_spi_master();	
	configure_spi_master_callbacks();
	
	enum sd_card_type card_type;

	card_type = setup_samb11_drv(0);

	if (card_type == VERSION2_BLOCK || card_type == VERSION2_BYTE){
		while(1){
			asm(" NOP");
		}
	}

	while(1){
	}
}
