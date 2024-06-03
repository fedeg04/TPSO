#include <../include/conexion.h>

uint32_t pid_interrumpido;

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
                pid_interrumpido = recibir_interrupcion(socket_cliente);
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
                memcpy(registros_cpu, pcb->registros, sizeof(registros_t));
                while (1)
                {
                    log_info(logger, "PID: <%d> - FETCH - Program Counter: <%d>", pcb->pid, registros_cpu->PC);
                    enviar_pid_pc(pcb->pid, registros_cpu->PC, memoria_fd);
                    char* instruccion = recibir_instruccion(memoria_fd);
                    if(!ejecutar_instruccion(instruccion, logger, pcb, socket_cliente)) {
                        free(instruccion);
                        break;
                    }
                    registros_cpu->PC++;
                    free(instruccion);
                    if(hay_interrupcion(pcb->pid)){
                        enviar_contexto(socket_cliente, pcb, "TIMER");
                        pid_interrumpido = -1;
                        break;
                    }
                    pid_interrumpido = -1;
                }
                free(pcb->registros);
                free(pcb);
                break;
            default:
                break;
        }
    }
    return;
}

int ejecutar_instruccion(char* instruccion, t_log* logger, proceso_t* pcb, int socket) {
    
    char** substrings = string_split(instruccion, " ");
    char* comando = substrings[0];
    op_code opcode = string_to_opcode(comando);
    char* registro_dest;
    char* registro_orig;
    uint32_t valor_dest;
    uint32_t valor_orig;
    int length = 0;
    while (substrings[length] != NULL) {
        length++;
    }
    if(length == 2) {
        registro_dest = substrings[1];
        valor_dest = get_valor_registro(registro_dest);
    }
    if (length == 3)
    {
        registro_dest = substrings[1];
        registro_orig = substrings[2];
        valor_dest = get_valor_registro(registro_dest);
        valor_orig = get_valor_registro(registro_orig);
    }
    
    switch(opcode) {
        case SET:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb->pid, comando, registro_dest, registro_orig);
            valor_dest = atoi(substrings[2]);
            set_registros(registro_dest, valor_dest);
            string_array_destroy(substrings);
            return 1;
        case MOV_IN:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb->pid, comando, registro_dest, registro_orig);
            uint32_t cantidad_bytes_in = cant_bytes(registro_dest);
            uint8_t cant_paginas = cantidad_paginas_enviar(cantidad_bytes_in, valor_orig);
            pedir_marcos(pcb->pid, cant_paginas, pagina_direccion_logica(valor_orig));
            uint32_t lectura = enviar_mov_in(cant_paginas, pcb->pid, cantidad_bytes_in, desplazamiento_direccion_logica(valor_orig));
            set_registros(registro_dest, lectura);
            return 1;
        case MOV_OUT:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb->pid, comando, registro_dest, registro_orig);
            uint32_t cantidad_bytes = cant_bytes(registro_orig);
            uint8_t cant_paginas_enviar = cantidad_paginas_enviar(cantidad_bytes, valor_dest);
            pedir_marcos(pcb->pid, cant_paginas_enviar, pagina_direccion_logica(valor_dest));
            enviar_mov_out(valor_orig, cant_paginas_enviar, pcb->pid, cantidad_bytes, desplazamiento_direccion_logica(valor_dest));
            string_array_destroy(substrings);
            return respuesta_memoria(pcb, socket);
        case SUM:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb->pid, comando, registro_dest, registro_orig);
            set_registros(registro_dest, valor_dest + valor_orig);
            return 1;
        case SUB:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb->pid, comando, registro_dest, registro_orig);
            log_info(logger, "1: %d, 2: %d", valor_dest, valor_orig);
            set_registros(registro_dest, valor_dest - valor_orig);
            return 1;
        case JNZ:
            valor_dest = atoi(substrings[2]);
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %d>", pcb->pid, comando, registro_orig, valor_dest);
            if(valor_orig != 0) {
                set_registros("PC", valor_dest);
            }
        return 1;
        case RESIZE:
            valor_dest = atoi(substrings[1]);
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%d>", pcb->pid, comando, valor_dest);
            enviar_resize(valor_dest, pcb->pid);
            string_array_destroy(substrings);
            return respuesta_memoria(pcb, socket);
        case COPY_STRING:
        return 1;
        case WAIT:
            enviar_contexto(socket, pcb, instruccion);
            return 0;
        case SIGNAL:
            enviar_contexto(socket, pcb, instruccion);
            return 0;
        case IO_GEN_SLEEP:
            log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb->pid, comando, registro_orig, registro_dest);
            registros_cpu->PC++;
            enviar_contexto(socket, pcb, instruccion);
            return 0;
        case IO_STDIN_READ:
        return 1;
        case IO_STDOUT_WRITE:
        return 1;
        case IO_FS_CREATE:
        return 1;
        case IO_FS_DELETE:
        return 1;
        case IO_FS_TRUNCATE:
        return 1;
        case IO_FS_WRITE:
        return 1;
        case IO_FS_READ:
        return 1;
        case EXIT:
            log_info(logger, "PID: <%d> - Ejecutando: <%s>", pcb->pid, instruccion);
            enviar_contexto(socket, pcb, instruccion);
            string_array_destroy(substrings);
            return 0;
    }
    string_array_destroy(substrings);
}

