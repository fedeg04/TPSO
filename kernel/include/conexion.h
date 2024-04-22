#ifndef CONEXION_H_
#define CONEXION_H_

#include <../include/sockets.h>
#include <../include/protocolo.h>

void procesar_conexion(void* args);
void conectar_generica();
void conectar_stdin();
void conectar_stdout();
void conectar_dialfs();

#endif