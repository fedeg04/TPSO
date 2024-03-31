#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/main.h>

void get_config(t_config* config) {
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
}

int main(int argc, char* argv[]) {

    t_log* logger_io = iniciar_logger("io.log", "I/O: ");
    t_config* config_io = iniciar_config("io.config");

    get_config(config_io);

    int memoria_fd = generar_conexion(logger_io, "memoria", ip_memoria, puerto_memoria, config_io);

    terminar_programa(logger_io, config_io);
    liberar_conexion(memoria_fd);
    return 0;
}
