#include "config_conecta_K.h"
#include "tablero.h"

void
tablero_inicializar(TABLERO *t)
{
	for(size_t i = 0; i < NUM_FILAS; ++i){
		for(size_t j = 0; j < MAX_NO_CERO; ++j) {
				t->columnas[i][j] = INVALIDO;
				celda_inicializar(&t->no_ceros[i][j]);
		}
	}
}

uint8_t
comprobar_empate(TABLERO *t){
	uint8_t hay_vacias=FALSE;
	for(int i=0; i<NUM_FILAS && !hay_vacias;i++){
		for(int j=0; j<MAX_NO_CERO && !hay_vacias;j++){
			if (t->no_ceros[i][j]==0){
				hay_vacias=TRUE;
			}
		}
	}
	return hay_vacias;
}

int
tablero_insertar_celda(TABLERO *t, uint8_t fila, uint8_t columna, CELDA *c)
{
	// comprobar valided posici�n
	if( !tablero_fila_valida(fila) || !tablero_columna_valida(columna)) {
		return ERROR;
	}
	
	// comprobar valided celda
	if (c == NULL) {
		return ERROR;
	}
	
	//comprobar que este libre
	if (!celda_vacia(tablero_leer_celda(t, fila, columna))){
		return ERROR;
	}

	// encontrar primera posici�n libre en el vector
	// de columnas de la fila correspondiente
	size_t col = 0;
	for(; (col < MAX_NO_CERO) && (t->columnas[fila][col] != INVALIDO); ++col);

	// se ha llenado nuestra matriz est�tica
	if(MAX_NO_CERO == col) {
		return ERROR;
	}

	// inserci�n
	t->columnas[fila][col] = columna;
	t->no_ceros[fila][col] = *c;
	
	return EXITO;
}

int
tablero_insertar_color(TABLERO *t, uint8_t fila, uint8_t columna, uint8_t color)
{
	// comprobar valided posici�n
	if( !tablero_fila_valida(fila) || !tablero_columna_valida(columna)) {
		return ERROR;
	}

	// comprobar valided color
	if (!tablero_color_valido(color)) {
		return ERROR;
	}

	//comprovar que este libre
	if (!celda_vacia(tablero_leer_celda(t, fila, columna))){
		return ERROR;
	}
	
	// encontrar primera posici�n libre en el vector
	// de columnas de la fila correspondiente
	size_t col = 0;
	for(; (col < MAX_NO_CERO) && (t->columnas[fila][col] != INVALIDO); ++col);

	// se ha llenado nuestra matriz est�tica
	if(MAX_NO_CERO == col) {
		return ERROR;
	}

	// inserci�n
	t->columnas[fila][col] = columna;
	celda_poner_valor(&t->no_ceros[fila][col], color);
	
	if(!comprobar_empate(t)){return EMPATE;}else{
		return EXITO;
	}
}


int
tablero_borrar_celda(TABLERO *t, uint8_t fila, uint8_t columna)
{
	// comprobar validez posici�n
	if( !tablero_fila_valida(fila) || !tablero_columna_valida(columna)) {
		return ERROR;
	}

	// hay que mirarlos todos
	size_t col = 0;
	for(; (col < MAX_NO_CERO) && (t->columnas[fila][col] != columna); ++col);

	if(col == MAX_NO_CERO) {
		return ERROR;
	}

	t->columnas[fila][col] = INVALIDO;
	celda_inicializar(&t->no_ceros[fila][col]);
	return EXITO;
}

CELDA 
tablero_leer_celda(const TABLERO *t, uint8_t fila, uint8_t columna)
{
	CELDA celda_invalida;

	// marcar celda como invalidad
	celda_inicializar(&celda_invalida);

	// comprobar validez posici�n
	if( !tablero_fila_valida(fila) || !tablero_columna_valida(columna)) {
		return celda_invalida;
	}

	// buscar si celda es valida
	size_t col = 0;
	for(; (col < MAX_NO_CERO) && (t->columnas[fila][col] != columna); ++col);

	if(col == MAX_NO_CERO) {
			return celda_invalida;
	}

	// devolver celda valida
	return t->no_ceros[fila][col];
}

int
tablero_buscar_color(const TABLERO *t, uint8_t fila, uint8_t columna, uint8_t color)
{
	// comprobar validez posici�n
	if( !tablero_fila_valida(fila) || !tablero_columna_valida(columna)) {
		return ERROR;
	}

	// buscar columna en estructura dispersa
	size_t col = 0;
	for(; (col < MAX_NO_CERO) && (t->columnas[fila][col] != columna); ++col);

	if(col == MAX_NO_CERO) {
		return ERROR;
	}

	//si mismo color al que busco devolver encontrada
	if(celda_color(t->no_ceros[fila][col]) == color) {
		return EXITO;
	}
	
	// devolver error si posicion invalida y distinto color
	return ERROR;
}
