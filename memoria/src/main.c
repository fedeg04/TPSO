#include <../include/main.h>

void get_config(t_config* config) {
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    tam_pagina = (uint32_t)config_get_int_value(config, "TAM_PAGINA");
    path_instrucciones = config_get_string_value(config, "PATH_INSTRUCCIONES");
    retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");

}

int main(int argc, char* argv[]) {

    t_log* logger_memoria = iniciar_logger("memoria.log", "MEMORIA: ");
    t_config* config_memoria = iniciar_config("memoria.config");
    get_config(config_memoria);

    archivos_procesos = list_create();
    //PAGINAS
    memoria = malloc(tam_memoria);
    tablas_paginas_memoria = list_create();
    int cant_marcos = cantidad_marcos();
    char* bitarray = malloc((cant_marcos+7)/8);
    memset(bitarray, 0, sizeof(bitarray));
    bitarray_tabla = bitarray_create(bitarray,(cant_marcos+7)/8);

    controlar_seniales(logger_memoria);

    //Empieza el servidor
    int memoria_fd = iniciar_servidor(logger_memoria, puerto_escucha, "memoria");
    while(server_escuchar(memoria_fd, logger_memoria, (procesar_conexion_func_t)procesar_conexion, "memoria"));

    terminar_programa(logger_memoria, config_memoria);
    return 0;
}