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
                conectar_generica();
                break;
            case STDIN:
                conectar_stdin();
                break;
            case STDOUT:
                conectar_stdout();
                break;
            case DIALFS:
                conectar_dialfs();
                break;
            case FIN_DE_SLEEP:
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

void conectar_generica() {

}

void conectar_stdin() {
    
}

void conectar_stdout() {
    
}

void conectar_dialfs() {
    
}