#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/conexion.h>
#include <../include/consola.h>
#include <pthread.h>
#include <commons/collections/queue.h>


char* puerto_memoria;
char* ip_memoria;
char* puerto_cpu_dispatch;
char* ip_cpu;
char* puerto_cpu_interrupt;
char* puerto_escucha;
int quantum;
char** recursos;
int* instancias_recursos;
int cantidad_recursos;
char* algoritmo_planificacion;
int grado_multiprogramacion;
pthread_t hilo_consola;
t_list* pcbs_new;
t_list* pcbs_ready;
t_list* pcbs_exec;
t_list* pcbs_generica;
t_list* pcbs_stdin;
t_list* pcbs_stdout;
t_list* pcbs_dialfs;



#endif