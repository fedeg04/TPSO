#include <../include/main.h>

int main(int argc, char* argv[]) {
    
    t_log* logger_memoria = iniciar_logger("memoria.log", "MEMORIA: ");
    t_config* config_memoria = iniciar_config("memoria.config");

    puerto_escucha = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");

    int memoria_fd = iniciar_servidor(logger_memoria, puerto_escucha);

    while(server_escuchar(memoria_fd, logger_memoria));

    terminar_programa(logger_memoria, config_memoria);

    return 0;
}

