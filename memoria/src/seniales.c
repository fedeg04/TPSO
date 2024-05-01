#include "../include/seniales.h"

t_log* logger_sen;

void controlar_seniales(t_log* logger) {
    logger_sen = logger;
    signal(SIGINT, cerrar_seniales);
}

void cerrar_seniales() {
    log_info(logger_sen, "Llegue aca");
    list_destroy_and_destroy_elements(archivos_procesos, (void*) archivo_proceso_destroy);
    exit(0);
}