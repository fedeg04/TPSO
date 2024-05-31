#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/protocolo.h>
#include <../include/serializacion.h>
#include <../include/seniales.h>

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
void conectar_a_kernel();
void atender_pedidos_kernel();
void generica_atender_kernel();
void stdin_atender_kernel();
void stdout_atender_kernel();
void dialfs_atender_kernel();
void fin_de(op_code opcode);
void enviar_pedido_stdinout(op_code opcode , uint32_t proceso_pid, uint32_t registro_direccion, uint32_t registro_tamanio)



#endif