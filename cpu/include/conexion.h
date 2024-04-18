#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>
#include <../include/registros.h>
#include <../include/conversores.h>

void procesar_conexion_interrupt(void* args);
void procesar_conexion_dispatch(void* args);
void recibir_pcb(int socket, proceso_t* pcb);
extern int memoria_fd;
extern registros_t* registros_cpu;
char* recibir_instruccion(int socket);
void ejecutar_instruccion(char* instruccion, t_log* logger);

#endif