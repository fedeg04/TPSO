#include "../include/seniales.h"

t_log* logger_sen;

void controlar_seniales(t_log* logger) {
    logger_sen = logger;
    log_info(logger_sen, "Llegue aca");
    signal(SIGINT, cerrar_seniales);
}

void cerrar_seniales() {
    log_info(logger_sen, "Llegue aca");
    avisar_desconexion_kernel();
    exit(0);
}

void avisar_desconexion_kernel() {
    if(!strcmp("GENERICA", tipo_interfaz)) {
        interfaz_desconectarse(GENERICA_BYE);
   } 
   if(!strcmp("STDIN", tipo_interfaz)) {
    interfaz_desconectarse(STDIN_BYE);
   } 
   if(!strcmp("STDOUT", tipo_interfaz)) {
    interfaz_desconectarse(STDOUT_BYE);  
   } 
   if(!strcmp("DIALFS", tipo_interfaz)) {
    interfaz_desconectarse(DIALFS_BYE);  
   } 
}

void interfaz_desconectarse(op_code code_interfaz) {
    void* stream = malloc(sizeof(op_code));
    int offset = 0;
    agregar_opcode(stream, &offset, code_interfaz);
    send(kernel_fd, stream, offset, 0);
    free(stream);   
}