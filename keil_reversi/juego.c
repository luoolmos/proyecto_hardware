#include "juego.h"

//****************************************************
typedef enum JUGADOR{
		JUGADOR_1 = 1,
    JUGADOR_2 = 2
}JUGADOR;

enum BUTTON{
		boton_1 = 1,
    boton_2 = 2
};

enum JUGADA_OPTIONS{
		conf_jugada = 'C',
		cancelar_jugada = 'X'
};

enum RX_OPTIONS {
    VOID = 0,
    NEW = 1,
	END = 2,
    CONF_JUGADA = 3
};

enum TX_OPTIONS {
	ONLY_TX = 0,
    JUGADA = 1,
	STATS = 2,
    INFO_CANCELAR = 3,
};

enum INFO_OPTIONS {
	INI = 0,
    PULSA_1_CANCELAR = 1,
	FILA_INVALIDA = 2,
	COL_INVALIDA = 3,
    FIL_COL_INVALID = 4,
	COMANDO_INVALIDO = 5,
	CELDA_NOT_EMPTY = 6
};

enum CAUSA_END {
	UART = 0,
  BOTON_2 = 1,
	WIN_0 = 2,
	WIN_X = 3,
  EMPAT = 4
};


const char TO_ASCII = '0';

//*****************************************************JUEGO
//****************************************conecta_K
static volatile STATE state_juego = OFF;
static volatile JUGADOR color = JUGADOR_1;
static volatile uint8_t causa_end;
static volatile JUGADOR jugador_ini = JUGADOR_1;
//***************************************provisional
static volatile uint8_t provisional_row;
static volatile uint8_t provisional_column;
static volatile uint8_t provisional;
#define BUFFER_SIZE 12  

//****************************************cuenta
static volatile uint8_t cuenta = 0;


//****************************************DATOS_GPIO
volatile static GPIO_HAL_PIN_T pin_error;
volatile static uint8_t n_bits_error;

//*************************************************TABLERO
TABLERO cuadricula;
static CELDA salida[NUM_FILAS+1][NUM_COLUMNAS+1];

//*************************************************CALLBACK_FUNCTION
void (*funcion_callback_fifo_juego)(EVENTO_T, uint32_t);   //encolar
uint32_t (*funcion_callback_fifo_estadisticas)(EVENTO_T);  	//estadisticas

//*************************************************TIMER_STATS
//********************************************jugada
static volatile uint64_t time_respuesta=0;
static volatile uint64_t time_aux_ini_jugada=0;
static volatile uint64_t cont_respuesta=0;
//*********************************************hay_linea
static volatile uint64_t time_hay_linea=0;
static volatile uint64_t time_aux_ini_hay_linea=0;
static volatile uint64_t cont_hay_linea=0;
//*********************************************total
static volatile uint64_t time_procesado=0;
static volatile uint64_t time_aux_ini_procesado=0;
static volatile uint64_t cont_partidas=0;


//**************************************************EV_VARIABLES
uint32_t ocurrencias_eventos[MAX_EVENTO_T];

//******************funciones_privadas*********************************

//valor-> valor a escribir en el gpio 
//escribe valor en los bits configurados para el error del juego
void gpio_escribir_error(uint32_t valor){
	gpio_hal_escribir(pin_error,n_bits_error,valor);
}

//une a dest, src
void append_text(char *dest, const char *src) {
    // Encontrar el índice del final de dest
    int dest_index = 0;
    while (dest[dest_index] != '\0') {
        dest_index++;
    }

    // Copiar src al final de dest 
    int src_index = 0;
    while (src[src_index] != '\0') {
        dest[dest_index] = src[src_index];
        dest_index++;
        src_index++;
    }

    // Agregar el carácter nulo al final
    dest[dest_index] = '\0';
}


