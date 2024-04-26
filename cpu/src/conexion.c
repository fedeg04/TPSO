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
                proceso_t* pcb = malloc(sizeof(proceso_t));  
                pcb->registros = malloc(sizeof(registros_t));
                recibir_pcb(socket_cliente, pcb, logger);
                log_info(logger, "PCB->PC: %d", pcb->registros->PC);
                memcpy(registros_cpu, pcb->registros, sizeof(registros_t));
                while (1)
                {
                    log_info(logger, "PID: <%d> - FETCH - Program Counter: <%d>", pcb->pid, registros_cpu->PC);
                    enviar_pid_pc(pcb->pid, registros_cpu->PC, memoria_fd);
                    char* instruccion = recibir_instruccion(memoria_fd);
                    if(!strcmp(instruccion, "EXIT")) {
                        memcpy(pcb->registros, registros_cpu, sizeof(register_t));
                        log_info(logger, "PID: <%d> - Ejecutando: <%s>", pcb->pid, instruccion);
                        enviar_contexto(socket_cliente, pcb, instruccion);
                        free(pcb->registros);
                        free(pcb);
                        free(instruccion);

                        break;
                    }
                    ejecutar_instruccion(instruccion, logger, pcb->pid);
                    //verificar_interrupcion();
                    registros_cpu->PC++;
                    free(instruccion);
                }
                log_info(logger, "AX: %u", registros_cpu->AX);
                log_info(logger, "BX: %u", registros_cpu->BX);
                break;
            default:
                break;
        }
    }
    return;
}

void ejecutar_instruccion(char* instruccion, t_log* logger, uint32_t pid) {
    
    char** substrings = string_split(instruccion, " ");
    char* comando = substrings[0];
    op_code opcode = string_to_opcode(comando);
    char* registro_dest;
    char* registro_orig;
    uint32_t valor_dest;
    uint32_t valor_orig;
    //uint8_t valor_dest;
    //uint8_t valor_orig;

    switch(opcode) {
        case SET:
            registro_dest = substrings[1];
            valor_dest = atoi(substrings[2]);
            set_registros(registro_dest, valor_dest);
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %u>", pid, comando, registro_dest, valor_dest);
            break;
        case MOV_IN:
        break;
        case MOV_OUT:
        break;
        case SUM:
            registro_dest = substrings[1];
            registro_orig = substrings[2];
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pid, comando, registro_dest, registro_orig);
            valor_dest = get_valor_registro(registro_dest);
            valor_orig = get_valor_registro(registro_orig);
            set_registros(registro_dest, valor_dest + valor_orig);
            break;
        case SUB:
            registro_dest = substrings[1];
            registro_orig = substrings[2];
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pid, comando, registro_dest, registro_orig);
            valor_dest = get_valor_registro(registro_dest);
            valor_orig = get_valor_registro(registro_orig);
            log_info(logger, "1: %d, 2: %d", valor_dest, valor_orig);
            set_registros(registro_dest, valor_dest - valor_orig);
            break;
        case JNZ:
            registro_orig = substrings[1];
            valor_dest = atoi(substrings[2]);
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %d>", pid, comando, registro_orig, valor_dest);
            valor_orig = get_valor_registro(registro_orig);
            if(valor_orig != 0) {
                set_registros("PC", valor_dest);
            }
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
    string_split_free(&substrings);
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

void recibir_pcb(int socket, proceso_t* pcb, t_log* logger) {
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
    pcb->registros->PC = PC;
    pcb->registros->AX = AX;
    pcb->registros->BX = BX;
    pcb->registros->CX = CX;
    pcb->registros->DX = DX;
    pcb->registros->EAX = EAX;
    pcb->registros->EBX = EBX;
    pcb->registros->ECX = ECX;
    pcb->registros->EDX = EDX;
    pcb->registros->SI = SI;
    pcb->registros->DI = DI;
}

char* recibir_instruccion(int socket) {
    uint32_t size;
    recv(socket, &size, sizeof(uint32_t), 0);
    char* instruccion = malloc(size);
    recv(socket, instruccion, size, 0);
    return instruccion;
}

void enviar_contexto(int socket, proceso_t* pcb, char* instruccion) {
    uint32_t tamanio = string_length(instruccion) + 1;
    void* stream = malloc(10 * sizeof(uint32_t) + 4 * sizeof(uint8_t) + tamanio);
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
    agregar_string(stream, &offset, instruccion);
    send(socket, stream, offset, 0);
    free(stream);
}

void set_registros(char* registro_dest, uint32_t valor) {
    if (strcmp(registro_dest, "AX") == 0) {
        registros_cpu->AX = valor;
    } else if (strcmp(registro_dest, "BX") == 0) {
        registros_cpu->BX = valor;
    } else if (strcmp(registro_dest, "CX") == 0) {
        registros_cpu->CX = valor;
    } else if (strcmp(registro_dest, "DX") == 0) {
        registros_cpu->DX = valor;
    } else if (strcmp(registro_dest, "EAX") == 0) {
        registros_cpu->EAX = valor;
    } else if (strcmp(registro_dest, "EBX") == 0) {
        registros_cpu->EBX = valor;
    } else if (strcmp(registro_dest, "ECX") == 0) {
        registros_cpu->ECX = valor;
    } else if (strcmp(registro_dest, "EDX") == 0) {
        registros_cpu->EDX = valor;
    } else if (strcmp(registro_dest, "PC") == 0) {
        registros_cpu->PC = valor-1;
    } else if (strcmp(registro_dest, "SI") == 0) {
        registros_cpu->SI = valor;
    } else if (strcmp(registro_dest, "DI") == 0) {
        registros_cpu->DI = valor;
    }
}

uint32_t get_valor_registro(char* registro) {
    if(registro[strlen(registro) -1] == '\n') registro[strlen(registro) -1] = '\0';
    if (strcmp(registro, "AX") == 0) {
        return registros_cpu->AX;
    } else if (strcmp(registro, "BX") == 0) {
        return registros_cpu->BX;
    } else if (strcmp(registro, "CX") == 0) {
        return registros_cpu->CX;
    } else if (strcmp(registro, "DX") == 0) {
        return registros_cpu->DX;
    } else if (strcmp(registro, "EAX") == 0) {
        return registros_cpu->EAX;
    } else if (strcmp(registro, "EBX") == 0) {
        return registros_cpu->EBX;
    } else if (strcmp(registro, "ECX") == 0) {
        return registros_cpu->ECX;
    } else if (strcmp(registro, "EDX") == 0) {
        return registros_cpu->EDX;
    } else if (strcmp(registro, "PC") == 0) {
        return registros_cpu->PC;
    } else if (strcmp(registro, "SI") == 0) {
        return registros_cpu->SI;
    } else if (strcmp(registro, "DI") == 0) {
        return registros_cpu->DI;
    }
}