#ifndef CONSOLA_H_
#define CONSOLA_H_
#include <../include/protocolo.h>

typedef struct
{
    int size;
    void *stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer *buffer;
} t_paquete;

void enviar_paquete(op_code op_code, int socket, char* arg1);

#endif