//envia a la pantalla un mensaje determinado
void send_info(uint8_t type){
	
	char buffer[700];
	const char *texto;
	
	switch (type){
		case INI: 
			texto = "***********CONECTA_K ***********\n"
			"Nueva partida: $NEW!\n"
			"Acabar la partida: $END! o BOTON_2 \n"
			"Jugada:$#-#! donde #-# sera el numero \nde fila-columna codificado\n"
			"Introduce un comando para empezar a jugar\n\n\n";
			break;
		case PULSA_1_CANCELAR: 
			texto = "Pulsa el boton 1 para cancelar jugada\n";
			break;
		case FILA_INVALIDA:
			texto = "Fila invalida\n";
			break;
		case COL_INVALIDA:
			texto = "Columna invalida.\n";
			break;
		case COMANDO_INVALIDO:
			texto = "Comando invalido.\n";
			break;
		case FIL_COL_INVALID:
			texto = "Fila y columna invalida.\n";
			break;
		case CELDA_NOT_EMPTY:
			texto = "Celda no vacia.\n";
			break;
	}

    // Inicializar índice para el bucle
    int i = 0;

    // Copiar el texto al buffer caracter por caracter
    while (texto[i] != '\0') {
        buffer[i] = texto[i];
        i++;
    }

    // Agregar el carácter nulo al final del buffer
    buffer[i] = '\0';
		linea_serie_drv_enviar_array(buffer, ONLY_TX);
}


// c1 y c2 estan codificados en ascii
// comprueba si la fila y las columnas son validas
uint8_t comandoNumericoValido(uint8_t c1,uint8_t c2){
	
		uint8_t fila_valida =  c1>'0' && c1<=TO_ASCII + NUM_FILAS;
		uint8_t col_valida  =  c2>'0' && c2<=TO_ASCII + NUM_COLUMNAS;
	
		if(!fila_valida && !col_valida ){
			send_info(COMANDO_INVALIDO);
			gpio_escribir_error(ON);
		}	else if(!fila_valida){
			send_info(FILA_INVALIDA);
			gpio_escribir_error(ON);
		}else if(!col_valida){
			send_info(COL_INVALIDA);
			gpio_escribir_error(ON);
		}
		
		return fila_valida && col_valida;
}


//cambia de jugador1 a jugador2 y viceversa
void change_color(){
	if(color == JUGADOR_1){
		color = JUGADOR_2; 
	}else{
		color = JUGADOR_1;
	}
}

/*	comando |= (buffer[3]<<24);
	comando |= (buffer[2]<<16);
	comando |= (buffer[1]<<8);
	comando |= (buffer[0]);*/
/*funcion que dado un comando (uint32_t) 
 lo transforma para su validez en el juego*/
uint16_t convierteComando(uint32_t comando){
	
	gpio_escribir_error(OFF);
	if(comando == conf_jugada){ 
		return CONF_JUGADA;
	}else{
		uint16_t transformed_comand = VOID; 

		uint8_t c1 = comando >> 24;
		uint8_t c2 = (comando & 0xFF0000)>>16;
		uint8_t c3 = (comando & 0xFF00)>>8;
		
		if(c1 == 'N' && c2 == 'E' && c3 == 'W'){
			transformed_comand =  NEW; 
		}else if(c1 == 'E'&& c2 == 'N' && c3 == 'D'){
			transformed_comand = END;
		}else{
			uint8_t row_column_valido = comandoNumericoValido(c1,c3);
			if(row_column_valido && c2 =='-'){ // comprobamos si son numeros validos o un comando
				transformed_comand |= (c1<<8); //en los 8 bits de mas peso, tengo el valor c1
				transformed_comand |= c3; // en los 8 de menos peso, buffer[2]
			}
		}
		return transformed_comand;
	}
}

//auxData = lo que debe encolar Tx como auxData, es decir
//lo que debe hacer tras terminar de escribir el tablero 
//envia el tablero a la uart con un texto 
void send_tablero (char *texto, uint32_t auxData){
	
	conecta_K_visualizar_tablero(&cuadricula, salida); //cargamos el tablero
	
	char tablero_buffer[500];
	unsigned index = 0;
	unsigned i = 0;
	unsigned j = 0;
	unsigned iText = 0;
	
	while (texto[iText] != '\0') {
			tablero_buffer[index++] = texto[iText++];
	}
	
	for (i=1; i <=NUM_FILAS; i++){
		tablero_buffer[index++] = i + TO_ASCII; // para que salga bien el ascii
		tablero_buffer[index++] ='|';
		
		for(j= 1; j<= NUM_COLUMNAS; j++){
			if(i == provisional_row+1 && j == provisional_column+1 && provisional){
				tablero_buffer[index ++] = '#';
			}else if(celda_blanca(salida[i][j])){
				tablero_buffer[index++] = 'O';
			}else if(celda_negra(salida[i][j])){
				tablero_buffer[index++] = 'X';
			}
			else if(celda_vacia(salida[i][j])){
				tablero_buffer[index++] = ' ';
			}
			tablero_buffer[index++] ='|';
		}
		tablero_buffer[index++] ='\n';
	}
	tablero_buffer[index++] = '-';
	tablero_buffer[index++] = '|';
	for(j= 1; j<= NUM_COLUMNAS; j++){
		tablero_buffer[index++] = j + TO_ASCII; // para que salga bien el ascii
		tablero_buffer[index++] ='|';
	}
	tablero_buffer[index++] ='\n';
	tablero_buffer[index++] ='\n';
	tablero_buffer[index++] ='\n';
	tablero_buffer[index++] ='\0';
	//enviarArray
	linea_serie_drv_enviar_array(tablero_buffer, auxData); // auxData indica el tipo de TX a enviar.
}


