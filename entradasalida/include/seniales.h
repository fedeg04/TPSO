#ifndef SENIALES_H
#define SENIALES_H

#include <signal.h>
#include <../include/protocolo.h>
#include <commons/log.h>

extern char* tipo_interfaz;
extern int kernel_fd;
void controlar_seniales(t_log* logger);
void cerrar_seniales();
void interfaz_desconectarse(op_code code_interfaz);
void avisar_desconexion_kernel();

#endif