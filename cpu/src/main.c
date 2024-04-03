#include <stdlib.h>
#include <stdio.h>
#include <../include/main.h>

void get_config(t_config *config)
{
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
    puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
}

int servers_escuchar()
{
return server_escuchar(cpu_dispatch_fd, logger_cpu, (procesar_conexion_func_t)procesar_conexion, "CPU dispatch") && server_escuchar(cpu_interrupt_fd, logger_cpu, (procesar_conexion_func_t)procesar_conexion, "CPU interrupt");
}

int main(int argc, char *argv[])
{
    logger_cpu = iniciar_logger("cpu.log", "CPU: ");
    t_config *config_cpu = iniciar_config("cpu.config");
    get_config(config_cpu);

    int *flag_dispatch_encendido = malloc(sizeof(int));
    int *flag_interrupt_encendido = malloc(sizeof(int));

    // Empieza el servidor dispatch
    cpu_dispatch_fd = iniciar_servidor(logger_cpu, puerto_escucha_dispatch, "CPU dispatch");
    // empezar_hilo_servidor(cpu_dispatch_fd, logger_cpu, (procesar_conexion_func_t)procesar_conexion, "CPU dispatch", flag_dispatch_encendido);

    // Empieza el servidor interrupt
    cpu_interrupt_fd = iniciar_servidor(logger_cpu, puerto_escucha_interrupt, "CPU interrupt");
    // empezar_hilo_servidor(cpu_interrupt_fd, logger_cpu, (procesar_conexion_func_t)procesar_conexion, "CPU interrupt", flag_interrupt_encendido);
    while (servers_escuchar());

        // Se conecta como cliente a la memoria
    int memoria_fd = generar_conexion(logger_cpu, "memoria", ip_memoria, puerto_memoria, config_cpu);

    // TODO: ver como sincronizazr el comienzo de cada server y del cliente.

    //while (*flag_dispatch_encendido && *flag_interrupt_encendido);
    //free(flag_dispatch_encendido);
    //free(flag_interrupt_encendido);

    liberar_conexion(memoria_fd);
    terminar_programa(logger_cpu, config_cpu);

    return 0;
}
