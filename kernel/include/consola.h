#ifndef CONSOLA_H_
#define CONSOLA_H_
#include <../include/protocolo.h>
#include <commons/log.h>
#include <pthread.h>

typedef struct {
    op_code opcode;
    char* path;
} iniciar_proceso_t;

typedef struct {
    t_log* logger;
    int socket;
} leer_consola_t;

void leer_consola(void* args_void);
void procesar_instruccion(char* instruccion, t_log* logger, int socket);
void empezar_hilo_consola(pthread_t* hilo_consola, t_log* logger,int socket);

#endif