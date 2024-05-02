#ifndef REGISTROS_H_
#define REGISTROS_H_
#include <commons/collections/list.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <../include/procesos.h>
#include <commons/log.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <semaphore.h>

typedef struct {
    proceso_t* proceso;
    t_log* logger;
} nuevo_proceso_t;

extern t_log* logger_kernel;
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
extern t_list* pcbs_ready_prioritarios;
extern t_list* pcbs_exec;
extern t_queue* pcbs_exit;
extern t_list* pcbs_generica;
extern t_list* pcbs_stdin;
extern t_list* pcbs_stdout;
extern t_list* pcbs_dialfs;
extern t_list** pcbs_recursos;
extern pthread_mutex_t* mutex_new_list;
extern pthread_mutex_t* mutex_ready_list;
extern pthread_mutex_t* mutex_ready_prioritario_list;
extern pthread_mutex_t* mutex_exec_list;
extern pthread_mutex_t* mutex_exit_queue;
extern pthread_mutex_t* mutex_generica_list;
extern pthread_mutex_t* mutex_generica_exec;
extern pthread_mutex_t* mutex_stdin_list;
extern pthread_mutex_t* mutex_stdout_list;
extern pthread_mutex_t* mutex_dialfs_list;
extern pthread_mutex_t** mutex_recursos_list;
extern sem_t* multiprogramacion;
extern sem_t* pcb_esperando_ready;
extern sem_t* pcb_esperando_exec;
extern sem_t* pcb_esperando_exit;
extern sem_t* pcb_esperando_generica;
extern sem_t* pcb_esperando_stdin;
extern sem_t* pcb_esperando_stdout;
extern sem_t* pcb_esperando_dialfs;
extern sem_t** pcb_esperando_recurso;
extern int procesos_activos;
extern uint32_t pid_siguiente;
extern int cpu_dispatch_fd;
extern int cpu_interrupt_fd;
extern int memoria_interrupt_fd;
extern t_dictionary* diccionario_interfaces;
extern void enviar_proceso_io_gen_sleep(proceso_t* proceso,char* interfaz_sleep, uint32_t uni_de_trabajo);
void inicializar_listas();
void liberar_listas();
void inicializar_semaforos();
void liberar_semaforos();
void get_config(t_config* config); 
int cantidadDeRecursos(char** instancias_string);
extern void finalizar_proceso(proceso_t* proceso);
extern void recibir_fin_de_sleep();
extern void ejecutar_proceso(proceso_t* proceso, t_log* logger);

#endif