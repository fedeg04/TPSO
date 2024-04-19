#include <../include/archivo_proceso.h>
#include <stdbool.h>
#include <stddef.h>

uint32_t pid_a_buscar;

bool existe_archivo(char* path) {
    return fopen(path, "r") != NULL;
}

void agregar_proceso(t_list* archivos_procesos, char* path, uint32_t pid) {
    archivo_proceso_t* archivo_proceso = malloc(sizeof(archivo_proceso_t));
    archivo_proceso->path = path;
    archivo_proceso->pid = pid;
    list_add(archivos_procesos, archivo_proceso);
}


bool buscar_por_pid(archivo_proceso_t *archivo_proceso) {
    return (archivo_proceso->pid == pid_a_buscar);
}

char* buscar_instruccion(uint32_t pid, uint32_t pc, t_list* archivos_procesos) {
    pid_a_buscar = pid;

    archivo_proceso_t* archivo_proceso_encontrado = malloc(sizeof(archivo_proceso_t));

    archivo_proceso_encontrado = list_find(archivos_procesos, (void*)buscar_por_pid);

    FILE* f = fopen(archivo_proceso_encontrado->path, "r");

    fseek(f, 0, SEEK_SET);
    char* instruccion = buscar_instruccion_en(f, pc);
    fclose(f);
    
    return instruccion;
}

char* buscar_instruccion_en(FILE* f, uint32_t pc) {
    char* linea;
    size_t longitud = 0;
    ssize_t leidos;
    for (int i = 0; i <= pc; i++) {
        leidos = getline(&linea, &longitud, f);
        if (leidos == -1) { // Se alcanzó el final del archivo antes de llegar a la línea 'pc'
            return NULL;
        }
    }

    // Cerrar el archivo y devolver la línea leída
    //fclose(f);
    return linea;
}