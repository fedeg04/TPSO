#ifndef BLOQUEADOS_H_
#define BLOQUEADOS_H_

#include <../include/registros.h>
#include <../include/protocolo.h>
#include <../include/serializacion.h>

void enviar_proceso_io_gen_sleep(proceso_t* proceso,char* interfaz_sleep, uint32_t uni_de_trabajo);
int estaConectada(char* interfaz);
void hacer_io_gen_sleep();
void recibir_fin_de_sleep();
void desbloquear_proceso(proceso_t* proceso);
void enviar_proceso_a_wait(proceso_t* proceso, char* recurso_wait, uint32_t tiempo_en_cpu, t_temporal* timer);
int posicion_de_recurso(char* recurso);
t_list* lista_de_recurso(char* recurso);
bool hay_recursos_de(char* recurso);
void pedir_recurso(char* recurso_wait);
bool existe_recurso(char* recurso);
void enviar_proceso_a_signal(proceso_t* proceso, char* recurso_signal);
void devolver_recurso(char* recurso_signal);
void enviar_proceso_a_interfaz(proceso_a_interfaz_t* proceso_a_interfaz, char* interfaz, void (*hacer_peticion)(proceso_a_interfaz_t*));
void hacer_io_stdin_read(proceso_a_interfaz_t* proceso_interfaz);
void hacer_io_stdout_write(proceso_a_interfaz_t* proceso_interfaz);
#endif