//transforma el uint  a ascii
void uint32_to_ascii(uint32_t number, char* buffer) {
    if (buffer == NULL) {
        // Manejo de error: buffer no válido
        return;
    }

    // Índice para seguir la cadena generada
    int index = 0;

    // Manejar el caso especial de 0
    if (number == 0) {
        buffer[index++] = '0';
    } else {
        // Proceso inverso: extraer dígitos del número y agregarlos al búfer
        while (number > 0 && index < BUFFER_SIZE - 1) {
            int digit = number % 10;
            buffer[index++] = digit + '0';
            number /= 10;
        }

        // Invertir la cadena en el búfer
        for (int i = 0, j = index - 1; i < j; i++, j--) {
            char temp = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = temp;
        }
    }

    // Agregar el terminador nulo
    buffer[index++] = '\n';
		buffer[index] = '\0';
}

//alamcena en texto la información a mostrar tras la llegada de un end
// Causa, tiempo de procesado, tiempo medio de jugadas, tiempo medio en producirse victoria
void send_info_end(char* texto){

	texto[0]='\0';
	switch(causa_end){	
		case UART: //uart
			append_text(texto, "Finalizado por uart.\n");
			break;
		case BOTON_2: //boton2
			if(color == JUGADOR_1){
					append_text(texto, "Jugador 1 se ha rendido.\n");
			}else{
					append_text(texto, "Jugador 2 se ha rendido.\n");
			}
			break;
		case WIN_0: //win O
			append_text(texto, "Finalizado por victoria de O.\n");
			break;
		case WIN_X: //win X
			append_text(texto, "Finalizado por victoria de X.\n");
			break;
		case EMPAT: //empate
			append_text(texto, "Finalizado por empate.\n");
			break;
	}
	//****************STATS FIFO***************
	uint8_t i_ev = 0;
	uint32_t ev_Ocurrences[MAX_EVENTO_T];
	char aux;
	for(i_ev = 0; i_ev< MAX_EVENTO_T; i_ev++){
		ev_Ocurrences[i_ev] = funcion_callback_fifo_estadisticas(i_ev);
		ev_Ocurrences[i_ev] -= ocurrencias_eventos[i_ev];
	}
	append_text(texto,"\n************FIFO_estadisticas************\n");
	append_text(texto,"Numero elementos encolados: ");
	uint32_to_ascii(ev_Ocurrences[ID_VOID],&aux);
	append_text(texto,&aux);
	append_text(texto,"CONTAR_TICK_apariciones: ");
	uint32_to_ascii(ev_Ocurrences[CONTAR_TICK],&aux);
	append_text(texto,&aux);
	append_text(texto,"BOTON_apariciones: ");
	uint32_to_ascii(ev_Ocurrences[BOTON],&aux);
	append_text(texto,&aux);
	append_text(texto,"POWER_DOWN_apariciones: ");
	uint32_to_ascii(ev_Ocurrences[POWER_DOWN],&aux);
	append_text(texto,&aux);
	append_text(texto,"ev_VISUALIZAR_CUENTA_apariciones: ");
	uint32_to_ascii(ev_Ocurrences[ev_VISUALIZAR_CUENTA],&aux);
	append_text(texto,&aux);
	append_text(texto,"ev_LATIDO_apariciones: ");
	uint32_to_ascii(ev_Ocurrences[ev_LATIDO],&aux);
	append_text(texto,&aux);
	append_text(texto,"ev_VISUALIZAR_HELLO_apariciones: ");
	uint32_to_ascii(ev_Ocurrences[ev_VISUALIZAR_HELLO],&aux);
	append_text(texto,&aux);
	append_text(texto,"ev_RX_SERIE_apariciones: ");
	uint32_to_ascii(ev_Ocurrences[ev_RX_SERIE],&aux);
	append_text(texto,&aux);
	append_text(texto,"ev_TX_SERIE_apariciones: ");
	uint32_to_ascii(ev_Ocurrences[ev_TX_SERIE],&aux);
	append_text(texto,&aux);
	append_text(texto,"\n");
	//*****************************************
	//*****************STATS*******************
	append_text(texto,"******************TIME******************\n");
	append_text(texto, "Time procesado(ms): ");
	uint32_to_ascii(time_procesado,&aux);
	append_text(texto, &aux);
	append_text(texto, "Time total jugadas(ms): ");
	uint32_to_ascii(time_respuesta,&aux);
	append_text(texto, &aux);
	append_text(texto, "Time medio jugadas(ms): ");
	uint64_t medioRespuesta = 0;
	if(cont_respuesta > 0){
		medioRespuesta = (time_respuesta / cont_respuesta);
	}
	uint32_to_ascii(medioRespuesta,&aux);
	append_text(texto, &aux);
	append_text(texto, "Time total hayLinea(us): ");
	uint32_to_ascii(time_hay_linea,&aux);
	append_text(texto, &aux);
	append_text(texto, "Time medio hayLinea(us): ");
	uint64_t medioLinea = 0;
	if(cont_hay_linea >0){
		medioLinea = time_hay_linea / cont_hay_linea;
	}
	uint32_to_ascii(medioLinea,&aux);
	append_text(texto, &aux);
	append_text(texto, "\n \nIntroduce $NEW! para comenzar una nueva partida. \n\n");
	//****************************************
}


