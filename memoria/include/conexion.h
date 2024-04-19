#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>
#include <../include/archivo_proceso.h>
#include <../include/conversores.h>

void procesar_conexion(void* args);
void enviar_pid(int socket_cliente, uint32_t pid);
void enviar_instruccion(int socket, char* instruccion);
extern t_list* archivos_procesos;

#endif