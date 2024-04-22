#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/main.h>
#include <../include/consola.h>



int main(int argc, char* argv[]) {
    t_log* logger_kernel = iniciar_logger("kernel.log", "KERNEL: ");
    t_config* config_kernel = iniciar_config("kernel.config");
    get_config(config_kernel);
    inicializar_listas();
    pid_siguiente = 1;
    procesos_activos = 0;

    //Se conecta como cliente a la memoria (interrupt)
    memoria_interrupt_fd = generar_conexion(logger_kernel, "memoria", ip_memoria, puerto_memoria, config_kernel);
    
    //Se conecta como cliente a la memoria (dispatch)
    int memoria_dispatch_fd = generar_conexion(logger_kernel, "memoria", ip_memoria, puerto_memoria, config_kernel);
    char* msg = "Kernel en Memoria";
    void* stream = malloc(sizeof(uint32_t) + strlen(msg) + 1);
    int offset = 0;
    agregar_opcode(stream, &offset, MSG);
    agregar_string(stream, &offset, msg);
    send(memoria_dispatch_fd, stream, offset, 0);

    empezar_hilo_consola(&hilo_consola, logger_kernel, memoria_dispatch_fd);

    //Se conecta como cliente al CPU dispatch
    cpu_dispatch_fd = generar_conexion(logger_kernel, "CPU dispatch", ip_cpu, puerto_cpu_dispatch, config_kernel);
    char* msgCPU = "Kernel en CPU";
    void* streamCPU = malloc(sizeof(uint32_t) + strlen(msgCPU) + 1);
    int offsetCPU = 0;
    agregar_opcode(streamCPU, &offsetCPU, MSG);
    agregar_string(streamCPU, &offsetCPU, msgCPU);
    send(cpu_dispatch_fd, streamCPU, offsetCPU, 0);
    //Se conecta como cliente al CPU interrupt
    int cpu_interrupt_fd = generar_conexion(logger_kernel, "CPU interrupt", ip_cpu, puerto_cpu_interrupt, config_kernel);    
    
    //Empieza el servidor
    int kernel_fd = iniciar_servidor(logger_kernel, puerto_escucha, "kernel");
    while(server_escuchar(kernel_fd, logger_kernel, (procesar_conexion_func_t)procesar_conexion, "kernel"));

    pthread_join(hilo_consola, NULL);
    liberar_listas();
    terminar_programa(logger_kernel, config_kernel);
    liberar_conexion(memoria_dispatch_fd);
    liberar_conexion(memoria_interrupt_fd);
    liberar_conexion(cpu_dispatch_fd);
    liberar_conexion(cpu_interrupt_fd);
    return 0;
}
