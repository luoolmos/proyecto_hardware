#include "wd.h"

// Configura el temporizador del wd con un tiempo de espera especificado en segundos.
// El temporizador es habilitado y configurado para realizar un reset del sistema al expirar.
void WD_hal_inicializar(int sec){
	// first, it checks for a previous watchdog time out  
	//WDTOF (bit 2)The Watchdog Time-Out Flag is set when the watchdog times out. This flag is cleared by software.
	if( WDMOD & 0x04 ) {					   /* Check for watchdog time out. El bit 2 se activa si se ha disparado el watchdog*/
			WDMOD &= ~0x04;						   /* Clear time out flag           */
	 }
	// Time out: Pclk*WDTC*4
	// valor m?nimo WTC= 256; valor m?ximo 2^32 (si escribes algo menos que FF, se escribe FF)
	WDTC  = sec * 15000000/4;						   // Set watchdog time out value
	WDMOD = 0x03;                            /* Enable watchdog timer (bit 0) and reset (bit 1).  */   
	// se puede trabajar en dos modos: reset dando el valor 11 a los dos bits menos significativos; Int: dando 10 (en ese caso el WT genera una interrupci?n
	// Una vez se da un valor a estos bits ?ya no se pueden cambiar a no ser que haya un reset externo, o la cuenta del WT acabe
	// Watchdog Timer Value register (WDTV) The WDTV register is used to read the current value of watchdog timer.
}


// Alimenta (reset) el temporizador del wd escribiendo valores específicos en sus registros.
void WD_hal_feed(){
	//*********************************************
	uint32_t bit_IRQ = read_IRQ_bit();
	if(bit_IRQ == 1){
			disable_irq();}
	//********************s**************************
	WDFEED = 0xAA;						   
  WDFEED = 0x55;  
	//*********************************************
		if(bit_IRQ == 1){
			enable_irq();}
	//*********************************************
	
}

// Utilizada para verificar la funcionalidad del temporizador del wd.
void WD_hal_test(){
	while (1);
	
}
