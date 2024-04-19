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
                while (1)
                {
                    enviar_pid_pc(pcb->pid, registros_cpu->PC, memoria_fd);
                    char* instruccion = recibir_instruccion(memoria_fd);
                    if(!strcmp(instruccion, "EXIT")) {
                        pcb->registros = registros_cpu;
                        enviar_contexto(socket_cliente, pcb);
                        break;
                    }
                    ejecutar_instruccion(instruccion, logger);
                    //verificar_interrupcion();
                    registros_cpu->PC++;
                }
                log_info(logger, "AX: %d", registros_cpu->AX);
                log_info(logger, "BX: %d", registros_cpu->BX);
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
    
    char** substrings = string_split(instruccion, " "); // [SET, AX, 1];
    char* comando = substrings[0];
    op_code opcode = string_to_opcode(comando);

    switch(opcode) {
        case SET:
            char* registro_set = substrings[1];
            uint8_t valor_set = atoi(substrings[2]);
            
            if (strcmp(registro_set, "AX") == 0) {
                registros_cpu->AX = valor_set;
            } else if (strcmp(registro_set, "BX") == 0) {
                registros_cpu->BX = valor_set;
            } else if (strcmp(registro_set, "CX") == 0) {
                registros_cpu->CX = valor_set;
            } else if (strcmp(registro_set, "DX") == 0) {
                registros_cpu->DX = valor_set;
            } else if (strcmp(registro_set, "EAX") == 0) {
                registros_cpu->EAX = valor_set;
            } else if (strcmp(registro_set, "EBX") == 0) {
                registros_cpu->EBX = valor_set;
            } else if (strcmp(registro_set, "ECX") == 0) {
                registros_cpu->ECX = valor_set;
            } else if (strcmp(registro_set, "EDX") == 0) {
                registros_cpu->EDX = valor_set;
            } else if (strcmp(registro_set, "PC") == 0) {
                registros_cpu->PC = valor_set-1;
            } else if (strcmp(registro_set, "SI") == 0) {
                registros_cpu->SI = valor_set;
            } else if (strcmp(registro_set, "DI") == 0) {
                registros_cpu->DI = valor_set;
            } else {
                log_info(logger, "Registro no válido: %s\n", registro_set);
            }
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
    registros_cpu->PC = PC;
    registros_cpu->AX = AX;
    registros_cpu->BX = BX;
    registros_cpu->CX = CX;
    registros_cpu->DX = DX;
    registros_cpu->EAX = EAX;
    registros_cpu->EBX = EBX;
    registros_cpu->ECX = ECX;
    registros_cpu->EDX = EDX;
    registros_cpu->SI = SI;
    registros_cpu->DI = DI;
}

char* recibir_instruccion(int socket) {
    uint32_t size;
    recv(socket, &size, sizeof(uint32_t), 0);
    char* instruccion = malloc(size);
    recv(socket, instruccion, size, 0);
    return instruccion;
}

void enviar_contexto(int socket, proceso_t* pcb) {
    void* stream = malloc(9 * sizeof(uint32_t) + 4 * sizeof(uint8_t));
    int offset = 0;
    agregar_uint32_t(stream, &offset, pcb->pid);
    agregar_uint32_t(stream, &offset, pcb->quantum);
    agregar_uint32_t(stream, &offset, pcb->registros->PC);
    agregar_uint8_t(stream, &offset, pcb->registros->AX);
    agregar_uint8_t(stream, &offset, pcb->registros->BX);
    agregar_uint8_t(stream, &offset, pcb->registros->CX);
    agregar_uint8_t(stream, &offset, pcb->registros->DX);
    agregar_uint32_t(stream, &offset, pcb->registros->EAX);
    agregar_uint32_t(stream, &offset, pcb->registros->EBX);
    agregar_uint32_t(stream, &offset, pcb->registros->ECX);
    agregar_uint32_t(stream, &offset, pcb->registros->EDX);
    agregar_uint32_t(stream, &offset, pcb->registros->SI);
    agregar_uint32_t(stream, &offset, pcb->registros->DI);
    send(socket, stream, offset, 0);
}