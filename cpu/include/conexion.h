#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>
#include <../include/registros.h>
#include <../include/conversores.h>
#include <../include/frees.h>
#include <../include/numeros.h>

void procesar_conexion_interrupt(void* args);
void procesar_conexion_dispatch(void* args);
void recibir_pcb(int socket, proceso_t* pcb, t_log* logger);
extern int memoria_fd;
extern int tamanio_pagina;
extern registros_t* registros_cpu;
char* recibir_instruccion(int socket);
uint32_t recibir_interrupcion(int socket);
int ejecutar_instruccion(char** parametros, char* instruccion, t_log* logger, proceso_t* pcb, int socket);
void set_registros(char* registro, uint32_t valor);
uint32_t get_valor_registro(char* registro);
void enviar_pid_pc(uint32_t pid, uint32_t pc, int socket);
bool hay_interrupcion(uint32_t pid);
void pedir_marcos(uint32_t pid, uint8_t cant_paginas_enviar, uint32_t nro_pagina);
void enviar_mov_out(uint32_t valor, uint8_t cant_pags_a_enviar, uint32_t pid, uint32_t cant_bytes, uint16_t desplazamiento);
char* generar_envio_direcciones_tamanios(uint8_t cant_pags, uint32_t tamanio, uint16_t desplazamiento);
uint32_t cant_bytes(char* registro);
bool respuesta_memoria(proceso_t* pcb, int socket);
uint32_t enviar_mov_in(uint8_t cant_pags, uint32_t pid, uint32_t cant_bytes, uint16_t desplazamiento);
uint32_t recibir_mov_in(uint32_t cantidad_bytes);

#endif