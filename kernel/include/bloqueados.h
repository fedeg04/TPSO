#ifndef BLOQUEADOS_H_
#define BLOQUEADOS_H_

#include <../include/registros.h>
#include <../include/protocolo.h>
#include <../include/serializacion.h>
typedef struct {
    proceso_t* proceso;
    uint32_t uni_de_trabajo;
} proceso_sleep_t;

void enviar_proceso_io_gen_sleep(proceso_t* proceso,char* interfaz_sleep, uint32_t uni_de_trabajo);
int estaConectada(char* interfaz);
void hacer_io_gen_sleep();
void recibir_fin_de_sleep();
void desbloquear_proceso(proceso_t* proceso);

#endif