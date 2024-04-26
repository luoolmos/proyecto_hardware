#ifndef TEMPORIZADOR_HAL_H
#define TEMPORIZADOR_HAL_H

#include <inttypes.h>
//#include "botones_drv.h"

// la constante temporizador_hal_ticks2us: permite convertir de ticks a
// microsegundos.
extern const unsigned int temporizador_hal_ticks2us;

// función que programa un contador para
// que pueda ser utilizado.
void temporizador_hal_iniciar(void);

// función que inicia la cuenta de un
// contador de forma indefinida.
void temporizador_hal_empezar(void);

// función que lee el tiempo que lleva
// contando el contador desde la última vez que se ejecutó
// temporizador_hal_empezar y lo devuelve en ticks.
uint64_t temporizador_hal_leer(void);

// detiene el contador y devuelve el
// tiempo en ticks transcurrido desde el último temporizador_hal_empezar. 
uint64_t temporizador_hal_parar(void);

void temporizador_hal_reloj (uint32_t periodo, void (*funcion_callback)());

#endif // TEMPORIZADOR_HAL_H



