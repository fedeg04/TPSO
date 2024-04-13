#ifndef PROCESO_H_
#define PROCESO_H_
#include <../include/procesos.h>

uint32_t pid_siguiente = 1;

registros_t inicializar_registros();
proceso_t crear_pcb();

#endif