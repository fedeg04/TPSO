#include <../include/conexion.h>

void procesar_conexion(void* args_void) {
    conexion_args_t* args = (conexion_args_t*) args_void;
    int socket_cliente = args->socket_cliente;
    t_log* logger = args->logger;
    free(args);

    op_code opcode;
    while (socket_cliente != 1) {
        if ((recv(socket_cliente, &opcode, sizeof(op_code), MSG_WAITALL)) != sizeof(op_code)){
            log_info(logger, "Tiro error");
            return;
        }

        switch(opcode) {

            case INICIAR_PROCESO:
                uint32_t size;
                recv(socket_cliente, &size, sizeof(uint32_t), 0);
                char* path = malloc(size);
                recv(socket_cliente, path, size, 0);
                log_info(logger, "PATH: %s", path);
                uint32_t pid;
                recv(socket_cliente, &pid, sizeof(uint32_t), 0);
                log_info(logger, "PID RECIBIDO: %d", pid);
                if(existe_archivo(path)) {
                    agregar_proceso(archivos_procesos, path, pid);
                    int cant_marcos = cantidad_marcos();
                    tabla_t* tabla_paginas = iniciar_tabla(pid, cant_marcos);
                    log_info(logger, "PID: <%d> - Tamaño: <%d>", pid, cant_marcos);
                    list_add(tablas_paginas_memoria, tabla_paginas);
                } else {
                    pid = 0;
                }
                enviar_pid(socket_cliente, pid);
                break;
            case FINALIZAR_PROCESO:
                uint32_t pid_a_finalizar; 
                recv(socket_cliente, &pid_a_finalizar, sizeof(uint32_t), 0);
                log_info(logger, "PID: <%d> - Tamaño: <%d>", pid_a_finalizar, cantidad_paginas_proceso(pid_a_finalizar));
                reducir_tamanio_proceso(pid_a_finalizar, tamanio_proceso(pid_a_finalizar), logger);
                eliminar_tabla(pid_a_finalizar);
                eliminar_archivo_proceso(archivos_procesos, pid_a_finalizar);
                break;
            case FETCH:
                uint32_t pid_a_buscar; 
                recv(socket_cliente, &pid_a_buscar, sizeof(uint32_t), 0);
                uint32_t pc;
                recv(socket_cliente, &pc, sizeof(uint32_t), 0);
                char* instruccion = buscar_instruccion(pid_a_buscar, pc, archivos_procesos);
                log_info(logger, "Instruccion: %s", instruccion);
                log_info(logger, "IRetardo: %d", retardo_respuesta);
                usleep(retardo_respuesta * 1000);
                enviar_instruccion(socket_cliente, instruccion);
                free(instruccion);
                break;
            case MOV_OUT:
            break;
            case RESIZE:
                uint32_t tamanio;
                recv(socket_cliente, &tamanio, sizeof(uint32_t), 0);
                uint32_t pid_resize;
                recv(socket_cliente, &pid_resize, sizeof(uint32_t), 0);
                int tam_proceso = tamanio_proceso(pid_resize);
                int response = 0;
                if(tamanio >= tam_proceso) {
                    log_info(logger, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>", pid_resize, tamanio_proceso(pid_resize),tamanio);
                    response = ampliar_tamanio_proceso(pid_resize, tamanio-tam_proceso);
                } else {
                    log_info(logger, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d>", pid_resize, tamanio_proceso(pid_resize),tamanio);
                    reducir_tamanio_proceso(pid_resize, tam_proceso - tamanio, logger);
                }
                if(response) {
                    enviar_out_of_memory(socket_cliente);
                } else {
                    enviar_resize_ok(socket_cliente);
                }
                break;
            default:
                break;
        }
    }
    
    return;
}

void enviar_pid(int socket_cliente, uint32_t pid) {
    void* stream = malloc(sizeof(uint32_t));
    int offset = 0;
    agregar_uint32_t(stream, &offset, pid);
    send(socket_cliente, stream, sizeof(uint32_t), 0);
    free(stream);
}

void enviar_instruccion(int socket, char* instruccion) {
    void* stream = malloc(sizeof(uint32_t) + string_length(instruccion) + 1);
    int offset = 0;
    agregar_string(stream, &offset, instruccion);
    send(socket, stream, offset, 0);
    free(stream);
}

void enviar_out_of_memory(int socket_cliente) {
    void* stream = malloc(sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, OUTOFMEMORY);
    send(socket_cliente, stream, offset, 0);
    free(stream);
}


void enviar_resize_ok(int socket_cliente) {
    void* stream = malloc(sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, MSG);
    send(socket_cliente, stream, offset, 0);
    free(stream);
}


