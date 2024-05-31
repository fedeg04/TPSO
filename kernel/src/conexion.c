#include <../include/conexion.h>

void procesar_conexion(void* args_void) {
    conexion_args_t* args = (conexion_args_t*) args_void;
    int socket_cliente = args->socket_cliente;
    t_log* logger = args->logger;
    free(args);

    op_code opcode;
    while (socket_cliente != 1) {
        if ((recv(socket_cliente, &opcode, sizeof(op_code), 0)) != sizeof(op_code)){
            log_info(logger, "Tiro error");
            return;
        }
        log_info(logger_kernel, "OPCODE: %d", opcode);
        switch(opcode) {
            case GENERICA:
                log_info(logger, "Se conectó la interfaz genérica");
                conectar_interfaz("GENERICA", socket_cliente);
                break;
            case STDIN:
                conectar_interfaz("STDIN", socket_cliente);
                break;
            case STDOUT:
                conectar_interfaz("STDOUT", socket_cliente);
                break;
            case DIALFS:
                conectar_interfaz("DIALFS", socket_cliente);
                break;
            case GENERICA_BYE:
                log_info(logger, "Se desconectó la genérica");
                desconectar_interfaz("GENERICA");
                break;
            case STDIN_BYE:
                desconectar_interfaz("STDIN");
                break;
            case STDOUT_BYE:
                desconectar_interfaz("STDOUT");
                break;
            case DIALFS_BYE:
                desconectar_interfaz("DIALFS");
                break;
            case FIN_DE_SLEEP:
                log_info(logger, "VOLVIO PROCESO DE HACER SLEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP");
                sem_post(&vuelta_io_gen_sleep);
                break;
            case FIN_DE_STDIN:
                break;
            case FIN_DE_STDOUT:
                break;
            default:
        }
    }
    log_info(logger_kernel, "desconectado por bobi");
    return;
}

void conectar_interfaz(char* interfaz, int socket) {
    dictionary_put(diccionario_interfaces, interfaz, (void*) socket);
}

void desconectar_interfaz(char* interfaz) {
    dictionary_remove(diccionario_interfaces, interfaz);
}