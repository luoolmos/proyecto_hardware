#ifndef JUEGO_H
#define JUEGO_H

#include <inttypes.h>
#include "temporizador_drv.h"
#include "tablero.h"
#include "conecta_K_2023.h"
#include "linea_serie_drv.h"
#include "entrada.h"
#include "alarmas.h"


//pre: funcion_parametro se corresponde con la funcion que encola eventos
//		error_command indica el bit del gpio en el que se deben marcar los errores del juego
//		n_error_command indica el numero de bits a modificar en el gpio
//post: inicializa el módulo juego dejándolo listo para empezar a tratar comandos, 
//		envía instrucciones de juego
void juego_inicializar(void (*funcion_parametro)(),uint32_t (*funcion_estadisticas)(),GPIO_HAL_PIN_T error_command , uint8_t n_error_command);
/* 	pre: ID_evento: tipo de evento a tratar
		   auxData: comando a tratar
 *  post: Se maneja el evento de recepción de datos por la serie.
 *   - Si el comando recibido es VOID, no realiza acciones adicionales.
 *   - Si el comando recibido es NEW, inicia el juego si el estado es OFF.
 *   - Para otros comandos:
 *     - Si el estado del juego es ON:
 *       - Para CONF_JUGADA: Confirma una jugada exitosa.
 *       - Para CANC_JUGADA: Cancela la jugada y cancela la alarma.
 *       - Para END: Trata el fin del juego.
 *       - Para otros comandos (jugadas):
 *         - Calcula el tiempo de respuesta entre jugadas.
 *         - Si la fila y columna son válidas y la celda está vacía, carga datos provisionales y activa una alarma.
 *         - Si la celda no está vacía, envía un mensaje de información sobre el fallo.
 *     - Si el estado del juego es OFF, no realiza acciones.
 * - Se maneja el evento de transmisión por la serie.
 *   - Si el tipo de transmisión es JUGADA, se registra el inicio del tiempo de jugada.
 *   - Si el tipo de transmisión es STATS, se envían las estadísticas al finalizar el juego.
 *   - Si el tipo de transmisión es INFO_CANCELAR, se envía un mensaje para mostrar "pulsa 1 para cancelar jugada". */
void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData);


#endif // JUEGO_H