//gestiona la llegada de un end
void juego_tratar_end(){
	if(state_juego == ON){
		uint64_t time_aux = clock_get_us() / us_to_ms; // time_aux = t0 (ms)
		time_procesado = time_aux - time_aux_ini_procesado; // time_proceso = tiempo actual menos time en el que se inicio la partida
		send_tablero("",STATS); // escribimos el tablero, posteriormente las STATS
		if(jugador_ini == color){
			change_color();
		}
		state_juego = OFF;				//cambiamos el estado de juego a OFF==END
	}
}

//pre: cancelada indica si la jugada ha sido cancelada o confirmada
//post: trata la jugada, según el estado del juego, y si está cancelada o no
//			si el estado del juego es ON, y no se ha cancelado la jugada, inserta la jugada provisional,
//		comprueba si hay linea, y en ese caso toma el tiempo del reloj, y trata el end.
//		En caso contrario, cambia de jugador y muestra el tablero con la jugada realizada.
//
//			si el estado del juego es ON, y se ha cancelado la jugada, se muestra un 
//		mensaje que lo indica y el tablero tras esta acción, sin la jugada provisional.
//		
//			si el estado del juego es OFF, no se realiza niguna acción

void confirmar_jugada(uint8_t cancelada){
	char *texto;
	provisional = OFF; // el tablero ya no es provisional
	if(state_juego == ON){
		if(!cancelada){ // si la jugada ha sido confirmada == (no cancelada) 
				uint8_t insertada = tablero_insertar_color(&cuadricula, provisional_row, provisional_column, color);
				if(insertada == EXITO) { //inserta
					time_aux_ini_hay_linea = clock_get_us();
					uint8_t hay_linea = conecta_K_hay_linea_c_arm_ambos(&cuadricula,provisional_row,provisional_column,color);
					//********************calculo_tiempo_hasta_linea******************
					uint64_t time_until_hay_linea = clock_get_us();  
					cont_hay_linea++;
					time_hay_linea += (time_until_hay_linea-time_aux_ini_hay_linea);
					//****************************************************************
					if(hay_linea) { //comprueba victoria
						causa_end = 1 + color; // las causas de end de victoria aparecen a partir del case 2
						juego_tratar_end();   // gestiona el fin
						
					}else{ // si no hay victoria
						change_color();		//cambia de jugador
						if(color == JUGADOR_1){texto = "\nTurno de Jugador 1: O \n";}
						else{ texto = "\nTurno de Jugador 2: X \n";}
						provisional_column=0;
						provisional_row =0;
						send_tablero(texto, JUGADA); // jugada terminada necesitamos actulizar tiempo jugada
					}
				}else if(insertada == EMPATE){
					causa_end = EMPAT;
					juego_tratar_end();
				}else{
					texto = "Error al insertar la ficha.\n\n";
					send_tablero(texto, ONLY_TX); // jugada terminada pero no actulizamos time jugada
				}
		}else{
			texto = "Jugada cancelada.\n";
			send_tablero(texto, ONLY_TX); // jugada terminada pero no actulizamos time jugada
		}
	}
	
}

