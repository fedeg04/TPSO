#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>
#include <../include/procesos.h>

void procesar_conexion(void* args);
void enviar_pid(int socket_cliente, uint32_t pid);

#endif