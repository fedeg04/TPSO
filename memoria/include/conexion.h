#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>
#include <../include/archivo_proceso.h>
#include <../include/conversores.h>
#include <../include/paginas.h>
#include <commons/bitarray.h>

void procesar_conexion(void* args);
void enviar_pid(int socket_cliente, uint32_t pid);
void enviar_instruccion(int socket, char* instruccion);
extern int cantidad_marcos();
int tamanio_proceso(uint32_t pid);
tabla_t* tabla_paginas_por_pid(uint32_t pid);
bool tabla_paginas_por_pid_actual(tabla_t* tabla);
bool ampliar_tamanio_proceso(uint32_t pid, int tamanio);
void reducir_tamanio_proceso(uint32_t pid, int tamanio);
int completar_ultima_pagina(tabla_t* tabla, int tamanio);
bool tiene_validez(pagina_t* pagina);
int vaciar_ultima_pagina(tabla_t* tabla, int tamanio);
extern t_list* archivos_procesos;
extern void* memoria;
extern t_list* tablas_paginas_memoria;
extern int tam_memoria;
extern int tam_pagina;
extern t_bitarray* bitarray_tabla;


#endif