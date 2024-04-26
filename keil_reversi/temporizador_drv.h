#ifndef TEMPORIZADOR_DRV_H
#define TEMPORIZADOR_DRV_H

#include <inttypes.h>
#include "temporizador_hal.h"
#include "eventos.h"

#define us_to_ms 1000;

// funci�n que programa un contador para
// que pueda ser utilizado.
void temporizador_drv_iniciar(void);

// funci�n que inicia la cuenta de un
// contador de forma indefinida.
void temporizador_drv_empezar(void);

// funci�n que lee el tiempo que lleva
// contando el contador desde la �ltima vez que se ejecut�
// temporizador_drv_empezar y lo devuelve en microsegundos.
uint64_t temporizador_drv_leer(void);

// detiene el contador y devuelve el
// tiempo transcurrido desde el �ltimo temporizador_drv_empezar.
uint64_t temporizador_drv_parar(void); 

void temporizador_drv_reloj(uint32_t periodo, void (*funcion_encolar_evento)(), EVENTO_T ID_evento, uint32_t auxData);

uint64_t __swi(0) clock_get_us(void);
#endif // TEMPORIZADOR_DRV_H
