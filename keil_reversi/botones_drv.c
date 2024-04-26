#include "botones_drv.h"

static volatile int boton1_state, boton2_state;
void (*funcion_callback_fifo_boton)(EVENTO_T, uint32_t); 

static volatile uint32_t retardo, aux_ev_a_boton;
void (*funcion_callback_alarm_boton)(EVENTO_T, uint32_t, uint32_t); 


//***********************************funcion_privada************************************************
// Activa el estado del botón, configura una alarma para consultar el estado periódicamente y
// se encola en evento botón con el boton correspondiente.
void pulsacion(uint32_t boton){
	activar_estado(boton);
	retardo = 0x80000064; //bit31 = 1 and 100ms de retardo
	aux_ev_a_boton = boton;
	funcion_callback_alarm_boton(CONSULTAR_BOTON,retardo,aux_ev_a_boton); //100ms y periodico
	funcion_callback_fifo_boton(BOTON,boton);
};
//*******************************************************************************************************

/**
 * Inicializa el controlador de botones.
 * funcion_parametro_fifo Puntero a la función de callback FIFO_encolar.
 * funcion_parametro_alarma Puntero a la función de callback de alarma_activar.
 * 
 * Esta función configura y habilita las interrupciones para EINT1 y EINT2, e inicializa las variables y punteros necesarios.
 */
void init_boton(void (*funcion_parametro_fifo)(),void(*funcion_parametro_alarma)()){
	boton1_state=NO_PULSADO;
	boton2_state=NO_PULSADO;
	eint1_init(&pulsacion);
	eint2_init(&pulsacion);

	funcion_callback_fifo_boton= funcion_parametro_fifo;
	funcion_callback_alarm_boton = funcion_parametro_alarma;	
}

/**
 * pre:  Obtiene el estado actual de un botón.
 * 		 boton Número de botón.
 * post: Estado actual del botón (PULSADO o NO_PULSADO).
 */
int mostrar_estado(int boton){
	if(boton==BOTON_1){
		return boton1_state;
	}else if(boton==BOTON_2){
		return boton2_state;
	}else{
		return -1;
	}
}

/**
 * pre: Activa el estado de un botón.
 * 		boton Número de botón.
 * 
 * Esta función se utiliza para activar el estado de un botón específico (PULSADO).
 */
void activar_estado(int boton){
		if(boton==BOTON_1){
				boton1_state=PULSADO;
	}else if(boton==BOTON_2){
			boton2_state=PULSADO;
	}
}

/**
 * pre: Desactiva el estado de un botón.
 * 		boton Número de botón.
 * 
 * post: Esta función se utiliza para desactivar el estado de un botón específico (NO_PULSADO).
 */
void desactivar_estado(int boton){
		if(boton==BOTON_1){
				boton1_state=NO_PULSADO;
			
	}else if(boton==BOTON_1){
			boton2_state=NO_PULSADO;
	}
}

/**
 * Gestiona el estado de los botones.
 * 
 * Verifica si los botones han sido liberados y, en ese caso, desactiva sus estados y habilita las interrupciones correspondientes.
 * Si ambos botones están en estado NO_PULSADO, desactiva la alarma que consulta el estado periódicamente.
 */
void tratar_boton(){
		if(eint1_pressed() == NO_PULSADO && boton1_state== PULSADO ){
			desactivar_estado(1);
			eint1_habilitar_interrupciones();
		}
		if(eint2_pressed()== NO_PULSADO && boton2_state== PULSADO ){
			desactivar_estado(2);
			eint2_habilitar_interrupciones();
		}
		if(boton1_state == NO_PULSADO && boton2_state == NO_PULSADO){
			retardo = 0;
			aux_ev_a_boton = 0;
			funcion_callback_alarm_boton(CONSULTAR_BOTON,retardo,aux_ev_a_boton);
		}
}

