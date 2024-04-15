#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_
#include <../include/procesos.h>
#include <../include/registros.h>
#include <commons/log.h>

int procesos_activos = 0;
void planificar_nuevo_proceso(proceso_t* proceso, t_log* logger);
void ejecutar_proceso(proceso_t* proceso);
#endif