#ifndef GPIO_H
#define GPIO_H

#include <LPC210X.H>
#include <inttypes.h>

/*
el conjunto de direcciones E/S del GPIO gpio_hal_pin_dir_t:
GPIO_HAL_PIN_DIR_INPUT, GPIO_HAL_PIN_DIR_OUTPUT.
el tipo de datos de la representación del pin del GPIO: GPIO_HAL_PIN_T 
*/
typedef enum{
	GPIO_HAL_PIN_DIR_INPUT,
	GPIO_HAL_PIN_DIR_OUTPUT
}gpio_hal_pin_dir_t; 

typedef uint32_t GPIO_HAL_PIN_T; 	//tipo de datos ; 2^8 pins se pueden identificar

// el conjunto de direcciones E/S del GPIO gpio_hal_pin_dir_t:
// GPIO_HAL_PIN_DIR_INPUT, GPIO_HAL_PIN_DIR_OUTPUT
//enum gpio_hal_pin_dir_t GPIO_HAL_PIN_DIR_INPUT ;
//enum gpio_hal_pin_dir_t GPIO_HAL_PIN_DIR_OUTPUT;

//************************************************************

//Permite emplear el GPIO y debe ser invocada antes
//de poder llamar al resto de funciones de la biblioteca
__inline void 
	gpio_hal_iniciar(void){
	PINSEL0 = 0;
	PINSEL1 = 0;
}

// los bits indicados se utilizarán como
//entrada o salida según la dirección
__inline void 
gpio_hal_sentido(GPIO_HAL_PIN_T gpio_inicial, uint8_t num_bits,
gpio_hal_pin_dir_t direccion){
	
	if (direccion == GPIO_HAL_PIN_DIR_INPUT) {
			// 0AF00030
        uint32_t aux = ~((~(0xFFFFFFFF << num_bits)) << gpio_inicial);	
				IODIR = IODIR & aux;
	} else if (direccion == GPIO_HAL_PIN_DIR_OUTPUT){
        // Configura el pin GPIO como salida
        uint32_t aux = ((~(0xFFFFFFFF << num_bits)) << gpio_inicial);	
				IODIR = IODIR | aux;
    }
}

// gpio_inicial indica el primer bit a leer, num_bits indica cuántos
// bits queremos leer
__inline uint32_t 
gpio_hal_leer(GPIO_HAL_PIN_T gpio_inicial, uint8_t
num_bits){
	
	// IOPIN: he current state of the GPIO configured port 
	// pins can always be read from this register,
	// regardless of pin direction.
	/****************************************** 
	lee los más significativos
	return ((IOPIN << gpio_inicial) & ~(0xFFFFFFFF >> num_bits) >> (32-num_bits));s
	*******************************************/
	
	/****************************************** 
	lee los menos significativos
	*******************************************/
	return (IOPIN >> gpio_inicial) & ~(0xFFFFFFFF << num_bits);
	//return ((IOPIN >> (gpio_inicial+1 - num_bits)) & ~(0xFFFFFFFF << num_bits));
	
	// IOPIN tiene el estado, se desplaza a la derecha gpio_incial bits
	// se desplaza hacia la izquierda 0xFFFFFFFF num bits, para 
	// que se queden 0 el bits que quiero leer, al hacer complemento
	// me quedo con 1 en los bits que quiero leer 
}

// gpio_inicial indica el primer bit a escribir, num_bits indica cuántos
// bits queremos escribir
__inline void 
gpio_hal_escribir(GPIO_HAL_PIN_T bit_inicial, uint8_t num_bits,
uint32_t valor){
	
	// falta comprobar si los bits en los que escribes 
	// son de salida??????
	uint32_t aux = (valor & ~(0xFFFFFFFF << num_bits)) ; 		// me quedo con los bits que voy a escribir, de menos peso para IOSET
	uint32_t aux2 = (~valor & (~(0xFFFFFFFF << num_bits)));	//	su negado para IOCLR		
	IOCLR = (aux2 << (bit_inicial)) ;	
	IOSET = (aux << (bit_inicial)) ;
}

#endif // GPIO_H
