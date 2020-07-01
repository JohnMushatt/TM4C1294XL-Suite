#include "internal_temperatuer_sensor/temperature_tools.h"
#include "board/Board.h"
#include  "driverlib/flash.h"
uint32_t gSystemClock = 120000000; // [Hz] system clock frequency

/*
 *  ======== main ========
 */
int main(void) {
	IntMasterDisable();

	/* Call board init functions */
	Board_initGeneral();
	Board_initGPIO();
	Board_initEMAC();
	temp_init();
	display_init();
	//enet_init();

	BIOS_start();

	return (0);
}

