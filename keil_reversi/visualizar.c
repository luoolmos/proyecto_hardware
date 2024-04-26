#include "visualizar.h"

// Configura el sentido de los pines asociados a la visualización como salidas.
void visualizar_init(){
	gpio_hal_sentido(GPIO_CUENTA,GPIO_CUENTA_BITS,GPIO_HAL_PIN_DIR_OUTPUT);
}

//Escribe el valor de la cuenta en los pines asociados a la visualización.
void visualizar_cuenta(uint32_t cuenta){
	gpio_hal_escribir(GPIO_CUENTA,GPIO_CUENTA_BITS,cuenta);
}

