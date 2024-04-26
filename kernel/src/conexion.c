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
                log_info(logger, "Se desconect{o la genérica");
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
                recibir_fin_de_sleep();
                break;
            default:
                uint32_t size_msg;
                recv(socket_cliente, &size_msg, sizeof(uint32_t), 0);
                void* msg = malloc(size_msg);
                recv(socket_cliente, msg, size_msg, 0);
                //log_info(logger, "%s", msg);
        }
    }
    return;
}

void conectar_interfaz(char* interfaz, int socket) {
    dictionary_put(diccionario_interfaces, interfaz, (void*) socket);
}

void desconectar_interfaz(char* interfaz) {
    dictionary_remove(diccionario_interfaces, interfaz);
}