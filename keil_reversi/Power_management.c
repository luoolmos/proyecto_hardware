
#include "power_management.h"
// Set the processor into power down state 
// The watchdog cannot wake up the processor from power down
/* Configura el bit EXTWAKE para permitir que la interrupción EXTINT0 despierte el procesador y
 * establece el bit PCON[1] para poner el procesador en el estado de apagado (power down).
 * El watchdog no puede despertar al procesador desde el estado de apagado.
 */
void PM_power_down (void)  {
  EXTWAKE = 1; // EXTINT0 will awake the processor
	PCON |= 0x02; 
}

/* Configura los bits PCONP[3:1] para habilitar el reloj de los periféricos UART1, UART0 y Timer 0.
 * Luego, establece el bit PCON[0] para poner el procesador en el estado de bajo consumo.
 */
void power_hal_wait(void){
	PCONP |= 0X0E;
	PCON = 0X01;
	
}

/* Configura el bit EXTWAKE para permitir que la interrupción EXTINT1 despierte el procesador y
 * establece el bit PCON[1] para poner el procesador en el estado deep sleep.
 * Cambia al modo PLL antes de entrar en el estado de sueño profundo.*/
void power_hal_deep_sleep(void){
	EXTWAKE = 6; //EXTINT1 will awake the processor 
	PCON |= 0x02; 
	Switch_to_PLL();
}
	
