#include <../include/paginas.h>

tabla_t* iniciar_tabla(uint32_t pid, int cantidad_paginas) {
    tabla_t* tabla = malloc(sizeof(tabla_t));
    tabla->paginas = list_create();
    tabla->pid = pid;
    tabla->ultimo_marco_modificado = -1;
    for (int i = 0; i < cantidad_paginas; i++)
    {
        pagina_t* pagina = malloc(sizeof(pagina));
        pagina->marco = i;
        pagina->bytes_ocupados = 0;
        list_add(tabla->paginas, pagina);
    }
    return tabla;
}