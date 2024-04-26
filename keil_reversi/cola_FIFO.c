#include "cola_FIFO.h"

typedef struct EVENT{
	EVENTO_T id; // id del evento
	uint32_t	auxData;	// a?n no se bien
	uint8_t estado;	// tratado | no tratado
	//uint32_t	i,
} EVENT; 

typedef struct {
	EVENT cola[MAX_EVENTOS];
	int frente;
  int fin;
  int i;
  int sinTratar;
	int numApariciones[MAX_EVENTO_T];
}COLA_FIFO;

COLA_FIFO cola_eventos;

// Inicialización de la cola. Se le pasa como parámetro 
// el pin del GPIO utilizado para marcar errores. 
void FIFO_inicializar(GPIO_HAL_PIN_T pin_overflow){
		int i; 
	cola_eventos.frente = 0;
	cola_eventos.fin =-1;
	cola_eventos.i=0;				//size
	cola_eventos.sinTratar = 0;
	for(i = 0; i< MAX_EVENTO_T; i++){
		cola_eventos.numApariciones[i]=0;
	}
	
	/* El planificador inicializará la cola indicando que el GPIO para marcar el
	 overflow es el GPIO31 definido convenientemente en la cabecera
	 io_reserva.h como GPIO_OVERFLOW = 31 y de longitud GPIO_OVERFLOW_BITS = 1*/
	gpio_hal_sentido(pin_overflow,GPIO_OVERFLOW_BITS,GPIO_HAL_PIN_DIR_OUTPUT);
}

// Esta función guardará en la cola el evento. El campo ID_evento, que permita
// identificar el evento (p.e. qué interrupción ha saltado) y el campo
// auxData en caso de que el evento necesite pasar información extra. 
void FIFO_encolar(EVENTO_T ID_evento, uint32_t auxData){
	if(cola_eventos.sinTratar != MAX_EVENTOS){
		//*********************************************
		EVENT nuevoEvento;
		nuevoEvento.id = ID_evento;
		nuevoEvento.auxData = auxData;
		nuevoEvento.estado = 0;
		//*********************************************
		uint32_t bit_IRQ= read_IRQ_bit();
		if(bit_IRQ == 1){
			disable_irq();}
		//**********************************************
		cola_eventos.fin = (cola_eventos.fin+1) % MAX_EVENTOS;
		cola_eventos.cola[cola_eventos.fin]= nuevoEvento;
		cola_eventos.i++;
		cola_eventos.sinTratar++;
		cola_eventos.numApariciones[ID_evento]++;
		//*********************************************
		if(bit_IRQ == 1){
			enable_irq();}
		//*********************************************
	}else{
		gpio_hal_escribir(GPIO_OVERFLOW,GPIO_OVERFLOW_BITS,1);
		while(1);
	}
}
// Si hay eventos sin procesar, devuelve un valor distinto de cero y el evento
// más antiguo sin procesar por referencia. Cero indicará que la cola está
// vacía y no se ha devuelto ningún evento.
uint8_t FIFO_extraer(EVENTO_T *ID_evento, uint32_t *auxData){ 
	uint8_t porTratar = 0;
	int i = 0;
	*ID_evento = ID_VOID;
    if(cola_eventos.sinTratar != 0){
			//*********************************************
			uint32_t bit_IRQ= read_IRQ_bit();
			if(bit_IRQ == 1){
				disable_irq();}
			//**********************************************
			while(!porTratar){
				if(i >= cola_eventos.i){
					break;
				}else if(cola_eventos.cola[i].estado == 0){
						cola_eventos.frente = (cola_eventos.frente+1) % MAX_EVENTOS;
					  *ID_evento = cola_eventos.cola[i].id;
						*auxData = cola_eventos.cola[i].auxData;
						cola_eventos.cola[i].estado = 1; 
						cola_eventos.sinTratar--;
						porTratar=1;
				}else{
					i++;
				}
			}
			//*********************************************
			if(bit_IRQ == 1){
				enable_irq();}
		//*********************************************
    }
		return *ID_evento;
}


// Dado un identificador de evento nos devuelve el número total de veces que ese
// evento se ha encolado. El evento VOID nos devolverá el total de eventos
// encolados desde el inicio. 
uint32_t FIFO_estadisticas(EVENTO_T ID_evento){
		if(ID_evento == ID_VOID){
			return cola_eventos.i;
		}else{
			return cola_eventos.numApariciones[ID_evento];
		}
}
