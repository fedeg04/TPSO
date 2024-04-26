#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>
#include <../include/registros.h>
#include <../include/conversores.h>
#include <../include/frees.h>


void procesar_conexion_interrupt(void* args);
void procesar_conexion_dispatch(void* args);
void recibir_pcb(int socket, proceso_t* pcb);
extern int memoria_fd;
extern registros_t* registros_cpu;
extern proceso_t* pcb;
char* recibir_instruccion(int socket);
void ejecutar_instruccion(char* instruccion, t_log* logger, uint32_t pid);
void set_registros(char* registro, uint32_t valor);
uint32_t get_valor_registro(char* registro);
void enviar_pid_pc(uint32_t pid, uint32_t pc, int socket);

#endif