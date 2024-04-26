#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_
#include <../include/procesos.h>
#include <../include/registros.h>
#include <commons/log.h>
#include <../include/protocolo.h>
#include <../include/serializacion.h>
#include <../include/conversores.h>
#include <commons/temporal.h>

typedef struct {
    proceso_t* proceso;
    t_temporal* timer;
    t_log* logger;
} interrupcion_proceso_t;

void planificar_nuevo_proceso(void* void_args);
void ejecutar_proceso(proceso_t* proceso, t_log* logger);
void enviar_proceso_a_cpu(proceso_t* proceso, t_log* logger);
void enviar_proceso_a_cpu_con_timer(proceso_t* proceso, t_temporal* timer);
void esperar_contexto_de_ejecucion(proceso_t* proceso, t_log* logger);
void agregar_pcb(void* stream, int* offset, proceso_t* proceso);
void esperar_llegada_de_proceso_fifo(proceso_t* proceso, t_log* logger);
void esperar_llegada_de_proceso_rr_vrr(proceso_t* proceso, t_temporal* timer, t_log* logger);
void liberar_recursos_proceso(proceso_t* proceso, t_log* logger);
void elegir_proceso_a_exec(t_log* logger);
void finalizar_proceso(proceso_t* proceso);
void manejar_interrupcion_de_timer(void* args);
#endif