//pre: 	ID_evento = evento válido
//		auxData = id del boton pulsado, [1,2]
//post: según el boton pulsado, realiza 
//		  [1] -> suma  1 a cuenta y en caso de una jugada provisional activa, la cancela
//		  [2] -> resta 1 a cuenta y manda terminar el juego con motivo boton2
// 		y encola el evento correspondiente a visualizar cuenta
void juego_tratar_evento_boton(uint32_t auxData){
	
    if (auxData==boton_1){ // si se ha pulsado boton1
      cuenta++; //sumas uno a cuenta
			if(state_juego == ON && provisional){ // si hay una jugada provisonal activa
				//- Para CANC_JUGADA: Cancela la jugada y cancela la alarma.
				uint8_t cancelada = 1;
				alarma_activar(ev_RX_SERIE,0,cancelar_jugada); 
				confirmar_jugada(cancelada);
			}
    }else if (auxData==boton_2){
      cuenta--;
			if(state_juego == ON){ // si el juego esta activo
				causa_end = BOTON_2;
				juego_tratar_end();
			}
    }
	// ID_evento = visualizar cuenta
	funcion_callback_fifo_juego(ev_VISUALIZAR_CUENTA,cuenta); 
}

//***********************************************************************

//pre: funcion_parametro se corresponde con la funcion que encola eventos
//		error_command indica el bit del gpio en el que se deben marcar los errores del juego
//		n_error_command indica el numero de bits a modificar en el gpio
//post: inicializa el módulo juego dejándolo listo para empezar a tratar comandos, 
//		envía instrucciones de juego
void juego_inicializar(void (*funcion_parametro)(),uint32_t (*funcion_estadisticas)(),GPIO_HAL_PIN_T error_command , uint8_t n_error_command){
	//****************inicializar variables
	cuenta = 0;
	cont_respuesta=0;
	cont_hay_linea=0;
	time_respuesta=0;
	time_hay_linea = 0;
	state_juego = OFF;

	//************************************************funcion_encolar
	funcion_callback_fifo_juego= funcion_parametro;
	funcion_callback_fifo_estadisticas = funcion_estadisticas;

	//***********************************************************************gpio
	pin_error = error_command;
	n_bits_error = n_error_command;
	gpio_hal_sentido(pin_error,n_bits_error, GPIO_HAL_PIN_DIR_OUTPUT);
	gpio_escribir_error(OFF); //marco como desactivado

	//*****************************envio_informacion_inicial
	send_info(INI);
}


/* 	pre: ID_evento: tipo de evento a tratar
		   auxData: comando a tratar
 *  POST:
* 	Se maneja el evento de recepción de datos por la serie.
 *   - Si el comando recibido es VOID, no realiza acciones adicionales.
 *   - Si el comando recibido es NEW, inicia el juego si el estado es OFF.
 *   - Para otros comandos:
 *     - Si el estado del juego es ON:
 *       - Para CONF_JUGADA: Confirma una jugada exitosa.
 *       - Para END: Trata el fin del juego.
 *       - Para otros comandos (jugadas):
 *         - Calcula el tiempo de respuesta entre jugadas.
 *         - Si la fila y columna son válidas y la celda está vacía, carga datos provisionales y activa una alarma.
 *         - Si la celda no está vacía, envía un mensaje de información sobre el fallo.
 *     - Si el estado del juego es OFF, no realiza acciones.
 * - Se maneja el evento de transmisión por la serie.
 *   - Si el tipo de transmisión es JUGADA, se registra el inicio del tiempo de jugada.
 *   - Si el tipo de transmisión es STATS, se envían las estadísticas al finalizar el juego.
 *   - Si el tipo de transmisión es INFO_CANCELAR, se envía un mensaje para mostrar "pulsa 1 para cancelar jugada".
 * -Se maneja el evento de botón tratado*/
