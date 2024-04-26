#include "planificador.h"

const uint32_t USUARIO_AUSENTE =  12;
const uint32_t SEC_WD =  1;

/* hay varios tests, cada uno prueba una funcionalidad del juego
	, lo cual es muy util para probar que todo vaya correctamente
	antes de lanzar el juego con todo*/ 
#define test 1

#if test==1 //test JUEGO COMPLETO
	//TEST JUEGO
	
	/* El planificador b�sicamente estar� a
	la espera pendiente de la cola de eventos. Cuando aparezca un evento nuevo
	no tratado, lo procesar�. 
*/


void planificador(void){
	int valorPowerDown = USUARIO_AUSENTE*miliToSec; //12000
	valorPowerDown |= 0x80000000;
	//iniciamos la cola pasandole por parametro el pin que corresponde al overfow	
	FIFO_inicializar(GPIO_OVERFLOW); 
	//inicializamos las alarmas pasandole la funcion de encolar y los bits del gpio donde marcar el error
	alarma_inicializar(&FIFO_encolar, GPIO_OVERFLOW,GPIO_OVERFLOW_BITS);
	//inicializamos el hello world pasandole la funcion de encolar y la de activar una alarma
	hello_world_inicializar(&FIFO_encolar, &alarma_activar);
	//avisar_hello_world_tick_tack();
	// inicializamos el teclado pasandole la funcion de encolar y el pin del gpio que 
	//corresponde a un error de overflow en la linea serie
	init_teclado(&FIFO_encolar, &alarma_activar,valorPowerDown, GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS);
	//inicializamos el juego pasandole la funcion de encolar y el pin del gpio que
	//corresponde a un comando invalido
	juego_inicializar(&FIFO_encolar,&FIFO_estadisticas,GPIO_COMANDO_ERROR,GPIO_COMANDO_ERROR_BITS);
	//inicalizamos el visualizador de la  cuenta
	visualizar_init();
	//inicializamos los botones pasandole la funcion de encolar y la de activar una alarma
	init_boton(&FIFO_encolar, &alarma_activar);
	
	//programamos una alarma que encolara el evento POWER_DOWN a los 12 segundos 
	alarma_activar(POWER_DOWN,valorPowerDown,0);
	WD_hal_inicializar(SEC_WD);
	
	while(1){
		uint8_t ev_nuevo;
		EVENTO_T evento = ID_VOID;
		uint32_t auxData;
		//extraemos evento
		ev_nuevo=FIFO_extraer(&evento, &auxData);
			//si es VOID es que no hay nada en la cola, asi que nos quedamos en power_hal_wait() 
			if(evento == ID_VOID){
				power_hal_wait();
			}else{
				WD_hal_feed();
				if(evento ==  CONTAR_TICK){
					// tratas el evento desde la alarma
					alarma_tratar_evento();
				}else if(evento ==  BOTON){
					// si te llega un boton, lo marcas como pulsado, reseteas la alarma del power down y 
					// se lo pasas al juego para que lo trate
					activar_estado(auxData); 			
					alarma_activar(POWER_DOWN,valorPowerDown,0);	
					juego_tratar_evento(BOTON,auxData);
				}else if(evento == CONSULTAR_BOTON){
					tratar_boton();
				}else if (evento == ev_VISUALIZAR_CUENTA){
					visualizar_cuenta(auxData);
				}else if (evento == ev_LATIDO || evento == ev_VISUALIZAR_HELLO){
					// si te llega un evento contar_tick llamamos al contador de decimas del modulo hello world
					hello_world_tratar_evento(evento);
				}else if (ev_nuevo == POWER_DOWN){
					power_hal_deep_sleep();
					init_teclado(&FIFO_encolar, &alarma_activar,valorPowerDown, GPIO_SERIE_ERROR, GPIO_SERIE_ERROR_BITS);
					//FIFO_encolar(ev_TX_SERIE,0);
				}else if (evento == ev_RX_SERIE){
					juego_tratar_evento(ev_RX_SERIE,auxData);
				}else if (evento == ev_TX_SERIE){		
					juego_tratar_evento(ev_TX_SERIE,auxData);
				}
			}
		}
}

#endif


	


#if test==2 //test OVERFLOW_COLA
	void planificador(void){
	FIFO_inicializar(GPIO_OVERFLOW); 
	while(1){
		FIFO_encolar(ev_LATIDO,0);
		power_hal_wait();
	}
}
	


#endif


#if test==3 //test OVERFLOW ALARMAS
	
	/* El planificador b�sicamente estar� a
	la espera pendiente de la cola de eventos. Cuando aparezca un evento nuevo
	no tratado, lo procesar�. 
*/
void planificador(void){
	FIFO_inicializar(GPIO_OVERFLOW); 
	alarma_inicializar(&FIFO_encolar, GPIO_OVERFLOW,GPIO_OVERFLOW_BITS);
	int valorPowerDown = USUARIO_AUSENTE*miliToSec; //12000
	valorPowerDown |= 0x80000000;
	alarma_activar(POWER_DOWN,valorPowerDown,0);
	alarma_activar(ev_VISUALIZAR_CUENTA,valorPowerDown,0);
	alarma_activar(ev_VISUALIZAR_HELLO,valorPowerDown,0);
	alarma_activar(CONSULTAR_BOTON,0x80000064,1); //100ms y periodico
	while(1){
		EVENTO_T evento;
		uint32_t auxData;
		
			if((FIFO_extraer(&evento, &auxData))==0){
				power_hal_wait();
			}
			alarma_activar(ev_LATIDO,0x80000064,1); //alarma numero 5
	}
}

