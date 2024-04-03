#include <../include/conexion.h>


void procesar_conexion(void *args_void)
{
    conexion_args_t *args = (conexion_args_t *)args_void;
    int socket_server = args->socket_server;
    t_log *logger = args->logger;
    free(args);
    return;
}
