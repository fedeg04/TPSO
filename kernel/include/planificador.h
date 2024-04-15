#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_
#include <../include/procesos.h>
#include <../include/registros.h>
#include <commons/log.h>
#include <../include/protocolo.h>
#include <../include/serializacion.h>

void planificar_nuevo_proceso(proceso_t* proceso, t_log* logger);
void ejecutar_proceso(proceso_t* proceso, t_log* logger);
void enviar_proceso_a_cpu(proceso_t* proceso);
void enviar_proceso_a_cpu_con_timer(proceso_t* proceso);
void esperar_contexto_de_ejecucion(proceso_t* proceso);
void agregar_pcb(void* stream, int* offset, proceso_t* proceso);
#endif