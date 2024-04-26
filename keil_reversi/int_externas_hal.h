#ifndef INT_EXTERNAS_HAL_H
#define INT_EXTERNAS_HAL_H

#include <inttypes.h>
#include <LPC210X.H>                            // LPC21XX Peripheral Registers


enum {
		BOTON_1 = 1,
    BOTON_2 = 2
};

// pre: funcion_pulsacion se corresponde con la funcion de encolar
// post: La interrupción EINT1 está configurada y habilitada para detectar flancos de subida en el pin correspondiente.
//      Las variables globales eint1_nueva_pulsacion y eint1_cuenta se han inicializado a 1.
void eint1_init (void (*funcion_pulsacion)());

//Limpia la variable que indica una nueva pulsación para EINT1.
void eint1_clear_nueva_pulsacion(void);

// Obtiene el valor de la variable que indica una nueva pulsación para EINT1.
// devuelve 1 si hay una nueva pulsación, 0 en caso contrario.
unsigned int eint1_read_nueva_pulsacion(void);

//Devuelve el valor del contador de pulsaciones
unsigned int eint1_read_count(void);

//Habilita las interrupikciones para EINT1.
void eint1_habilitar_interrupciones(void);

// Devuelve 1 si el botón está presionado, 0 en caso contrario.
unsigned int eint1_pressed(void);

// pre: funcion_pulsacion se corresponde con la funcion de encolar
// post: La interrupción EINT2 está configurada y habilitada para detectar flancos de subida en el pin correspondiente.
//      Las variables globales eint2_nueva_pulsacion y eint2_cuenta se han inicializado a 0.
void eint2_init (void (*funcion_pulsacion)());

//Devuelve el valor del contador de pulsaciones de EINT2.
unsigned int eint2_read_count(void);

//Limpia la variable que indica una nueva pulsación para EINT2.
void eint2_clear_nueva_pulsacion(void);

//Habilita las interrupciones para EINT2.
void eint2_habilitar_interrupciones(void);

// Devuelve 1 si el botón está presionado, 0 en caso contrario.
unsigned int eint2_pressed(void);

//Devuelve el valor del contador de pulsaciones de EINT2.
unsigned int eint2_read_count(void);

#endif // INT_EXTERNAS_HAL_H



