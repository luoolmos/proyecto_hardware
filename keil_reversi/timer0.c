
#include <LPC210X.H>                            // LPC21XX Peripheral Registers
#include "timer0.h"
#include "pulsacion.h"
// variable para contabilizar el número de interrupciones
static volatile unsigned int timer0_int_count = 0;

//void timer0_ISR (void) __irq;    // Generate Interrupt 

//int valor = 0;

/* Setup the Timer Counter 0 Interrupt */

void timer0_init (int interrump, int resetValue) {
		timer0_int_count = 0;	
	// configuration of Timer 0
		//valor = interrump;
		T0MR0 = interrump;                        // Interrumpe cada 0,05ms = 150.000-1 counts
    T0MCR = resetValue;                     // Generates an interrupt and resets the count when the value of MR0 is reached
    //T0TCR = 1;                             // Timer0 Enable
    // configuration of the IRQ slot number 0 of the VIC for Timer 0 Interrupt
		//VICVectAddr0 = (unsigned long)timer0_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual  
		//VICVectCntl0 = 0x20 | 4;                   
    //VICIntEnable = VICIntEnable | 0x00000010;                  // Enable Timer0 Interrupt
}

void timer0_starts (void) {
		T0TCR = 1;                             // Timer0 Enable
		//VICVectAddr0 = (unsigned long)timer0_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual  
		VICVectCntl0 = 0x20 | 4;                   
    VICIntEnable = VICIntEnable | 0x00000010;                  // Enable Timer0 Interrupt
}


void timer0_end (void) {
		T0TCR = 3; // resetea la cuenta
		T0TCR = 0; // quita el reset y detiene el contador
}

/* Timer Counter 0 Interrupt executes each 10ms @ 60 MHz CPU Clock */
/*
void timer0_ISR (void) __irq {
    timer0_int_count++;
    T0IR = 1;                              // Clear interrupt flag
    VICVectAddr = 0;                            // Acknowledge Interrupt
}
*/
unsigned int timer0_read_int_count(void){
	return 	timer0_int_count;
};


