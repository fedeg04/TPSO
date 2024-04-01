#ifndef UTILS_SOCKETS_H_
#define UTILS_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>

typedef struct {
    int socket_server;
    t_log* logger;
} conexion_args_t;

typedef void* (*procesar_conexion_func_t)(void*);
int iniciar_servidor(t_log* logger, char* puerto);
int esperar_cliente(int socket_servidor, t_log* logger);
int server_escuchar(int socket_server, t_log* logger, procesar_conexion_func_t procesar_conexion_func);
int crear_conexion(t_log* logger, const char* nombre_server, char* ip, char* puerto);
int generar_conexion(t_log* logger, char* nombre_server, char* ip, char* puerto, t_config* config);

#endif