#ifndef WD_H
#define WD_H

#include <inttypes.h>
#include "llamadas_sistema_hal.h"
#include <LPC210X.H>  


void WD_hal_inicializar(int sec);

void WD_hal_feed(void);

void WD_hal_test(void);


#endif
