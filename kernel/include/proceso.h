#ifndef PROCESO_H_
#define PROCESO_H_
#include <../include/procesos.h>
#include <../include/main.h>
#include <stdint.h>

registros_t inicializar_registros();
proceso_t* crear_pcb(uint32_t pid);

#endif