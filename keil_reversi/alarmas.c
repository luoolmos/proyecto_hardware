#include "alarmas.h"

static volatile int cont_alarmas = 0;
void (*funcion_callback_fifo_alarmas)(EVENTO_T, uint32_t); 

static volatile GPIO_HAL_PIN_T bit_error;
static volatile uint8_t n_bit_error;


typedef struct ALARM{
	EVENTO_T id; // id del evento
	uint32_t	auxData;	// a�n no se bien
	int inicio;
	uint32_t time;
	uint16_t periodico;
} ALARM; 

ALARM ALARMAS[ALARMAS_MAX];

//*************************funcion_privada*********************************
//valor-> valor a escribir en el gpio 
//escribe valor en los bits configurados para el error del juego
void gpio_aux_escribir_error(uint32_t valor){
	gpio_hal_escribir(bit_error,n_bit_error,valor);
}


//pre: funcion_parametro se corresponde con la funcion que encola eventos
//		error indica el bit del gpio en el que se deben marcar los errores del módulo alarma
//		n_error indica el numero de bits a modificar en el gpio
//post: inicializa el módulo alarmas dejándolo listo para empezar a gestionarlas
void alarma_inicializar(void (*funcion_parametro)(), GPIO_HAL_PIN_T bit_inicial, uint8_t num_bits){
	
		int i=0;
		while(i<ALARMAS_MAX ){ //inicio todas las alarmas con evento vacio
			ALARMAS[i].id = ID_VOID;
			i++;
		}
		cont_alarmas=0;
		funcion_callback_fifo_alarmas = funcion_parametro;
		//**************************************************************gpio
		bit_error = bit_inicial;
		n_bit_error = num_bits;
		gpio_hal_sentido(bit_error,n_bit_error,GPIO_HAL_PIN_DIR_OUTPUT);
		gpio_aux_escribir_error(0);
		
}

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
auxData){
	
	uint16_t periodic = (retardo >> 31 ); // retardo >> 31
	uint32_t time = (retardo & 0x7FFFFFFF);
	
	if(retardo == 0){
		int i=0, encontrado=0;
		while(i<ALARMAS_MAX && !encontrado && cont_alarmas>0){ // hay alguna alarma
			if(ALARMAS[i].id == ID_evento){
				
				ALARMAS[i].id = ID_VOID;
				ALARMAS[i].time = 0;
				cont_alarmas--;
				encontrado = 1;
				
			}
			i++;
		}
	}
	else { // se puede programar una alarma m�s
		int freeIndex;
		int i=0, encontrado = 0;
		while(i<ALARMAS_MAX && !encontrado ){
			if(ALARMAS[i].id == ID_VOID){
				freeIndex=i;
			}else if(ALARMAS[i].id==ID_evento){
				encontrado=1;
			}
			i++;
		}
		if(encontrado){
			ALARMAS[i-1].time=time;
			ALARMAS[i-1].inicio = temporizador_drv_leer()/us_to_ms;
		}
		else{
			if(cont_alarmas>=ALARMAS_MAX){
					gpio_aux_escribir_error(1);
					while(1);
			}else{
				//**********newALARMA***************
				ALARM newAlarm;
				newAlarm.id = ID_evento;
				newAlarm.auxData = auxData;
				newAlarm.time = time;
				newAlarm.inicio = temporizador_drv_leer()/us_to_ms; //ms actuales de timer1;
				newAlarm.periodico = periodic;
				//*******************************************
			
				ALARMAS[freeIndex] = newAlarm; // lo meto en un indice vac�o
				cont_alarmas++;
			}
		}
	}		
}


/* Esta funci�n comprobar� si hay que disparar el
evento asociado a alguna de las alarmas programadas pendientes. Las
alarmas no peri�dicas se cancelan tras dispararse. Las peri�dicas contin�an
activas de forma indefinida hasta que se eliminen. */
void  alarma_tratar_evento(){
	
	int i=0;
	int now_time;
			while(i<ALARMAS_MAX ){
					now_time = temporizador_drv_leer()/us_to_ms;
						int time_periodo= ALARMAS[i].time;
					int time_inicio = ALARMAS[i].inicio;
				if(ALARMAS[i].auxData == 'C'){
					i +=0;
				}
				if((time_periodo<= now_time - time_inicio) && ALARMAS[i].id!= ID_VOID){ //cuando se vence la alarma
					funcion_callback_fifo_alarmas(ALARMAS[i].id,ALARMAS[i].auxData); //encolas el evento correspondiente
					if(ALARMAS[i].periodico != 0){ //si es periodico 
						ALARMAS[i].inicio = now_time; //actualiza el inicio
					}else{
						alarma_activar(ALARMAS[i].id,0,0); //cancela la alarma
					}
				}
				i++;
		}
	
}
