#include "hello_world.h"

static volatile unsigned int tick_count_decimas = 0;
static volatile unsigned int tick_count = 0;
static volatile unsigned int tick_en_decimas = 10; //un tick cada 10 ms : 100 ms = 1 ds
void (*funcion_callback_fifo_hello)(EVENTO_T, uint32_t); 

static volatile EVENTO_T id_ev_a;
static volatile uint32_t retardo, aux_ev_a;
void (*funcion_callback_alarm_hello)(EVENTO_T, uint32_t, uint32_t); 


/*
 pre: funcion_parametro tiene la funcion para encolar eventos, funcion_parametro_alarma tiene la 
	funcion para añadir alarmas 
 post: define que los pines GPIO_HELLO_WORLD son de salida, y añade las dos funciones que se le
 	han pasado por parametro a dos variables locales para poder usarlas en las funciones del modulo, 
	programo una alarma que encola el ev_LATIDO cada 10 ms periodicamente

*/
void hello_world_inicializar(void (*funcion_parametro)(),void (*funcion_parametro_alarma)()){
	gpio_hal_sentido(GPIO_HELLO_WORLD, GPIO_HELLO_WORLD_BITS, GPIO_HAL_PIN_DIR_OUTPUT );
	funcion_callback_fifo_hello= funcion_parametro;
	funcion_callback_alarm_hello= funcion_parametro_alarma;
	funcion_callback_alarm_hello(ev_LATIDO,0x8000000A,0);
	temporizador_drv_reloj(1,funcion_callback_fifo_hello,CONTAR_TICK,0);
}
	
/* 
 pre:
 post: suma uno a tick_count que cuenta decenas de ms, si llega a 10 (100 ms = 1 decima de seg)
	suma uno a tick_count_decimas que cuenta decimas de segundo y lo muestra por el gpio
 */
void  hello_world_tick_tack(){
	tick_count++;
	if(tick_count >= tick_en_decimas){
		funcion_callback_fifo_hello(ev_VISUALIZAR_HELLO, 0);
		tick_count = 0;
		tick_count_decimas++;
	}
	
}

void hello_world_visualizar() {
   gpio_hal_escribir(GPIO_HELLO_WORLD,GPIO_HELLO_WORLD_BITS,(tick_count_decimas%128));
}

/* 
 pre: ID_evento es un identificador de evento válido
 post: cuenta ticks, cuando llega a tick_en decimas encola el evento de visualización
			 si llega el evento de visualizacion se muestra en el gpio
  */
// Función para manejar eventos relacionados con el "hello world"
void hello_world_tratar_evento(EVENTO_T ID_evento) {
    // Pre-condición: Asumiendo que ID_evento es un identificador de evento válido

    // Verifica si el evento recibido es un latido
    if (ID_evento == ev_LATIDO) {
				hello_world_tick_tack();
    } 
    // Si el evento recibido es el de visualizar "hello"
    else if (ID_evento == ev_VISUALIZAR_HELLO) {
        // Llama a la función para manejar la visualización de "hello"
        hello_world_visualizar();
    }

}


