#include "linea_serie_drv.h"

char buffer[4];
volatile static int iBuffer = 0;
volatile static int ignorar	= 0;

volatile static STATE state_linea_serie;

//*********************************************
volatile static EVENTO_T id_ev;
volatile static uint32_t aux_ev;
void (*funcion_callback_f_lineaserie)(EVENTO_T, uint32_t); 
volatile static uint32_t timePowerDown;
void (*funcion_callback_alarm_lineaserie)(EVENTO_T, uint32_t, uint32_t);

//********************************************
volatile static GPIO_HAL_PIN_T pin_error;
volatile static uint8_t n_bits_error;

//*******************************************


//*********************************************
//valor-> valor a escribir en el gpio 
//escribe valor en los bits configurados para el error del juego
void gpio_escribir_error_linea_serie(uint32_t valor){
	gpio_hal_escribir(pin_error,n_bits_error,valor);
}
/* 
 pre: funcion_parametro_fifo tiene la funcion para encolar, y pin_serie_error tiene el pin del gpio para 
 los errores de la linea serie 
 post: inicializa la uart0 pasandole las funciones del drv, define 
 */
void init_teclado(void (*funcion_parametro_fifo)(),void (*funcion_parametro_alarma)(), 
	uint32_t timeReinicio,GPIO_HAL_PIN_T pin_serie_error, uint8_t n_serie_bits){
		
	init_UART0(rellenoBuffer, linea_serie_drv_continuar_envio);
	state_linea_serie=ON;
	funcion_callback_f_lineaserie= funcion_parametro_fifo;
	funcion_callback_alarm_lineaserie = funcion_parametro_alarma;
	timePowerDown = timeReinicio;
	pin_error = pin_serie_error;
	n_bits_error = n_serie_bits;
	gpio_hal_sentido(pin_error,n_bits_error,GPIO_HAL_PIN_DIR_OUTPUT);
	gpio_escribir_error_linea_serie(0);
}

/* 
  post: llama a la funcion que envia el array en el hal
 */
void linea_serie_drv_enviar_array(char array[700], uint32_t aux) {
	 aux_ev = aux;
 	 linea_serie_hal_enviar_array(array);
}

/* 
	post: pone el estado de la linea serie a ON 
 */
void transmision_finalizada(){
	state_linea_serie=ON;
}

/*
	pre: ha llegado a esta funcion desde la rutina de servicio de interrupcion
	de la UART0
	post: si no ha llegado al final del buffer, llama a la funcion del hal qeu escribe el 
	caracter que toca en linea serie, si no, encola un evento diciendo que ha acabado la
	transmision
*/
void linea_serie_drv_continuar_envio() {
    if (check_state() != fin_buffer) {
			linea_serie_hal_continuar_envio();
		}else{
			funcion_callback_f_lineaserie(ev_TX_SERIE,aux_ev);
			aux_ev = 0; // reset
		}
}

/*
	pre: se llega aqui desde la RSI de la UART0, caracter_recibido tiene el
	caracter que le ha llegado a dicha rutina de servicio
	post: 
	
  */
void rellenoBuffer(uint32_t caracter_recibido){ //LO HACE DENTRO DE IRQ
	funcion_callback_alarm_lineaserie(POWER_DOWN,0,0); // se cancela el power_down
	if(state_linea_serie == ON){
		ignorar = gpio_hal_leer(pin_error,n_bits_error);
		if(caracter_recibido == '$'){
			if(ignorar == 1){
				gpio_escribir_error_linea_serie(0);
			}
			iBuffer = 0 ;
		}else if(caracter_recibido == '!' && !ignorar){
			buffer[iBuffer]= '\0'; 
				if(iBuffer==3){
					uint32_t value = 0;
					//*************************
					value |= (buffer[0]<<24);
					value |= (buffer[1]<<16);
					value |= (buffer[2]<<8);
					value |= (buffer[3]);		
					//*************************
					funcion_callback_f_lineaserie(ev_RX_SERIE,value);	
					state_linea_serie=OFF;
					iBuffer++;
				}else{ //si el buffer no es exactamente de 3 componentes, error de buffer
					gpio_escribir_error_linea_serie(1);		
				}
				funcion_callback_alarm_lineaserie(POWER_DOWN,timePowerDown,0); // se reaunuda
		}else if(iBuffer >=3 && !ignorar){
				gpio_escribir_error_linea_serie(1);	
				funcion_callback_alarm_lineaserie(POWER_DOWN,timePowerDown,0); // se reaunuda
		}else if (!ignorar){ // guarda el caracter recibido
					buffer[iBuffer]= caracter_recibido;
					iBuffer++;
			}
		}
	}



