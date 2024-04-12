#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/main.h>
#include <../include/consola.h>

void get_config(t_config* config) {
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    quantum = config_get_int_value(config, "QUANTUM");
    grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    recursos = config_get_array_value(config, "RECURSOS");
    char** instancias_string = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    cantidad_recursos = cantidadDeRecursos(instancias_string);
    instancias_recursos = malloc(cantidad_recursos * sizeof(int));
    for(int i = 0; i < cantidad_recursos; i++) 
    { 
        instancias_recursos[i] = atoi(instancias_string[i]);
    }
}

    int cantidadDeRecursos(char** instancias_string)
    { 
        int cant = 0;
        while(instancias_string[cant] != NULL)
        { 
            cant++;
        }
        return cant;
    }

int main(int argc, char* argv[]) {
    t_log* logger_kernel = iniciar_logger("kernel.log", "KERNEL: ");
    t_config* config_kernel = iniciar_config("kernel.config");
    get_config(config_kernel);
    t_queue* pcbs_new = queue_create();
    t_queue* pcbs_ready = queue_create();

    //Se conecta como cliente a la memoria (interrupt)
    int memoria_interrupt_fd = generar_conexion(logger_kernel, "memoria", ip_memoria, puerto_memoria, config_kernel);
    
    //Se conecta como cliente a la memoria (dispatch)
    int memoria_dispatch_fd = generar_conexion(logger_kernel, "memoria", ip_memoria, puerto_memoria, config_kernel);

    empezar_hilo_consola(&hilo_consola, logger_kernel, memoria_dispatch_fd);

    //Se conecta como cliente al CPU dispatch
    int cpu_dispatch_fd = generar_conexion(logger_kernel, "CPU dispatch", ip_cpu, puerto_cpu_dispatch, config_kernel);
    
    //Se conecta como cliente al CPU interrupt
    int cpu_interrupt_fd = generar_conexion(logger_kernel, "CPU interrupt", ip_cpu, puerto_cpu_interrupt, config_kernel);    
    
    //Empieza el servidor
    int kernel_fd = iniciar_servidor(logger_kernel, puerto_escucha, "kernel");
    while(server_escuchar(kernel_fd, logger_kernel, (procesar_conexion_func_t)procesar_conexion, "kernel"));

    pthread_join(hilo_consola, NULL);
    terminar_programa(logger_kernel, config_kernel);
    liberar_conexion(memoria_dispatch_fd);
    liberar_conexion(memoria_interrupt_fd);
    liberar_conexion(cpu_dispatch_fd);
    liberar_conexion(cpu_interrupt_fd);
    return 0;
}
