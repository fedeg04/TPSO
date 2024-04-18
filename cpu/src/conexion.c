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
                log_info(logger, "Me llego un proceso");
                proceso_t* pcb = malloc(sizeof(proceso_t));
                pcb->registros = malloc(sizeof(registros_t));
                recibir_pcb(socket_cliente, pcb);
                registros_cpu = pcb->registros;
                enviar_pid_pc(pcb->pid, registros_cpu->PC, memoria_fd);

                char* instruccion = recibir_instruccion(memoria_fd);
                
                ejecutar_instruccion(instruccion, logger);
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

void ejecutar_instruccion(char* instruccion, t_log* logger) {
    //SET AX 1
    //COPY_STRING 8
    //IO_FS_WRITE Int4 notas.txt AX ECX EDX
    //comando = SET, COPYSTRI, IO_FS
    log_info(logger, "Instruccion: %s", instruccion);
    log_info(logger, "2: %d", (int)instruccion[2]);
    log_info(logger, "3: %d", (int)instruccion[3]);
    log_info(logger, "4: %d", (int)instruccion[4]);
    
    char** substrings = string_split(instruccion, " "); // [SET, AX, 1];
    char* comando = substrings[0];
    op_code opcode = string_to_opcode(comando);

    switch(opcode) {
        case SET:
            char* registro_set = substrings[1];
            uint8_t valor_set = atoi(substrings[2]);

            break;
        case MOV_IN:
        break;
        case MOV_OUT:
        break;
        case SUM:
        break;
        case SUB:
        break;
        case JNZ:
        break;
        case RESIZE:
        break;
        case COPY_STRING:
        break;
        case WAIT:
        break;
        case SIGNAL:
        break;
        case IO_GEN_SLEEP:
        break;
        case IO_STDIN_READ:
        break;
        case IO_STDOUT_WRITE:
        break;
        case IO_FS_CREATE:
        break;
        case IO_FS_DELETE:
        break;
        case IO_FS_TRUNCATE:
        break;
        case IO_FS_WRITE:
        break;
        case IO_FS_READ:
        break;
        case EXIT:
        break;
    }
}

void enviar_pid_pc(uint32_t pid, uint32_t pc, int socket) {
    void* stream = malloc(sizeof(uint32_t)*2 + sizeof(op_code));
    int offset = 0;
    agregar_opcode(stream, &offset, FETCH);
    agregar_uint32_t(stream, &offset, pid);
    agregar_uint32_t(stream, &offset, pc);
    send(socket, stream, offset, 0);
    free(stream);
}

void recibir_pcb(int socket, proceso_t* pcb) {
    uint32_t pid;
    uint32_t quantum;
    uint32_t PC;
    uint8_t AX;
    uint8_t BX;
    uint8_t CX;
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t SI;
    uint32_t DI;
    recv(socket, &pid, sizeof(uint32_t), 0);
    recv(socket, &quantum, sizeof(uint32_t), 0);
    recv(socket, &PC, sizeof(uint32_t), 0);
    recv(socket, &AX, sizeof(uint8_t), 0);
    recv(socket, &BX, sizeof(uint8_t), 0);
    recv(socket, &CX, sizeof(uint8_t), 0);
    recv(socket, &DX, sizeof(uint8_t), 0);
    recv(socket, &EAX, sizeof(uint32_t), 0);
    recv(socket, &EBX, sizeof(uint32_t), 0);
    recv(socket, &ECX, sizeof(uint32_t), 0);
    recv(socket, &EDX, sizeof(uint32_t), 0);
    recv(socket, &SI, sizeof(uint32_t), 0);
    recv(socket, &DI, sizeof(uint32_t), 0);
    pcb->pid = pid;
    pcb->quantum = quantum;
    pcb->registros->AX = AX;
    pcb->registros->BX = BX;
    pcb->registros->CX = CX;
    pcb->registros->DX = DX;
    pcb->registros->EAX = EAX;
    pcb->registros->EBX = EBX;
    pcb->registros->ECX = ECX;
    pcb->registros->EDX = EDX;
    pcb->registros->SI = SI;
    pcb->registros->SI = DI;
}

char* recibir_instruccion(int socket) {
    uint32_t size;
    recv(socket, &size, sizeof(uint32_t), 0);
    char* instruccion = malloc(size);
    recv(socket, instruccion, size, 0);
    return instruccion;
}