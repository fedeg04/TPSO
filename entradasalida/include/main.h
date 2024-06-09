#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/protocolo.h>
#include <../include/serializacion.h>
#include <../include/seniales.h>
#include <readline/readline.h>

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
char* nombre;
void conectar_a_kernel(char* nombre);
void atender_pedidos_kernel();
void generica_atender_kernel();
void stdin_atender_kernel();
void stdout_atender_kernel();
void dialfs_atender_kernel();
void fin_de(op_code opcode);
void enviar_pedido_stdin(uint32_t proceso_pid, uint32_t cant_paginas, char* direcciones_bytes, char* leido);
void enviar_pedido_stdout(uint32_t proceso_pid, uint32_t cant_paginas, char* direcciones_bytes);



#endif