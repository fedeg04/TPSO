#ifndef CONSOLAH
#define CONSOLAH
#include <../include/protocolo.h>
#include <commons/log.h>
#include <stdint.h>

typedef struct
{
    uint32_t size;
    void* stream;
} t_buffer;

typedef struct
{
    op_code op_code;
    t_buffer* buffer;
} t_paquete;

void enviar_paquete(op_code op_code, int socket, char* arg1, t_log* logger);

#endif