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
    int kernel_fd = generar_conexion(logger_io, "kernel", ip_kernel, puerto_kernel, config_io);
    char* msg = "E/S en Kernel";
    void* stream = malloc(sizeof(uint32_t) + strlen(msg) + 1);
    uint32_t offset = 0;
    agregar_opcode(stream, &offset, MSG);
    agregar_string(stream, &offset, msg);
    send(kernel_fd, stream, offset, 0);
    //Se conecta a la memoria
    int memoria_fd = generar_conexion(logger_io, "memoria", ip_memoria, puerto_memoria, config_io);
    char* msgMem = "E/S en Memoria";
    void* streamMem = malloc(sizeof(uint32_t) + strlen(msg) + 1);
    uint32_t offsetMem = 0;
    agregar_opcode(streamMem, &offsetMem, MSG);
    agregar_string(streamMem, &offsetMem, msgMem);
    send(memoria_fd, streamMem, offsetMem, 0);

    

    terminar_programa(logger_io, config_io);
    liberar_conexion(memoria_fd);
    liberar_conexion(kernel_fd);
    return 0;
}
