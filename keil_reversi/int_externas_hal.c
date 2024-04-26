#include "int_externas_hal.h"

//************************************************************************************variablesGlobalesEINT1
// variable para comprobar que se hacen las interrupciones que deber�an hacerse
static volatile unsigned int eint1_count = 0;
// variable que se activa al detectar una nueva pulsaci�n
static volatile int eint1_nueva_pulsacion = 0;

//************************************************************************************variablesGlobalesEINT2
static volatile unsigned int eint2_count = 0;
// variable que se activa al detectar una nueva pulsaci�n
static volatile int eint2_nueva_pulsacion = 0;

void (*funcion_callback_boton)(uint32_t);  


//***************EINT1***************************//

// Incrementa el contador de pulsaciones eint1_count, activa la variable eint1_nueva_pulsacion,
// y llama a la función de callback asociada al botón 1.
// Esta función se encarga de tratar el botón
void eint1_ISR (void) __irq {
	eint1_count++;
	VICIntEnClr |= 0x00008000; // Deshabilitar la interrupci�n EINT1
	EXTINT =  EXTINT | 2;        // clear interrupt flag        //bin(10)=2
	VICVectAddr = 0;             // Acknowledge Interrupt
	eint1_nueva_pulsacion = 1;
	funcion_callback_boton(BOTON_1);
}

//Limpia la variable que indica una nueva pulsación para EINT1.
void eint1_clear_nueva_pulsacion(void){
	eint1_nueva_pulsacion = 0;
};

// Obtiene el valor de la variable que indica una nueva pulsación para EINT1.
// devuelve 1 si hay una nueva pulsación, 0 en caso contrario.
unsigned int eint1_read_nueva_pulsacion(void){
	return eint1_nueva_pulsacion;
};

//Devuelve el valor del contador de pulsaciones
unsigned int eint1_read_count(void){
	return eint1_count;
};

//Habilita las interrupikciones para EINT1.
void eint1_habilitar_interrupciones(void){
	VICIntEnable = VICIntEnable | 0x00008000;
};

// Devuelve 1 si el botón está presionado, 0 en caso contrario.
unsigned int eint1_pressed(void){
	EXTINT = EXTINT | 2;
	return ((EXTINT & 2)==2);
};

// pre: funcion_pulsacion se corresponde con la funcion de encolar
// post: La interrupción EINT1 está configurada y habilitada para detectar flancos de subida en el pin correspondiente.
//      Las variables globales eint1_nueva_pulsacion y eint1_cuenta se han inicializado a 1.
void 	eint1_init (void (*funcion_drv)()) {
// NOTA: seg�n el manual se puede configurar c�mo se activan las interrupciones: por flanco o nivel, alta o baja. 
// Se usar�an los registros EXTMODE y EXTPOLAR. 
// Sin embargo parece que el simulador los ignora y no aparecen en la ventana de ocnfiguraci�n de EXT Interrupts
// configure EXTINT0 active if a rising-edge is detected
//	EXTMODE 	=	1; //1 edge, 0 level
//	EXTPOLAR	=	1; // 1 high, rising-edge; 0 low, falling-edge
//  prueba = EXTMODE;
	eint1_nueva_pulsacion = 0;
	eint1_count = 0;
	EXTINT =  EXTINT | 2;        // clear interrupt flag     	
	// configuration of the IRQ slot number 2 of the VIC for EXTINT0
	VICVectAddr2 = (unsigned long)eint1_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 

	
		// 15 is the number of the interrupt assigned. Number 15 is the EINT1 (see table 40 of the LPC2105 user manual

	PINSEL0 = PINSEL0 & 0xDfffffff; //Sets bits 29:28 to 0
	PINSEL0	= PINSEL0 | 0x20000000; //Set 29:28 bits to 10
	
	
	VICVectCntl2 = 0x20 | 15;                   
 	VICIntEnable = VICIntEnable | 0x00008000;                  // Enable EXTINT1 Interrupt
	funcion_callback_boton = funcion_drv;
	
}

	/*********************************
	activa los bits [31:28] de forma 1010
	ya que para habilitar la interrupci�n
	de EINT 1 [29:28] -> 10
	de EINT 2 [31:30] -> 10
	**********************************/

//*******************EINT2***************************//
// Incrementa el contador de pulsaciones eint2_count, activa la variable eint2_nueva_pulsacion,
// y llama a la función de callback asociada al botón 2.
// Esta función se encarga de tratar el botón
void eint2_ISR (void) __irq {
	eint2_count++;
	VICIntEnClr |= 0x00010000;
	EXTINT =  EXTINT | 4;        // clear interrupt flag 
	VICVectAddr = 0;             // Acknowledge Interrupt
	eint2_nueva_pulsacion = 1;
	funcion_callback_boton(BOTON_2);
}

//Limpia la variable que indica una nueva pulsación para EINT2.
void eint2_clear_nueva_pulsacion(void){
	eint2_nueva_pulsacion = 0;
};

// Obtiene el valor de la variable que indica una nueva pulsación para EINT1.
// devuelve 1 si hay una nueva pulsación, 0 en caso contrario.
unsigned int eint2_read_nueva_pulsacion(void){
	return eint2_nueva_pulsacion;
};

//Devuelve el valor del contador de pulsaciones de EINT2.
unsigned int eint2_read_count(void){
	return eint2_count;
};

//Habilita las interrupciones para EINT2.
void eint2_habilitar_interrupciones(void){
//VICIntEnable = VICIntEnable | 0x00008000;
	VICIntEnable = VICIntEnable | 0x00010000;
};

// Devuelve 1 si el botón está presionado, 0 en caso contrario.
unsigned int eint2_pressed(void){
	EXTINT = EXTINT | 4;
	return ((EXTINT & 4)==4 );
};

// pre: funcion_pulsacion se corresponde con la funcion de encolar
// post: La interrupción EINT2 está configurada y habilitada para detectar flancos de subida en el pin correspondiente.
//      Las variables globales eint2_nueva_pulsacion y eint2_cuenta se han inicializado a 0.
void eint2_init (void (*funcion_drv)()) {
// NOTA: seg�n el manual se puede configurar c�mo se activan las interrupciones: por flanco o nivel, alta o baja. 
// Se usar�an los registros EXTMODE y EXTPOLAR. 
// Sin embargo parece que el simulador los ignora y no aparecen en la ventana de ocnfiguraci�n de EXT Interrupts
// configure EXTINT0 active if a rising-edge is detected
//	EXTMODE 	=	1; //1 edge, 0 level
//	EXTPOLAR	=	1; // 1 high, rising-edge; 0 low, falling-edge
//  prueba = EXTMODE;
	eint2_nueva_pulsacion = 0;
	eint2_count = 0;
	EXTINT =  EXTINT | 4;        // clear interrupt flag     	
	// configuration of the IRQ slot number 2 of the VIC for EXTINT0
	VICVectAddr3 = (unsigned long)eint2_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 

		// 16 is the number of the interrupt assigned. Number 16 is the EINT2 (see table 40 of the LPC2105 user manual

	PINSEL0 = PINSEL0 & 0x3fffffff; //Sets bits 31:30 to 0
	PINSEL0	= PINSEL0 | 0x80000000; //Set 31:30 bits to 10
	
	
	VICVectCntl3 = 0x20 | 16;                   
  VICIntEnable = VICIntEnable | 0x00010000;             // Enable EXTINT2 Interrupt
	funcion_callback_boton = funcion_drv;
	
}



