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
                log_info(logger, "Me llego iniciar proceso");
                uint32_t size;
                recv(socket_cliente, &size, sizeof(uint32_t), 0);
                void* path = malloc(size);
                recv(socket_cliente, path, size, 0);
                uint32_t pid;
                recv(socket_cliente, &pid, sizeof(uint32_t), 0);
                log_info(logger, "PATH: %s", path);
                log_info(logger, "PID: %d", pid);
            case FINALIZAR_PROCESO:
            case FETCH:
            case MOV_OUT:
            case RESIZE:
            default:
                uint32_t size_msg;
                recv(socket_cliente, &size_msg, sizeof(uint32_t), 0);
                void* msg = malloc(size_msg);
                recv(socket_cliente, msg, size_msg, 0);
                log_info(logger, "%s", msg);
        }
    }
    return;
}