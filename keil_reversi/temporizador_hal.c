#include <LPC210X.H>  
#include "temporizador_hal.h"

void timer0_ISR(void) __irq;
void timer1_ISR(void) __irq;
const unsigned int temporizador_hal_ticks2us = 15;
static volatile unsigned int timer0_int_count = 0;
static volatile unsigned int timer1_int_count = 0;

//#define frec 15000
#define frec 0XFFFFFFFF
void (*funcion_callbackHAL)();   


// funci�n que programa un contador para
// que pueda ser utilizado.
void  temporizador_hal_iniciar(void){
		timer0_int_count = 0;	
	// configuration of Timer 0
		T0PR=0;
		T0MR0 = frec-1;                        // Interrumpe cada 0,05ms = 150.000-1 counts
    T0MCR = 3;                     // Generates an interrupt and resets the count when the value of MR0 is reached
    //VICVectCntl0 = 0x20 | 4; 
		VICVectAddr0 = (unsigned long)timer0_ISR;   // set interrupt vector in 0
		VICVectCntl0 = 0x20 | 4;
	//T0TCR = 1;                             // Timer0 Enable
}

// funci�n que inicia la cuenta de un
// contador de forma indefinida..
void temporizador_hal_empezar(void){
		//T0MCR = 3; 
		T0TCR = 1;                             // Timer0 Enable
		//VICVectAddr0 = (unsigned long)timer0_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual  
		                   
    VICIntEnable = VICIntEnable | 0x00000010;                  // Enable Timer0 Interrupt
}

/* Timer Counter 0 Interrupt executes each 1ms @ 60 MHz CPU Clock */
void timer0_ISR(void) __irq {
    timer0_int_count++;
    T0IR = 1;                              // Clear interrupt flag
    VICVectAddr = 0;                        // Acknowledge Interrupt
}


// funci�n que lee el tiempo que lleva
// contando el contador desde la �ltima vez que se ejecut�
// temporizador_hal_empezar y lo devuelve en ticks.
uint64_t temporizador_hal_leer(void){
	uint64_t timer0_tick = timer0_int_count*frec;
	return T0TC + timer0_tick; 
 }

// detiene el contador y devuelve el
// tiempo en ticks transcurrido desde el �ltimo temporizador_hal_empezar. 
uint64_t temporizador_hal_parar(void){
	T0TCR = 3; // resetea la cuenta
	T0TCR = 0; // quita el reset y detiene el contador
	return temporizador_hal_leer();
};


void temporizador_hal_reloj (uint32_t periodo, void (*funcion_callback)()){
    if (periodo!=0){
				timer1_int_count = 0;	
				// configuration of Timer 0	
				T1PR=0;
				T1MR0 = (((periodo*1000)*temporizador_hal_ticks2us) -1);
				T1MCR = 3;  
				funcion_callbackHAL =  funcion_callback;
				VICVectAddr1 = (unsigned long)timer1_ISR; 
				T1TCR = 1;                             // Timer1 Enable
				// 0x20 bit 5 enables vectored IRQs. 
				// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual  
				VICVectCntl1 = 0x20 | 5;                   
				VICIntEnable = VICIntEnable | 0x00000020;                  // Enable Timer0 Interrupt
    }else{
        T1TCR = 3; // resetea la cuenta
				T1TCR = 0; // quita el reset y detiene el contador
    }
}

void timer1_ISR()__irq{
	T1IR = 1;                              // Clear interrupt flag
  VICVectAddr = 0;                            // Acknowledge Interrupt
	funcion_callbackHAL();
}
