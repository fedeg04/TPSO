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
    logger_io = iniciar_logger("io.log", "I/O: ");
    log_info(logger_io, "%s", argv[1]); // ./bin/entradasalida "unpath"
    t_config* config_io = iniciar_config("io.config");
    get_config(config_io);

    controlar_seniales(logger_io);
// Se conecta al kernel
    kernel_fd = generar_conexion(logger_io, "kernel", ip_kernel, puerto_kernel, config_io);

    //Se conecta a la memoria
    memoria_fd = generar_conexion(logger_io, "memoria", ip_memoria, puerto_memoria, config_io);

    conectar_a_kernel();
    atender_pedidos_kernel();

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
            recv(kernel_fd, &unis_de_trabajo, sizeof(uint32_t), 0);
            log_info(logger_io, "TIEMPO UNI: %d", tiempo_unidad_trabajo);
            log_info(logger_io, "UNIS: %d", unis_de_trabajo);
            log_info(logger_io, "TOTAL: %d", unis_de_trabajo * tiempo_unidad_trabajo);
            usleep(tiempo_unidad_trabajo * unis_de_trabajo * 1000);
            log_info(logger_io, "LLegue sleep");
            fin_de_sleep();
                break;
            default:
        }
    }
}

void stdin_atender_kernel() {
    while(1) {
        op_code opcode;
        recv(kernel_fd, &opcode, sizeof(op_code), 0);
        switch(opcode) {
            case IO_STDIN_READ:
            uint32_t proceso_pid;
            uint32_t registro_direccion;
            uint32_t registro_tamanio;
            recv(kernel_fd, &proceso_pid, sizeof(uint32_t), 0);
            recv(kernel_fd, &registro_direccion, sizeof(uint32_t), 0);
            recv(kernel_fd, &registro_tamanio, sizeof(uint32_t), 0);
            enviar_pedido_stdinout(IO_STDIN_READ ,proceso_pid, registro_direccion, registro_tamanio);
            op_code op_code_recep;
            recv(memoria_fd, &op_code_recep, sizeof(op_code), 0);
            fin_de(FIN_DE_STDIN);
            break;
            default:
        }
    }
}

void stdout_atender_kernel() {
  while(1) {
        op_code opcode;
        recv(kernel_fd, &opcode, sizeof(op_code), 0);
        switch(opcode) {
            case IO_STDOUT_WRITE:
            uint32_t proceso_pid;
            uint32_t registro_direccion;
            uint32_t registro_tamanio;
            recv(kernel_fd, &proceso_pid, sizeof(uint32_t), 0);
            recv(kernel_fd, &registro_direccion, sizeof(uint32_t), 0);
            recv(kernel_fd, &registro_tamanio, sizeof(uint32_t), 0);
            enviar_pedido_stdinout(IO_STDOUT_WRITE ,proceso_pid, registro_direccion, registro_tamanio);
            op_code op_code_recep;
            recv(memoria_fd, &op_code_recep, sizeof(op_code), 0);
            fin_de(FIN_DE_STDOUT);
            break;
            default:
        }
    }  
}

void dialfs_atender_kernel() {
    
}

void fin_de(op_code opcode) {
    void* stream = malloc(sizeof(op_code));
    int offset = 0;
    agregar_opcode(stream, &offset, opcode);
    send(kernel_fd, stream, offset, 0);
    log_info(logger_io, "STREAM: %d", *((int*) stream));
    free(stream);
}

void enviar_pedido_stdinout(op_code opcode , uint32_t proceso_pid, uint32_t registro_direccion, uint32_t registro_tamanio) {
    void* stream = malloc(sizeof(op_code) + 3 * sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, opcode);
    agregar_uint32_t(stream, &offset, proceso_pid);
    agregar_uint32_t(stream, &offset, registro_direccion);
    agregar_uint32_t(stream, &offset, registro_tamanio);
    send(memoria_fd, stream, offset, 0);
}