#endif


#if test==4 //test HELLO_WORLD_TICK
	//TEST JUEGO
	
	/* El planificador b�sicamente estar� a
	la espera pendiente de la cola de eventos. Cuando aparezca un evento nuevo
	no tratado, lo procesar�. 
*/
void planificador(void){
	FIFO_inicializar(GPIO_OVERFLOW); 
	alarma_inicializar();
	hello_world_inicializar();
	while(1){
		uint8_t ev_nuevo;
		EVENTO_T evento;
		uint32_t auxData;
		
			if((ev_nuevo=FIFO_extraer(&evento, &auxData))==0){
				power_hal_wait();
			}
			if(ev_nuevo ==  CONTAR_TICK){
				//hello_world_tick_tack();
				alarma_tratar_evento();
			}else if (ev_nuevo == ev_LATIDO){
				hello_world_tratar_evento();
			}else if (ev_nuevo == ev_VISUALIZAR_HELLO){
				hello_world_tick_tack();}
	}
}


#endif


#if test==5 //test BOTON
	/* El planificador b�sicamente estar� a
	la espera pendiente de la cola de eventos. Cuando aparezca un evento nuevo
	no tratado, lo procesar�. 
*/
void planificador(void){
	FIFO_inicializar(GPIO_OVERFLOW); 
	hello_world_inicializar();
	alarma_inicializar();
	init_boton();
	while(1){
		uint8_t ev_nuevo;
		EVENTO_T evento;
		uint32_t auxData;
		
			if((ev_nuevo=FIFO_extraer(&evento, &auxData))==0){
				power_hal_wait();
			}
			if(ev_nuevo ==  CONTAR_TICK){
				alarma_tratar_evento();
			}else if(ev_nuevo ==  BOTON){
				activar_estado(auxData);
				juego_tratar_evento(evento,auxData);
			}else if(ev_nuevo == CONSULTAR_BOTON){
				tratar_boton();}
	}
}

#endif

#if test==6 //test POWER_DOWN
	
	/* El planificador b�sicamente estar� a
	la espera pendiente de la cola de eventos. Cuando aparezca un evento nuevo
	no tratado, lo procesar�. 
*/
void planificador(void){
	FIFO_inicializar(GPIO_OVERFLOW); 
	alarma_inicializar();
	int valorPowerDown = USUARIO_AUSENTE*miliToSec; //12000
	valorPowerDown |= 0x80000000;
	alarma_activar(POWER_DOWN,valorPowerDown,0);
	while(1){
		EVENTO_T evento;
		uint32_t auxData;
		
			FIFO_extraer(&evento, &auxData);
			alarma_tratar_evento();
			if (evento == POWER_DOWN){
				FIFO_estadisticas(BOTON);
				power_hal_deep_sleep();
			}
	}
}

#endif
#if test==7 //test JUEGO
	//TEST JUEGO
	
	/* El planificador b�sicamente estar� a
	la espera pendiente de la cola de eventos. Cuando aparezca un evento nuevo
	no tratado, lo procesar�. 
*/

void planificador(void){
	FIFO_inicializar(GPIO_OVERFLOW); 
	alarma_inicializar();
	visualizar_init();
	init_teclado();
	while(1){
		uint8_t ev_nuevo;
		EVENTO_T evento;
		uint32_t auxData;
			if((ev_nuevo=FIFO_extraer(&evento, &auxData))==0){
				power_hal_wait();
			}
			if(ev_nuevo ==  CONTAR_TICK){
				//hello_world_tick_tack();
				alarma_tratar_evento();
			}else if(ev_nuevo ==  ALARMA_OVERFLOW){
				FIFO_gpio(GPIO_OVERFLOW,GPIO_OVERFLOW_BITS,1);
			}else if (ev_nuevo == ev_LATIDO){
				hello_world_tratar_evento();
			}else if (ev_nuevo == ev_BUFFER_OVERFLOW){
				FIFO_gpio(GPIO_SERIE_ERROR,GPIO_SERIE_ERROR_BITS,1);
			}else if (ev_nuevo == ev_BUFFER_OVERFLOW_OFF){
				FIFO_gpio(GPIO_SERIE_ERROR,GPIO_SERIE_ERROR_BITS,0);
			}else if (ev_nuevo == ev_RX_SERIE){
				time = getTime();
				juego_tratar_evento(evento,auxData);
			}else if (ev_nuevo == ev_TX_SERIE){	
				visualizar_time(time);
				State_ON(); //puedo volver a procesar
				
			}
	}
}

#endif

#if test==8 //test WD
	//TEST JUEGO
	
	/* El planificador b�sicamente estar� a
	la espera pendiente de la cola de eventos. Cuando aparezca un evento nuevo
	no tratado, lo procesar�. 
*/

void planificador(void){
	WD_hal_inicializar(1);
	WD_hal_feed();
	WD_hal_test();

}

#endif

