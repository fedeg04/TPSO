#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>

void procesar_conexion_interrupt(void* args);
void procesar_conexion_dispatch(void* args);

#endif