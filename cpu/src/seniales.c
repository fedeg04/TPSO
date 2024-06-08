#include "../include/seniales.h"

t_log* logger;

void controlar_seniales(t_log* logger_cpu) {
    logger = logger_cpu;
    signal(SIGINT, cerrar_seniales);
}

void cerrar_seniales() {
    log_info(logger, "aaaaaa");
    eliminar_tlb();
    exit(0);
}