#include <../include/conexion.h>


void procesar_conexion(void* args_void) {
    conexion_args_t* args = (conexion_args_t*) args_void;
    int socket_cliente = args->socket_cliente;
    t_log* logger = args->logger;
    free(args);

    t_list* archivos_procesos = list_create(); //Lista de archivos con PID


    archivo_proceso_t* archivo_proceso = malloc(sizeof(archivo_proceso_t));
    archivo_proceso->f = fopen("./procesos/proceso1.txt", "r");
    archivo_proceso->pid = 1;
    list_add(archivos_procesos, archivo_proceso);
    
    archivo_proceso_t* archivo_proceso2 = malloc(sizeof(archivo_proceso_t));
    archivo_proceso2->f = fopen("./procesos/proceso1.txt", "r");
    archivo_proceso2->pid = 2;
    list_add(archivos_procesos, archivo_proceso2);

    op_code opcode;
    while (socket_cliente != 1) {
        if ((recv(socket_cliente, &opcode, sizeof(op_code), MSG_WAITALL)) != sizeof(op_code)){
            log_info(logger, "Tiro error");
            return;
        }

        switch(opcode) {

            case INICIAR_PROCESO:
                log_info(logger, "Socket: %d\n", socket_cliente);
                uint32_t size;
                recv(socket_cliente, &size, sizeof(uint32_t), 0);
                char* path = malloc(size);
                recv(socket_cliente, path, size, 0);
                uint32_t pid;
                recv(socket_cliente, &pid, sizeof(uint32_t), 0);
                if(existe_archivo(path)) {
                    archivo_proceso_t* archivo_proceso = malloc(sizeof(archivo_proceso_t));
                    archivo_proceso->f = fopen(path, "r");
                    archivo_proceso->pid = pid;
                    agregar_proceso(archivo_proceso, archivos_procesos);
                    enviar_pid(socket_cliente, archivo_proceso->pid);
                } else {
                    enviar_pid(socket_cliente, 0);
                }
                break;
            case FINALIZAR_PROCESO:
            break;
            case FETCH:
                uint32_t pid_a_buscar; 
                log_info(logger, "Socket: %d\n", socket_cliente);
                recv(socket_cliente, &pid_a_buscar, sizeof(uint32_t), 0);
                uint32_t pc;
                recv(socket_cliente, &pc, sizeof(uint32_t), 0);
                log_info(logger, "PC: %d", pc);
                char* instruccion = buscar_instruccion(pid_a_buscar, pc, archivos_procesos);
                //enviar_instruccion(socket_cliente, instruccion);
                log_info(logger, "Instruccion: %s", instruccion);
                break;
            case MOV_OUT:
            break;
            case RESIZE:
            break;
            default:
                uint32_t size_msg;
                recv(socket_cliente, &size_msg, sizeof(uint32_t), 0);
                void* msg = malloc(size_msg);
                recv(socket_cliente, msg, size_msg, 0);
                log_info(logger, "%s", msg);
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

}