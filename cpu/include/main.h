#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/conexion.h>
#include <../include/serializacion.h>

char* cpu_dispatch_fd;
char* cpu_interrupt_fd;
char* logger_cpu;
char* puerto_memoria;
char* ip_memoria;
char* puerto_escucha_dispatch;
char* puerto_escucha_interrupt;
int cantidad_entradas_tlb;
char* algoritmo_tlb;
int servers_escuchar();
registros_t* registros_cpu;

#endif