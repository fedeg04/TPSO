#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/conexion.h>
#include <../include/serializacion.h>

int cpu_dispatch_fd;
int cpu_interrupt_fd;
t_log* logger_cpu;
int memoria_fd;
char* puerto_memoria;
char* ip_memoria;
char* puerto_escucha_dispatch;
char* puerto_escucha_interrupt;
int cantidad_entradas_tlb;
char* algoritmo_tlb;
int servers_escuchar();
registros_t* registros_cpu;
proceso_t* pcb;

#endif