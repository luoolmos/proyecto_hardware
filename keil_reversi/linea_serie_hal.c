#include "linea_serie_hal.h"

void UART0_ISR(void) __irq;
volatile uint32_t caracter_recibido;
static volatile int uart0_send_i = 0;
static volatile int uart0_recv_i = 0;
void (*funcion_callback_uart)(uint32_t);
void (*funcion_callback_uart_send)();


static char buffer_hal[700];
static volatile int indice;
static volatile uint8_t buffer_state;

// devuelve el estado del buffer (notInicializado,inProgress o fin_buffer)
uint8_t check_state(){
	return buffer_state;
}

/* 
	pre: array es un vector de caracteres

	post: ha copiado array a la variable local buffer_state y define que el buffer
	esta en progreso de enviarse
 */
void linea_serie_hal_enviar_array(char array[])	{
	unsigned i = 0;
	// solo envio si no hay algo ya en proceso de enviarse
	if(buffer_state != inProgress){
		while(array[i] != '\0'){ //copio array
			buffer_hal[i]=array[i];
			i++;
		}
		buffer_hal[i]='\0';
		// inicializo indice
		indice = 0;
		buffer_state = inProgress;
		linea_serie_hal_continuar_envio();
	}
}

/* 
	pre: 

	post: miro si el caracter a enviar no es el final y llamo a
	la funcion para enviar el caracter  
 */

void linea_serie_hal_continuar_envio() {
	// si la siguiente es \0 significa que es el final del buffer
    if (buffer_hal[indice+1] == '\0') {
				buffer_state = fin_buffer;
    } 
		linea_serie_hal_enviar_caracter(buffer_hal[indice]);
		indice++;
  
}

/* 
	pre: 

	post: miro si el caracter a enviar no es el final y llamo a
	la funcion del hal para enviar el caracter  
 */

void init_UART0(void (*funcion_recive)(), void (*funcion_send)()){
	// Configuraci�n de los pines TXD0 y RXD0
    PINSEL0 |= 0x5;  // TXD0 y RXD0

    // Configuraci�n de la velocidad de transmisi�n
    U0LCR = 0x83;  // 8 bits de datos, 1 bit de stop, sin paridad (DLAB=1)
    U0DLM = 0;     // Parte alta del divisor (para 9600 baudios)
    U0DLL = 97;    // Parte baja del divisor (para 9600 baudios)
    U0LCR = 0x03;  // Desactivar DLAB

    // Habilitar interrupciones de UART0
    U0IER = 0x1;  // Habilitar interrupciones de recepci�n (RBRIE) y transmisi�n (THREIE)

    // Habilitar la UART0
    //U0TER |= (1 << 7);  // Habilitar la transmisi�n

    VICVectAddr4 = (unsigned long)UART0_ISR;
    VICVectCntl4 = 0x20 | 6;  // Configurar el canal de interrupci�n (6) para UART0_ISR
		VICIntEnable |= 0X00000040;
		funcion_callback_uart = funcion_recive;
		funcion_callback_uart_send = funcion_send;
		buffer_state = notInicializado;
}

// Rutina de servicio de interrupci�n para UART0
/* 
	post: si estoy recibiendo un caracter llamo a funcion_callback_uart con el 
	caracter caracter_recibido, si no , llamo a la funcion funcion_callback_uart_send
	y continuara con el envio del array
 */
__irq void UART0_ISR(void) {
		VICVectAddr = 0;
		if ((U0IIR & 0xE) == 4) {
		  caracter_recibido = U0RBR;
			funcion_callback_uart(caracter_recibido);
			uart0_recv_i++;
		}else{
			funcion_callback_uart_send();
			uart0_send_i++;
			//VICVectAddr = 0;
		}
    // Limpiar la bandera de interrupci�n
}

/* 
	post: muestra un caracter por la UART0
 */
void linea_serie_hal_enviar_caracter(char caracter) {
    // Enviar el car�cter al registro THR
  U0THR = caracter;
	U0IER |= 0x02;  // Habilitar interrupciones de recepci�n (RBRIE) y transmisi�n (THREIE)

}
