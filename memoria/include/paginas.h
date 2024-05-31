#ifndef PAGINAS_H_
#define PAGINAS_H_

#include <stdint.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/log.h>

typedef struct
{
    int nro_pagina;
    uint16_t marco;
    uint8_t bytes_ocupados;
} pagina_t;

typedef struct {
    int cantidad_paginas;
    t_list* paginas;
    uint32_t pid;
} tabla_t;

tabla_t* iniciar_tabla(uint32_t pid, int cantidad_paginas);
void pagina_destroy(pagina_t* pagina);
int cantidad_marcos();
int tamanio_proceso(uint32_t pid);
tabla_t* tabla_paginas_por_pid(uint32_t pid);
bool tabla_paginas_por_pid_actual(tabla_t* tabla);
bool ampliar_tamanio_proceso(uint32_t pid, int tamanio);
void reducir_tamanio_proceso(uint32_t pid, int tamanio, t_log* logger);	
int completar_ultima_pagina(tabla_t* tabla, int tamanio);
bool tiene_validez(pagina_t* pagina);
int vaciar_ultima_pagina(tabla_t* tabla, int tamanio);
void eliminar_tabla(uint32_t pid);
int cantidad_paginas_proceso(uint32_t pid_a_finalizar);
pagina_t *buscar_pagina_por_nro(tabla_t *tabla, int nro_pagina);
bool pagina_por_nro(pagina_t *pagina);
extern t_list* tablas_paginas_memoria;
extern int tam_memoria;
extern int tam_pagina;
extern t_bitarray* bitarray_tabla;

#endif