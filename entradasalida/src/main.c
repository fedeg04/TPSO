#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/main.h>

int main(int argc, char* argv[]) {

    t_log* logger_io = iniciar_logger("io.log", "I/O: ");
    t_config* config_io = iniciar_config("io.config");

    puerto_memoria = config_get_string_value(config_io, "PUERTO_MEMORIA");
    ip_memoria = config_get_string_value(config_io, "IP_MEMORIA");

    int memoria_fd = 0, kernel_fd = 0;    
    if(!(memoria_fd = crear_conexion(logger_io, "memoria", ip_memoria, puerto_memoria))){
        terminar_programa(logger_io, config_io);
        exit(3);
    }

    terminar_programa(logger_io, config_io);
    liberar_conexion(memoria_fd);
    return 0;
}
