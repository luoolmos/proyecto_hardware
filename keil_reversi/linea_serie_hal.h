#ifndef LINEA_SERIE_HAL_H
#define LINEA_SERIE_HAL_H

#include <LPC210X.H>
#include <inttypes.h>
#include <string.h>

#define inProgress 0
#define fin_buffer 1
#define notInicializado 2

// devuelve el estado del buffer (notInicializado,inProgress o fin_buffer)
uint8_t check_state(void);

/* 
	pre: array es un vector de caracteres

	post: ha copiado array a la variable local buffer_state y define que el buffer
	esta en progreso de enviarse
 */
void linea_serie_hal_enviar_array(char array[]);

/* 
	post: mira si el caracter a enviar no es el final y llama a
	la funcion para enviar el caracter  
 */
void linea_serie_hal_continuar_envio(void);
// Rutina de servicio de interrupci�n para UART0

/*  
	post: si el caracter a enviar no es el final y llama a
	la funcion del hal para enviar el caracter  
 */
void init_UART0(void (*funcion_recive)(), void (*funcion_send)());

/* 
	post: muestra un caracter por la UART0
 */
void linea_serie_hal_enviar_caracter(char caracter);
#endif // LINEA_SERIE_HAL_H
