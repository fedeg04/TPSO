#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <../include/init.h>
#include <../include/protocolo.h>
#include <../include/serializacion.h>
#include <../include/seniales.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <readline/readline.h>
#include <dirent.h>

t_log* logger_io;
char* tipo_interfaz;
int tiempo_unidad_trabajo;
char* puerto_memoria;
char* puerto_kernel;
char* ip_memoria;
char* ip_kernel;
char* path_base_dialfs;
int block_size;
int block_count;
int kernel_fd;
int memoria_fd;
int retraso_compactacion;
char* nombre;
FILE* f_bloques;
FILE* f_bitmap;
t_bitarray* bitarray;

typedef struct {
    int bloque_inicial;
    int tamanio;
    char* nombre;
} bloques_archivo_t;

void conectar_a_kernel(char* nombre);
void atender_pedidos_kernel();
void generica_atender_kernel();
void stdin_atender_kernel();
void stdout_atender_kernel();
void dialfs_atender_kernel();
void fin_de(op_code opcode);
void enviar_pedido_stdin(uint32_t proceso_pid, uint32_t cant_paginas, char* direcciones_bytes, char* leido);
void enviar_pedido_stdout(uint32_t proceso_pid, uint32_t cant_paginas, char* direcciones_bytes);
void iniciar_fs();
void iniciar_bloques();
void iniciar_bitmap();
void crear_archivo(char* nombre);
int tamanio_archivo(char* nombre);
void cambiar_tamanio_archivo(char* nombre, int tamanio_nuevo, int tamanio, int bloque_inicial, int agrandar);
void path_para_archivo(char** path, char* nombre);
int cant_bloques_archivo(int tamanio);
void cambiar_tamanio_metadata(char* nombre, int tamanio);
void leer_txt_y_agregar_a_lista(t_list* bloques_archivos);
bool puede_agrandar_sin_compactar(char* nombre, int tamanio_nuevo, int tamanio, int bloque_inicial);
int compactar(char* nombre);
void* leer_bloques_dat();
void escribir_bloques_dat(void* bloques);
void bloques_archivo_destroyer(bloques_archivo_t* bloques_archivo);
t_bitarray* setear_bitarray();

#endif