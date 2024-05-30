#include <../include/conexion.h>

 //Lista de archivos con PID
uint32_t pid_actual;
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
                uint32_t size;
                recv(socket_cliente, &size, sizeof(uint32_t), 0);
                char* path = malloc(size);
                recv(socket_cliente, path, size, 0);
                log_info(logger, "PATH: %s", path);
                uint32_t pid;
                recv(socket_cliente, &pid, sizeof(uint32_t), 0);
                log_info(logger, "PID RECIBIDO: %d", pid);
                if(existe_archivo(path)) {
                    agregar_proceso(archivos_procesos, path, pid);
                    int cant_marcos = cantidad_marcos();
                    tabla_t* tabla_paginas = iniciar_tabla(pid, cant_marcos);
                    log_info(logger, "PID: <%d> - Tamaño: <%d>", pid, cant_marcos);
                    list_add(tablas_paginas_memoria, tabla_paginas);
                } else {
                    pid = 0;
                }
                enviar_pid(socket_cliente, pid);
                break;
            case FINALIZAR_PROCESO:
            break;
            case FETCH:
                uint32_t pid_a_buscar; 
                recv(socket_cliente, &pid_a_buscar, sizeof(uint32_t), 0);
                uint32_t pc;
                recv(socket_cliente, &pc, sizeof(uint32_t), 0);
                char* instruccion = buscar_instruccion(pid_a_buscar, pc, archivos_procesos);
                log_info(logger, "Instruccion: %s", instruccion);
                log_info(logger, "IRetardo: %d", retardo_respuesta);
                usleep(retardo_respuesta * 1000);
                enviar_instruccion(socket_cliente, instruccion);
                free(instruccion);
                break;
            case MOV_OUT:
            break;
            case RESIZE:
                uint32_t tamanio;
                recv(socket_cliente, &tamanio, sizeof(uint32_t), 0);
                uint32_t pid_resize;
                recv(socket_cliente, &pid_resize, sizeof(uint32_t), 0);
                int tam_proceso = tamanio_proceso(pid_resize);
                int response = 0;
                if(tamanio >= tam_proceso) {
                    log_info(logger, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>", pid_resize, tamanio_proceso(pid_resize),tamanio);
                    response = ampliar_tamanio_proceso(pid_resize, tamanio-tam_proceso);
                } else {
                    log_info(logger, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d>", pid_resize, tamanio_proceso(pid_resize),tamanio);
                    reducir_tamanio_proceso(pid_resize, tam_proceso - tamanio);
                }
                if(response) {
                    enviar_out_of_memory(socket_cliente);
                } else {
                    enviar_resize_ok(socket_cliente);
                }
                break;
            default:
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
    void* stream = malloc(sizeof(uint32_t) + string_length(instruccion) + 1);
    int offset = 0;
    agregar_string(stream, &offset, instruccion);
    send(socket, stream, offset, 0);
    free(stream);
}

int cantidad_marcos() {
    return tam_memoria/tam_pagina;
}

int tamanio_proceso(uint32_t pid) {
    int tamanio_proceso = 0;
    tabla_t* tabla_proceso = malloc(sizeof(tabla_t));
    tabla_proceso = tabla_paginas_por_pid(pid);
    for (int i = 0; i < cantidad_marcos(); i++)
    {
        pagina_t* pagina = malloc(sizeof(pagina_t));
        pagina = list_get(tabla_proceso->paginas, i);
        tamanio_proceso += pagina->bytes_ocupados;
    }
    return tamanio_proceso;
}

tabla_t* tabla_paginas_por_pid(uint32_t pid) {
    pid_actual = pid;
    return list_find(tablas_paginas_memoria, (void*)tabla_paginas_por_pid_actual);
}

bool tabla_paginas_por_pid_actual(tabla_t* tabla) {
    return (tabla->pid == pid_actual);
}

bool ampliar_tamanio_proceso(uint32_t pid, int tamanio) {
    tabla_t* tabla_proceso = malloc(sizeof(tabla_t));
    tabla_proceso = tabla_paginas_por_pid(pid);
    int tamanio_restante = completar_ultima_pagina(tabla_proceso, tamanio);
    
    int cant_marcos_otorgar = (tam_pagina+tamanio_restante-1)/tam_pagina;
    
    for (int i = 0; (i < cantidad_marcos()) && cant_marcos_otorgar; i++)
    {
        if(!bitarray_test_bit(bitarray_tabla,i)) {
            pagina_t* pagina = malloc(sizeof(pagina_t));
            pagina = list_get(tabla_proceso->paginas, i);
            bitarray_set_bit(bitarray_tabla,i);
            if(cant_marcos_otorgar == 1) {
                pagina->bytes_ocupados = tamanio_restante;
            } else {
                pagina->bytes_ocupados = tam_pagina;
            }
            tabla_proceso->ultimo_marco_modificado = pagina->marco;
            cant_marcos_otorgar--;
            tamanio_restante -= tam_pagina;
        }
    }
    return cant_marcos_otorgar;
}

int completar_ultima_pagina(tabla_t* tabla, int tamanio) {
    if(tabla->ultimo_marco_modificado == -1) {
        return tamanio;
    }
    pagina_t* pagina = malloc(sizeof(pagina_t));
    pagina = list_get(tabla->paginas, tabla->ultimo_marco_modificado);
    int espacio_restante = tam_pagina - pagina->bytes_ocupados;
    bitarray_set_bit(bitarray_tabla,tabla->ultimo_marco_modificado);
    if (tamanio <= espacio_restante) {
        pagina->bytes_ocupados += tamanio;
        return 0;
    } else {
        pagina->bytes_ocupados = tam_pagina;
        return tamanio - espacio_restante;
    }
}

void enviar_out_of_memory(int socket_cliente) {
    void* stream = malloc(sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, OUTOFMEMORY);
    send(socket_cliente, stream, offset, 0);
    free(stream);
}


void enviar_resize_ok(int socket_cliente) {
    void* stream = malloc(sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, MSG);
    send(socket_cliente, stream, offset, 0);
    free(stream);
}


void reducir_tamanio_proceso(uint32_t pid, int tamanio) {
    tabla_t* tabla_proceso = malloc(sizeof(tabla_t));
    tabla_proceso = tabla_paginas_por_pid(pid);
    tamanio = vaciar_ultima_pagina(tabla_proceso, tamanio);
    int cant_marcos_liberar = (tam_pagina+tamanio-1)/tam_pagina;

    for (int i = 0; cant_marcos_liberar; i++)
    {
        if(bitarray_test_bit(bitarray_tabla,i)) {
            pagina_t* pagina = malloc(sizeof(pagina_t));
            pagina = list_get(tabla_proceso->paginas, i);
            if(pagina->bytes_ocupados){
                if(tamanio >= tam_pagina) {
                    pagina->bytes_ocupados = 0;
                    bitarray_clean_bit(bitarray_tabla, i);
                } else {
                    pagina->bytes_ocupados -= tamanio;
                }
                tabla_proceso->ultimo_marco_modificado = pagina->marco;
                cant_marcos_liberar--;
                tamanio -= tam_pagina;
            }
        }
    }
}

bool tiene_validez(pagina_t* pagina) {
    return pagina->bytes_ocupados;
}

int vaciar_ultima_pagina(tabla_t* tabla, int tamanio) {
    if(tabla->ultimo_marco_modificado == -1) {
        return tamanio;
    }
    pagina_t* pagina = list_get(tabla->paginas, tabla->ultimo_marco_modificado);
    if (tamanio >= pagina->bytes_ocupados) {
        int restante = tamanio - pagina->bytes_ocupados;
        pagina->bytes_ocupados = 0;
        bitarray_clean_bit(bitarray_tabla, pagina->marco);
        return restante;
    } else {
        pagina->bytes_ocupados -= tamanio;
        return 0;
    }
}