bool hay_interrupcion(uint32_t pid) {
    return pid_interrumpido == pid;
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

int pedir_tamanio_pagina(int socket_memoria) {
    void* stream = malloc(sizeof(op_code));
    int offset = 0;
    agregar_opcode(stream, &offset, TAMANIOPAGINA);
    send(socket_memoria, stream, offset, 0);
    free(stream);
    int tam;
    recv(socket_memoria, &tam, sizeof(int),0);
    return tam;
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

uint32_t recibir_interrupcion(int socket) {
    uint32_t pid_interrumpido_recv; 
    recv(socket, &pid_interrumpido_recv, sizeof(uint32_t), 0); 
    return pid_interrumpido_recv;
}

void enviar_contexto(int socket, proceso_t* pcb, char* instruccion) {
    memcpy(pcb->registros, registros_cpu, sizeof(registros_t));
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

void enviar_resize(uint32_t tamanio, uint32_t pid) {
    void* stream = malloc(sizeof(uint32_t)*2 + sizeof(op_code));
    int offset = 0;
    agregar_opcode(stream, &offset, RESIZE);
    agregar_uint32_t(stream, &offset, tamanio);
    agregar_uint32_t(stream, &offset, pid);
    send(memoria_fd, stream, offset, 0);
    free(stream);
}

void pedir_marcos(uint32_t pid, uint8_t cant_paginas_enviar, uint32_t nro_pagina) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint8_t) + sizeof(uint32_t)*2);
    int offset = 0;
    agregar_opcode(stream, &offset, PEDIR_MARCO);
    agregar_uint32_t(stream, &offset, pid);
    agregar_uint8_t(stream, &offset, cant_paginas_enviar);
    agregar_uint32_t(stream, &offset, nro_pagina);
    send(memoria_fd, stream, offset, 0);
    free(stream);
}

