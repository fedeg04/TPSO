#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>
#include <../include/registros.h>

void procesar_conexion(void* args);
void conectar_interfaz(char* interfaz, int socket);

#endif