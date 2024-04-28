#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/main.h>

void get_config(t_config* config) {
    tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    path_base_dialfs = config_get_string_value(config, "PATH_BASE_DIALFS");
    block_size = config_get_int_value(config, "BLOCK_SIZE");
    block_count = config_get_int_value(config, "BLOCK_COUNT");
}

int main(int argc, char* argv[]) {
    t_log* logger_io = iniciar_logger("io.log", "I/O: ");
    log_info(logger_io, "%s", argv[1]); // ./bin/entradasalida "unpath"
    t_config* config_io = iniciar_config("io.config");
    get_config(config_io);

// Se conecta al kernel
    kernel_fd = generar_conexion(logger_io, "kernel", ip_kernel, puerto_kernel, config_io);

    //Se conecta a la memoria
    int memoria_fd = generar_conexion(logger_io, "memoria", ip_memoria, puerto_memoria, config_io);

    conectar_a_kernel();
    atender_pedidos_kernel();
    avisar_desconexion_kernel();

    terminar_programa(logger_io, config_io);
    liberar_conexion(memoria_fd);
    liberar_conexion(kernel_fd);
    return 0;
}

void conectar_a_kernel() {
  void* stream = malloc(sizeof(op_code));
  int offset = 0;
  agregar_opcode(stream, &offset, string_to_opcode(tipo_interfaz));
  send(kernel_fd, stream, offset, 0);  
  free(stream);
}

void atender_pedidos_kernel() {

   if(!strcmp("GENERICA", tipo_interfaz)) {
    generica_atender_kernel();
   } 
   if(!strcmp("STDIN", tipo_interfaz)) {
    stdin_atender_kernel();
   } 
   if(!strcmp("STDOUT", tipo_interfaz)) {
    stdout_atender_kernel();
   } 
   if(!strcmp("DIALFS", tipo_interfaz)) {
    dialfs_atender_kernel();
   } 
}

void generica_atender_kernel() {
    while(1) {
        op_code opcode;
        recv(kernel_fd, &opcode, sizeof(op_code), 0);
        switch(opcode) {
            case IO_GEN_SLEEP:
            uint32_t unis_de_trabajo;
            recv(kernel_fd, &unis_de_trabajo, sizeof(op_code), 0);
            usleep(tiempo_unidad_trabajo * (int) unis_de_trabajo);
            fin_de_sleep();
                break;
            default:
        }
    }
}

void stdin_atender_kernel() {
    
}

void stdout_atender_kernel() {
    
}

void dialfs_atender_kernel() {
    
}

void fin_de_sleep() {
    void* stream = malloc(sizeof(op_code));
    int offset = 0;
    agregar_opcode(stream, &offset, FIN_DE_SLEEP);
    send(kernel_fd, stream, offset, 0);
    free(stream);
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
    void* stream = sizeof(op_code);
    int offset = 0;
    agregar_opcode(stream, &offset, code_interfaz);
    send(kernel_fd, stream, offset, 0);
    free(stream);   
}