void enviar_mov_out(uint32_t valor, uint8_t cant_pags_a_enviar, uint32_t pid, uint32_t cant_bytes, uint16_t desplazamiento) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint8_t) + sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, MOV_OUT);
    agregar_uint8_t(stream, &offset, cant_pags_a_enviar);
    agregar_uint32_t(stream, &offset, pid);
    uint16_t bytes_restantes = tamanio_pagina - desplazamiento;
    uint16_t marco;
    recv(memoria_fd, &marco, sizeof(uint16_t), 0);
    uint16_t direccion_fisica = marco*tamanio_pagina + desplazamiento;
    if(cant_pags_a_enviar == 1) {
        stream = realloc(stream, offset + sizeof(uint16_t) * 2 + cant_bytes);
        agregar_uint16_t(stream, &offset, direccion_fisica);
        agregar_uint16_t(stream, &offset, cant_bytes);
        void* valor_ptr = ((char*)&valor) + cant_bytes - 1;
        uint8_t* valor_uint_ptr = (uint8_t*)valor_ptr; 
        for (int i = 0; i < cant_bytes; i++)
        {
            uint8_t envio = *(valor_uint_ptr - i);
            agregar_uint8_t(stream, &offset, envio);
        }
    } else {
        desplazamiento = 0;
        stream = realloc(stream, offset + sizeof(uint16_t)*4 + cant_bytes);
        agregar_uint16_t(stream, &offset, direccion_fisica);
        agregar_uint16_t(stream, &offset, bytes_restantes);
        void* valor_ptr = ((char*)&valor) + cant_bytes - 1;
        uint8_t* valor_uint_ptr = (uint8_t*)valor_ptr; 
        for (int i = 0; i < bytes_restantes; i++)
        {
            uint8_t envio = *(valor_uint_ptr - i);
            agregar_uint8_t(stream, &offset, envio);
        }
        valor_ptr = valor_ptr - bytes_restantes;
        bytes_restantes = cant_bytes - bytes_restantes;
        recv(memoria_fd, &marco, sizeof(uint16_t), 0);
        direccion_fisica = marco*tamanio_pagina + desplazamiento;

        agregar_uint16_t(stream, &offset, direccion_fisica);
        agregar_uint16_t(stream, &offset, bytes_restantes);
        valor_uint_ptr = (uint8_t*)valor_ptr;
        for (int i = 0; i < bytes_restantes; i++)
        {
            uint8_t envio = *(valor_uint_ptr - i);
            agregar_uint8_t(stream, &offset, envio);
        }
    }
       
    send(memoria_fd, stream, offset, 0);
    free(stream);
}

uint32_t cant_bytes(char* registro) {
    if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0 || strcmp(registro, "CX") == 0 || strcmp(registro, "DX") == 0) {
        return 1;
    } else if (strcmp(registro, "EAX") == 0 || strcmp(registro, "EBX") == 0 || strcmp(registro, "ECX") == 0 || strcmp(registro, "EDX") == 0) {
        return 4;
    } 
}

bool respuesta_memoria(proceso_t* pcb, int socket_cliente) {
    op_code mov_out_response;
    recv(memoria_fd, &mov_out_response, sizeof(op_code), 0);
    if(mov_out_response != MSG) {
        enviar_contexto(socket_cliente, pcb, OUTOFMEMORY);
        return 0;
    }   
    return 1;
}

uint32_t enviar_mov_in(uint8_t cant_pags, uint32_t pid, uint32_t cant_bytes, uint16_t desplazamiento) {
    char* lectura = malloc(1);
    lectura[0] = '\0';
    uint16_t bytes_restantes = tamanio_pagina - desplazamiento;
    for (int i = 0; i < cant_pags; i++)
    {
        void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) + sizeof(uint16_t)*2);
        int offset = 0;
        agregar_opcode(stream, &offset, MOV_IN);
        agregar_uint32_t(stream, &offset, pid);
        uint16_t marco;
        recv(memoria_fd, &marco, sizeof(uint16_t), 0);
        uint16_t direccion_fisica = marco*tamanio_pagina + desplazamiento;
        if(cant_pags == 1) {
            bytes_restantes = cant_bytes;
        }
        agregar_uint16_t(stream, &offset, direccion_fisica);
        agregar_uint16_t(stream, &offset, bytes_restantes);
        send(memoria_fd, stream, offset, 0);
        free(stream);
        
        bytes_restantes = cant_bytes - bytes_restantes;
        desplazamiento = 0;
    }
    for (int i = 0; i < cant_pags; i++)
    {
        uint16_t cant_bytes_leer;
        recv(memoria_fd, &cant_bytes_leer, sizeof(uint16_t), 0);
        uint8_t numero_leido;
        for (int i = 0; i < cant_bytes_leer; i++)
        {
            recv(memoria_fd, &numero_leido, 1, 0);
            char* lectura_parcial = malloc(3);
            sprintf(lectura_parcial, "%02X", numero_leido);
            string_append(&lectura, lectura_parcial);
            free(lectura_parcial);
        }
    }
    unsigned long valor_hex = strtoul(lectura, NULL, 16);
    free(lectura);
    return (uint32_t)valor_hex;
}