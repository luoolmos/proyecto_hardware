#include "power.h"

/*Cuando el procesador no tenga cálculos que hacer, en lugar de ejecutar un bucle
innecesario, lo vamos a dormir para que reduzca su consumo (modo idle).*/
void power_hal_wait(){
	PCONP = PCONP | 0x6 ;
	PCON = 1;			//ACTIVO MODO IDLE
	EXTWAKE=1;
	//PCONP = PCONP | 0x6 ;	//DEJO ACTIVO TIMER 0 y 1
}
