#ifndef EVENTOS_H
#define EVENTOS_H

#define MAX_EVENTO_T 10

typedef enum {
	ID_VOID = 0,
	CONTAR_TICK = 1,
	BOTON = 2,
	CONSULTAR_BOTON = 3,
	POWER_DOWN = 4,
	ev_VISUALIZAR_CUENTA =5,
	ev_LATIDO =6,
	ev_VISUALIZAR_HELLO = 7,
	ev_RX_SERIE = 8,
	ev_TX_SERIE = 9
}EVENTO_T; //posibles eventos


#endif







