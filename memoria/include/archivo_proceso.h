#ifndef PROCESOS_H_
#define PROCESOS_H_

#include <stdio.h>
#include <stdint.h>
#include <commons/collections/list.h>

typedef struct {
    FILE* f;
    uint32_t pid;
} archivo_proceso_t;

bool existe_archivo(char* path);
void agregar_proceso(archivo_proceso_t* archivo_proceso, t_list* archivos_procesos);

#endif