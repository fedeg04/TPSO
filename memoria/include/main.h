#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/conexion.h>
#include <../include/seniales.h>

char* puerto_escucha;
int tam_memoria;
int tam_pagina;
char* path_instrucciones;
int retardo_respuesta;
t_list* archivos_procesos;
void* memoria;
t_list* tablas_paginas_memoria;
t_bitarray* bitarray_tabla;


#endif