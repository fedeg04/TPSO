#include <../include/conexion.h>

void procesar_conexion_interrupt(void* args_void) {
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
            case INTERRUMPIR:
            default:
        }
    }
    return;
}

void procesar_conexion_dispatch(void* args_void) {
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
            case ENVIAR_PCB:
            case SET:
            case MOV_IN:
            case MOV_OUT:
            case SUM:
            case SUB:
            case JNZ:
            case RESIZE:
            case COPY_STRING:
            case WAIT:
            case SIGNAL:
            case IO_GEN_SLEEP:
            case IO_STDIN_READ:
            case IO_STDOUT_WRITE:
            case IO_FS_CREATE:
            case IO_FS_DELETE:
            case IO_FS_TRUNCATE:
            case IO_FS_WRITE:
            case IO_FS_READ:
            case EXIT:
            default:
        }
    }
    return;
}