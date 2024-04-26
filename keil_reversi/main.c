                  
//#include <LPC210x.H>                       /* LPC210x definitions */
#include "Timer.h"
//#include "Power_management.h"
#include "boton_eint0.h"
#include "temporizador_drv.h"
#include "gpio.h"
#include "planificador.h"

int main (void) {

	GPIO_HAL_PIN_T pin16= 16;
	
	gpio_hal_sentido(pin16,8,GPIO_HAL_PIN_DIR_OUTPUT);
	IOCLR 		= 0x00FF0000;					//Initialices the outputs to 0
	
	temporizador_drv_iniciar();
	temporizador_drv_empezar();

	while (1)  {       		/* Loop forever */
		planificador();

  }
}

