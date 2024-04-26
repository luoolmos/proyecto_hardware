#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <inttypes.h>
#include "io_reserva.h"
#include "temporizador_drv.h"
#include "hello_world.h"
#include "botones_drv.h"
#include "visualizar.h"
#include "juego.h"
#include "Power_management.h"
#include "swi.h"
#include "wd.h"
#include "cola_FIFO.h"

/* El planificador b�sicamente estar� a
	la espera pendiente de la cola de eventos. Cuando aparezca un evento nuevo
	no tratado, lo procesar�. 
*/

#define miliToSec	1000
void planificador(void);
void avisar_hello_world_tick_tack(void);

/*
 El planificador inicializar� la cola indicando que el GPIO para marcar el
 overflow es el GPIO31 definido convenientemente en la cabecera
 io_reserva.h como GPIO_OVERFLOW = 31 y de longitud GPIO_OVERFLOW_BITS = 1.
*/


#endif // PLANIFICADOR_H
