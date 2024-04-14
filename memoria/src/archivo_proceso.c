#include <../include/archivo_proceso.h>
#include <stdbool.h>
#include <stddef.h>

bool existe_archivo(char* path) {
    return fopen(path, "r") != NULL;
}

void agregar_proceso(archivo_proceso_t* archivo_proceso, t_list* archivos_procesos) {
    list_add(archivos_procesos, archivo_proceso);
}