void juego_tratar_evento(EVENTO_T ID_evento, uint32_t auxData){
	
	if(ID_evento == ev_RX_SERIE){
	 uint16_t true_comand = convierteComando(auxData); //convierte el evento a tratar para un fácil manejo
		
		switch(true_comand){//maquina de estados según el comando recibido
			
			case VOID:
				funcion_callback_fifo_juego(ev_TX_SERIE,ONLY_TX); // ONLY_TX el ev_TX no hace nada adicional 
				break;
			
			case NEW:
				if (state_juego != ON) { // Solo inicia el tablero si el state = OFF, sino lo ignora
					state_juego = ON;
					
					// Inicializa el tablero y el juego
					if(cont_partidas == 0){
						init_juego(&cuadricula); //carga el de test
					}else{
						tablero_inicializar(&cuadricula); // tablero vacío
					}
					cont_partidas++;
					
					//*************tiempos_de_inicio**************
					time_aux_ini_procesado = clock_get_us()/ us_to_ms;
					time_aux_ini_hay_linea = 0;
					time_hay_linea = 0;
					cont_hay_linea=0;
					//********************************************
					
					//************fifo_stats***********************
					uint8_t i=0;
					for(i = 0; i < MAX_EVENTO_T; i++){
						ocurrencias_eventos[i]= funcion_callback_fifo_estadisticas(i);
					}
					jugador_ini = color;
					//Establece el texto asociado al tablero según el jugador inicial
					char *texto;
					if(color == JUGADOR_1){texto = "Turno de Jugador 1:  O \n";}
					else{ texto = "Turno de Jugador 2: X \n";}
					
					conecta_K_visualizar_tablero(&cuadricula, salida); 
					send_tablero(texto,JUGADA); // necesitamos actualizar el time de juego
					
				}else{
					funcion_callback_fifo_juego(ev_TX_SERIE,ONLY_TX); //TERMINA UN NEW INVALIDO
				}
        break;
				
			default: 
				if (state_juego == ON){
					
					static volatile uint8_t entrada[8];
				  //entrada_inicializar(entrada);
				  uint8_t row, column; 
					uint32_t retardo = 0xBB8; // 0xBB8 = 3000ms = 3s 
					
					uint64_t time_until_jugada=0;		
					uint8_t cancelada = 0;
					
					switch(true_comand){
						
						case CONF_JUGADA:
							cancelada = 0;
							confirmar_jugada(cancelada); //confirmar jugada exitosa
							break;
						
//						case CANC_JUGADA:
//							cancelada = 1;
//							alarma_activar(ev_RX_SERIE,0,cancelar_jugada); 
//							confirmar_jugada(cancelada); //confirmar jugada cancelada
//							break;
						
						case END:
							juego_tratar_end();
							break;
						
						default: //jugada
								//******************time_respuesta*********************
								time_until_jugada= clock_get_us() / us_to_ms;	
								time_respuesta += (time_until_jugada-time_aux_ini_jugada); //tiempo entre jugadas
								cont_respuesta++;
								//*****************************************************
						
								//en los 8 bits de mas peso, tengo el valor fila
								// en los 8 de menos peso, columna  
								row = ((true_comand>>8)-1) - '0';
								column = ((true_comand & 0xFF)-1) -'0';
								
						//*********************ejecuciónDeJuego*******************************************
								if(tablero_fila_valida(row) && tablero_columna_valida(column) && tablero_color_valido(color)){	
									if (celda_vacia(tablero_leer_celda(&cuadricula, row, column))){
										//**************carga_datos_provisionales**************
										provisional_column = column;
										provisional_row = row;
										provisional=1;
										//*****************************************************
										alarma_activar(ev_RX_SERIE,retardo, conf_jugada); //activa alarma en 3s que confirma jugada
										send_tablero("", INFO_CANCELAR); //indica que debe mostrar la informacion para cancelar
									}else{
										send_info(CELDA_NOT_EMPTY); // envia informacion sobre el fallo
									}
								}
						//********************************************************************************
								break;
					}
				}else{
					funcion_callback_fifo_juego(ev_TX_SERIE,ONLY_TX); // 0-> no hace nada adicional
				}
		}
	}else if(ID_evento == ev_TX_SERIE){
		transmision_finalizada();
		char texto[700];
		switch (auxData){
			case JUGADA: // se ha impreso el tablero y empiza a contar tiempo de jugada
				time_aux_ini_jugada = clock_get_us() / us_to_ms; 
				break;
			case STATS: // ha finalizado el juego y tiene que mandar las stats
				send_info_end(texto);
				linea_serie_drv_enviar_array(texto,ONLY_TX);
				break;
			case INFO_CANCELAR: // mostrar "pulsa 1 para cancelar jugada" partida
				send_info(PULSA_1_CANCELAR);
				break;
			
		}
	}else if(ID_evento == BOTON){
		//activar_estado(auxData);
		juego_tratar_evento_boton(auxData);
	}
}
	


