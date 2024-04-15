#ifndef REGISTROS_H_
#define REGISTROS_H_
#include <commons/collections/list.h>
#include <commons/config.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

char* puerto_memoria;
char* ip_memoria;
char* puerto_cpu_dispatch;
char* ip_cpu;
char* puerto_cpu_interrupt;
char* puerto_escucha;
pthread_t hilo_consola;
int quantum;
char** recursos;
int* instancias_recursos;
int cantidad_recursos;
char* algoritmo_planificacion;
int grado_multiprogramacion;
t_list* pcbs_new;
t_list* pcbs_ready;
t_list* pcbs_exec;
t_list* pcbs_generica;
t_list* pcbs_stdin;
t_list* pcbs_stdout;
t_list* pcbs_dialfs;
void inicializar_listas();
void get_config(t_config* config); 
int cantidadDeRecursos(char** instancias_string);

#endif