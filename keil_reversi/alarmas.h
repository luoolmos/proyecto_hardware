#ifndef ALARMAS_H
#define ALARMAS_H

#include <inttypes.h>
#include "temporizador_drv.h"
#include "hello_world.h"
#include "llamadas_sistema_hal.h"
#define ALARMAS_MAX 4

//pre: funcion_parametro se corresponde con la funcion que encola eventos
//		error indica el bit del gpio en el que se deben marcar los errores del módulo alarma
//		n_error indica el numero de bits a modificar en el gpio
//post: inicializa el módulo alarmas dejándolo listo para empezar a gestionarlas
void alarma_inicializar(void (*funcion_parametro)(), GPIO_HAL_PIN_T bit_inicial, uint8_t num_bits);

/*Cuando un m�dulo quiere programar una nueva alarma
El ID_evento determina el evento a insertar en la cola al vencer la
alarma (del conjunto de eventos posibles en la cola). El retardo codifica en el
bit de m�s peso si la alarma es peri�dica (debe repetirse), los restantes bits
indican el retardo en milisegundos. El campo auxData es el campo auxiliar que
se encolara en la cola de eventos adem�s del ID_evento. Si el retardo es cero
se cancelar� la alarma. Si se reprograma un determinado evento antes de
que salte, se reprogramar� al nuevo valor (identificador �nico).
*/
void alarma_activar(EVENTO_T ID_evento, uint32_t retardo, uint32_t
auxData);

/* Esta funci�n comprobar� si hay que disparar el
evento asociado a alguna de las alarmas programadas pendientes. Las
alarmas no peri�dicas se cancelan tras dispararse. Las peri�dicas contin�an
activas de forma indefinida hasta que se eliminen. */
void  alarma_tratar_evento(void);

#endif // ALARMAS_H
