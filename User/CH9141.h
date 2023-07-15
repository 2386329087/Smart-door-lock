#ifndef __CH9141_H
#define __CH9141_H

#include "debug.h"

void CH9141_Init(void);
uint16_t CH9141_uartAvailableBLE();
uint32_t CH9141_uartReadBLE(char * buffer , uint16_t num);

#endif
