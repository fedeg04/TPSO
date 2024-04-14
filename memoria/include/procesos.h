#ifndef PROCESOS_H_
#define PROCESOS_H_

#include <stdio.h>
#include <stdint.h>

typedef struct {
    FILE* f;
    uint32_t pid;
} archivo_proceso_t;

bool existe_archivo(char* path);

#endif