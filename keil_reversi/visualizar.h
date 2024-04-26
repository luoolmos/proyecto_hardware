#ifndef VISUALIZAR_H
#define VISUALIZAR_H

#include "gpio.h"
#include "io_reserva.h"

// Configura el sentido de los pines asociados a la visualización como salidas.
void visualizar_init(void);

// Escribe el valor de la cuenta en los pines asociados a la visualización.
void visualizar_cuenta(uint32_t cuenta);

#endif // VISUALIZAR_H
