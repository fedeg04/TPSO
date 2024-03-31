#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/main.h>

void get_config(t_config* config) {
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERUPT");
}

int main(int argc, char* argv[]) {
    t_log* logger_kernel = iniciar_logger("kernel.log", "KERNEL: ");
    t_config* config_kernel = iniciar_config("kernel.config");
    get_config(config_kernel);
    
    //Se conecta como cliente a la memoria
    int memoria_fd = generar_conexion(logger_kernel, "memoria", ip_memoria, puerto_memoria, config_kernel);

    //Se conecta como cliente al CPU dispatch
    int cpu_dispatch_fd = generar_conexion(logger_kernel, "CPU dispatch", ip_cpu, puerto_cpu_dispatch, config_kernel);
    
    //Se conecta como cliente al CPU interrupt
    int cpu_interrupt_fd = generar_conexion(logger_kernel, "CPU interrupt", ip_cpu, puerto_cpu_interrupt, config_kernel);
    
    terminar_programa(logger_kernel, config_kernel);
    liberar_conexion(memoria_fd);

    return 0;
}
