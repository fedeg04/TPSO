#include <../include/main.h>

void get_config(t_config* config) {
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
}

int main(int argc, char* argv[]) {
    
    t_log* logger_memoria = iniciar_logger("memoria.log", "MEMORIA: ");
    t_config* config_memoria = iniciar_config("memoria.config");
    get_config(config_memoria);

    //Empieza el servidor
    int memoria_fd = iniciar_servidor(logger_memoria, puerto_escucha, "memoria");
    while(server_escuchar(memoria_fd, logger_memoria, (procesar_conexion_func_t)procesar_conexion, "memoria"));

    terminar_programa(logger_memoria, config_memoria);

    return 0;
}

