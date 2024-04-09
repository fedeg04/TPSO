#include <../include/conexion.h>

void procesar_conexion(void* args_void) {
    conexion_args_t* args = (conexion_args_t*) args_void;
    int socket_cliente = args->socket_cliente;
    t_log* logger = args->logger;
    free(args);
    return;
}