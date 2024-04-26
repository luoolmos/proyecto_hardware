#ifndef LINEA_SERIE_DRV_H
#define LINEA_SERIE_DRV_H

#include <inttypes.h>
#include "linea_serie_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llamadas_sistema_hal.h"
#include "eventos.h"
#include "gpio.h"


typedef enum {
	OFF = 0,
	ON = 1
}STATE;


void init_teclado(void (*funcion_parametro_fifo)(),void (*funcion_parametro_alarma)(), 
	uint32_t timeReinicio,GPIO_HAL_PIN_T pin_serie_error, uint8_t n_serie_bits);
void linea_serie_drv_enviar_array(char array[500], uint32_t aux);
void linea_serie_drv_continuar_envio(void);
void rellenoBuffer(uint32_t caracter_recibido);
void transmision_finalizada(void);

#endif // LINEA_SERIE_DRV_H
