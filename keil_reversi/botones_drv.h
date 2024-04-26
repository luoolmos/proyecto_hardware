#ifndef BOTONES_DRV_H
#define BOTONES_DRV_H

#include <inttypes.h>
#include "int_externas_hal.h"
#include "eventos.h"

enum {
	PULSADO = 1,
	NO_PULSADO = 0
};


/**
 * Inicializa el controlador de botones.
 * funcion_parametro_fifo Puntero a la función de callback FIFO_encolar.
 * funcion_parametro_alarma Puntero a la función de callback de alarma_activar.
 * 
 * Esta función configura y habilita las interrupciones para EINT1 y EINT2, e inicializa las variables y punteros necesarios.
 */
void init_boton(void (*funcion_parametro_fifo)(),void(*funcion_parametro_alarma)());
/**
 * pre:  Obtiene el estado actual de un botón.
 * 		 boton Número de botón.
 * post: Estado actual del botón (PULSADO o NO_PULSADO).
 */
int mostrar_estado(int boton);
/**
 * pre: Activa el estado de un botón.
 * 		boton Número de botón.
 * 
 * Esta función se utiliza para activar el estado de un botón específico (PULSADO).
 */
void activar_estado(int boton);	
/**
 * pre: Desactiva el estado de un botón.
 * 		boton Número de botón.
 * 
 * post: Esta función se utiliza para desactivar el estado de un botón específico (NO_PULSADO).
 */
void desactivar_estado(int boton);	
/**
 * Gestiona el estado de los botones.
 * 
 * Verifica si los botones han sido liberados y, en ese caso, desactiva sus estados y habilita las interrupciones correspondientes.
 * Si ambos botones están en estado NO_PULSADO, desactiva la alarma que consulta el estado periódicamente.
 */
void tratar_boton(void);

#endif // BOTONES_DRV_H



