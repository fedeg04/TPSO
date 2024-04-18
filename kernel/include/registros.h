#ifndef REGISTROS_H_
#define REGISTROS_H_
#include <commons/collections/list.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <../include/procesos.h>

extern char* puerto_memoria;
extern char* ip_memoria;
extern char* puerto_cpu_dispatch;
extern char* ip_cpu;
extern char* puerto_cpu_interrupt;
extern char* puerto_escucha;
extern pthread_t hilo_consola;
extern int quantum;
extern char** recursos;
extern int* instancias_recursos;
extern int cantidad_recursos;
extern char* algoritmo_planificacion;
extern int grado_multiprogramacion;
extern t_list* pcbs_new;
extern t_list* pcbs_ready;
extern t_list* pcbs_exec;
extern t_list* pcbs_generica;
extern t_list* pcbs_stdin;
extern t_list* pcbs_stdout;
extern t_list* pcbs_dialfs;
extern int procesos_activos;
extern uint32_t pid_siguiente;
extern int cpu_dispatch_fd;
extern void enviar_proceso_io_gen_sleep(proceso_t* proceso,char* interfaz_sleep, uint32_t uni_de_trabajo);
void inicializar_listas();
void get_config(t_config* config); 
int cantidadDeRecursos(char** instancias_string);

#endif