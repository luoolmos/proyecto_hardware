#include "temporizador_drv.h"

static volatile EVENTO_T id_ev_temp;
static volatile uint32_t aux_ev_temp;
void (*funcion_a_ejecutar)(EVENTO_T, uint32_t);  

// funci�n que programa un contador para
// que pueda ser utilizado.
void temporizador_drv_iniciar(void){
	temporizador_hal_iniciar();
}

// funci�n que inicia la cuenta de un
// contador de forma indefinida.
void temporizador_drv_empezar(void){
	temporizador_hal_empezar();
}

// funci�n que lee el tiempo que lleva
// contando el contador desde la �ltima vez que se ejecut�
// temporizador_drv_empezar y lo devuelve en microsegundos.
uint64_t temporizador_drv_leer(void){
	return temporizador_hal_leer() / temporizador_hal_ticks2us;
}


// detiene el contador y devuelve el
// tiempo transcurrido desde el �ltimo temporizador_drv_empezar.
uint64_t temporizador_drv_parar(void){
	return temporizador_hal_parar() / temporizador_hal_ticks2us;
}

void funcion_callback(){
	funcion_a_ejecutar(id_ev_temp,aux_ev_temp);
}

void temporizador_drv_reloj(uint32_t periodo, void (*funcion_encolar_evento)(), EVENTO_T ID_evento, uint32_t auxData) {
		id_ev_temp=ID_evento;
		aux_ev_temp = auxData;
		funcion_a_ejecutar = funcion_encolar_evento;
    temporizador_hal_reloj(periodo, &funcion_callback);
}


uint64_t __swi(0) clock_get_us(void);
uint64_t __SWI_0 (void) {
	return temporizador_drv_leer();
}
