#ifndef CONSOLAH
#define CONSOLAH
#include <../include/protocolo.h>
#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t size;
    void* stream;
} t_buffer;

typedef struct
{
    op_code op_code;
    t_buffer* buffer;
} t_paquete;

void enviar_inicio_proceso(int socket, char* path, t_log* logger);
void agregar_opcode(void* stream, int* offset, op_code op_code);
void agregar_uint32_t(void* stream, int* offset, uint32_t uint32);
void agregar_uint8_t(void* stream, int* offset, uint8_t uint8);
void agregar_string(void* stream, int* offset, char* string);
void enviar_paquete(op_code op_code, int socket, char* arg1, t_log* logger);

#endif