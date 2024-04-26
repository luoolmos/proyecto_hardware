#ifndef HELLO_WORLD_H
#define HELLO_WORLD_H

#include <inttypes.h>

#include	"io_reserva.h"
#include  "alarmas.h"

/*
 pre: funcion_parametro tiene la funcion para encolar eventos, funcion_parametro_alarma tiene la 
	funcion para añadir alarmas 
 post: define que los pines GPIO_HELLO_WORLD son de salida, y añade las dos funciones que se le
 	han pasado por parametro a dos variables locales para poder usarlas en las funciones del modulo, 
	programo una alarma que encola el ev_LATIDO cada 10 ms periodicamente

*/
void hello_world_inicializar(void (*funcion_parametro)(),void (*funcion_parametro_alarma)());

	
/* 
 pre:
 post: suma uno a tick_count que cuenta decenas de ms, si llega a 10 (100 ms = 1 decima de seg)
	suma uno a tick_count_decimas que cuenta decimas de segundo y lo muestra por el gpio
 */
void  hello_world_tick_tack(void);
/* 
 pre:
 post: encola un evento ev_VISUALIZAR_HELLO
 */
void hello_world_tratar_evento(EVENTO_T ID_evento);
#endif // HELLO_WORLD_H
