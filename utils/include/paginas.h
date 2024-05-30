#ifndef PAGINAS_H_
#define PAGINAS_H_

#include <stdint.h>
#include <commons/collections/list.h>

typedef struct
{
    uint16_t marco;
    uint8_t bytes_ocupados;
} pagina_t;

typedef struct {
    int ultimo_marco_modificado;
    t_list* paginas;
    uint32_t pid;
} tabla_t;

tabla_t* iniciar_tabla(uint32_t pid, int cantidad_paginas);

#endif