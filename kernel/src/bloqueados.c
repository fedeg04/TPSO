#include <../include/bloqueados.h>
void enviar_proceso_io_gen_sleep(proceso_t *proceso, char *interfaz_sleep, uint32_t uni_de_trabajo)
{
    if (!strcmp("GENERICA", interfaz_sleep))
    {
        proceso_sleep_t* proceso_a_sleep = malloc(sizeof(proceso_sleep_t));
        proceso_a_sleep->proceso = proceso;
        proceso_a_sleep->uni_de_trabajo = uni_de_trabajo;
        pthread_mutex_lock(&mutex_generica_list);
        list_add(pcbs_generica, proceso_a_sleep);
        pthread_mutex_unlock(&mutex_generica_list);
        sem_post(&pcb_esperando_generica);
    }
    else
    {
        log_info(logger_kernel, "Finaliza el proceso %d - Motivo: INTERFAZ INCORRECTA", proceso->pid);
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <EXIT>", proceso->pid);
        entrar_a_exit(proceso);
    }
}

int estaConectada(char *interfaz)
{
    return dictionary_has_key(diccionario_interfaces, interfaz);
}

void hacer_io_gen_sleep()
{
        sem_wait(&pcb_esperando_generica);
        pthread_mutex_lock(&mutex_generica_exec);
        pthread_mutex_lock(&mutex_generica_list);
        proceso_sleep_t* proceso_a_sleep = list_get(pcbs_generica, 0);
        pthread_mutex_unlock(&mutex_generica_list);
    if (estaConectada("GENERICA"))
    {
        void *stream = malloc(sizeof(op_code) + sizeof(uint32_t));
        int offset = 0;
        agregar_opcode(stream, &offset, IO_GEN_SLEEP);
        log_info(logger_kernel, "UNIS: %d", proceso_a_sleep->uni_de_trabajo);
        agregar_uint32_t(stream, &offset, proceso_a_sleep->uni_de_trabajo);
        int socket_generica = (int)dictionary_get(diccionario_interfaces, "GENERICA");
        send(socket_generica, stream, offset, 0);
        free(stream);
        sem_wait(&vuelta_io_gen_sleep);
        recibir_fin_de_sleep();
    }
    else
    {
        log_info(logger_kernel, "Finaliza el proceso %d - Motivo: INTERFAZ NO CONECTADA", proceso_a_sleep->proceso->pid);
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <EXIT>", proceso_a_sleep->proceso->pid);
        proceso_t* proceso = proceso_a_sleep->proceso;
        free(proceso_a_sleep);
        pthread_mutex_unlock(&mutex_generica_exec);
        entrar_a_exit(proceso);   
    }
}

void recibir_fin_de_sleep()
{
    if(fin_a_proceso_sleep != 1) {
    pthread_mutex_lock(&mutex_generica_list);
    proceso_sleep_t *proceso_de_sleep = list_remove(pcbs_generica, 0);
    proceso_t *proceso_a_desbloquear = proceso_de_sleep->proceso;
    free(proceso_de_sleep);
    pthread_mutex_unlock(&mutex_generica_list);
    pthread_mutex_unlock(&mutex_generica_exec);
    desbloquear_proceso(proceso_a_desbloquear);
    }
    else {
        pthread_mutex_unlock(&mutex_generica_exec);
        //signal para finalizar proceso
    }
}

void desbloquear_proceso(proceso_t *proceso)
{
    log_info(logger_kernel, "PID: <%d> - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", proceso->pid);
    if (proceso->quantum == quantum)
    {
        pthread_mutex_lock(&mutex_ready_list);
        list_add(pcbs_ready, proceso);
        mostrar_pids_ready(pcbs_ready, "READY");
        pthread_mutex_unlock(&mutex_ready_list);
        sem_post(&pcb_esperando_exec);
        ingresar_a_exec();
    }
    else
    {
        pthread_mutex_lock(&mutex_ready_prioritario_list);
        list_add(pcbs_ready_prioritarios, proceso);
        mostrar_pids_ready(pcbs_ready_prioritarios, "READY PRIORITARIO");
        pthread_mutex_unlock(&mutex_ready_prioritario_list);
        sem_post(&pcb_esperando_exec);
        ingresar_a_exec();
    }

}

void enviar_proceso_a_wait(proceso_t* proceso, char* recurso_wait, uint32_t tiempo_en_cpu, t_temporal* timer) {
    if(existe_recurso(recurso_wait)) {
        pthread_mutex_lock(&mutex_recursos_list[posicion_de_recurso(recurso_wait)]);
        list_add(lista_de_recurso(recurso_wait), proceso);
        pthread_mutex_unlock(&mutex_recursos_list[posicion_de_recurso(recurso_wait)]);
        if(hay_recursos_de(recurso_wait)) {
            pedir_recurso(recurso_wait);
            volver_a_exec_o_ready(proceso, tiempo_en_cpu, timer);
        }
        else {
            liberar_cpu();
            temporal_stop(timer);
        if (!strcmp(algoritmo_planificacion, "VRR"))
    {
        if (temporal_gettime(timer) - tiempo_en_cpu < proceso->quantum)
        {
            proceso->quantum -= temporal_gettime(timer) - tiempo_en_cpu;
        }
        else
        {
            proceso->quantum = quantum;
        }
    }
            pedir_recurso(recurso_wait);
            desbloquear_proceso(proceso);
        }

    }
    else {
        entrar_a_exit(proceso);
    }
}

int posicion_de_recurso(char* recurso_wait) {
    for(int i=0; i< cantidad_recursos; i++) {
        if(!strcmp(recurso_wait, recursos[i])){
            return i;
        }
    }
}

t_list* lista_de_recurso(char* recurso) {
    return pcbs_recursos[posicion_de_recurso(recurso)];
}

bool hay_recursos_de(char* recurso) {
    pthread_mutex_lock(&mutex_recursos_instancias[posicion_de_recurso(recurso)]);
    bool hay_recursos = instancias_recursos[posicion_de_recurso(recurso)] > 0;
    pthread_mutex_unlock(&mutex_recursos_instancias[posicion_de_recurso(recurso)]);
    return hay_recursos;
}

bool existe_recurso(char* recurso) {
    for(int i=0; i< cantidad_recursos; i++) {
        if(!strcmp(recurso, recursos[i])){
            return true;
        }
    }
    return false;
}
void pedir_recurso(char* recurso_wait) {
    int indice = posicion_de_recurso(recurso_wait);
    sem_wait(&pcb_esperando_recurso[indice]);
    pthread_mutex_lock(&mutex_recursos_list[posicion_de_recurso(recurso_wait)]);
    proceso_t* proceso = list_remove(lista_de_recurso(recurso_wait), 0);
    pthread_mutex_unlock(&mutex_recursos_list[posicion_de_recurso(recurso_wait)]);
    pthread_mutex_lock(&mutex_recursos_instancias[indice]);
    instancias_recursos[indice]--;
    pthread_mutex_unlock(&mutex_recursos_instancias[indice]);

}

void enviar_proceso_a_signal(proceso_t* proceso, char* recurso_signal) {
    if(existe_recurso(recurso_signal)) {
        devolver_recurso(recurso_signal);
        // devolver proceso a ejecucion
    }
    else {
        entrar_a_exit(proceso);
    }
}

void devolver_recurso(char* recurso_signal) {

}

void enviar_proceso_a_interfaz(proceso_a_interfaz_t* proceso_a_interfaz, char* interfaz, void (*hacer_peticion)(proceso_a_interfaz_t*)) {
 if(!strcmp(interfaz, proceso_a_interfaz->interfaz) && estaConectada(proceso_a_interfaz->interfaz)) {
        hacer_peticion(proceso_a_interfaz);
    }
    else {
        entrar_a_exit(proceso_a_interfaz->proceso);
        free(proceso_a_interfaz);
    }
}

void hacer_io_stdin_read(proceso_a_interfaz_t* proceso_interfaz) {
    proceso_t* proceso = proceso_interfaz->proceso;
    uint32_t registro_direccion = proceso_interfaz->registro_direccion;
    uint32_t registro_tamanio = proceso_interfaz->registro_tamanio;
    free(proceso_interfaz);
    pthread_mutex_lock(&mutex_stdin_list);
    list_add(pcbs_stdin, proceso);
    pthread_mutex_unlock(&mutex_stdin_list);
    pthread_mutex_lock(&mutex_stdin_exec);
    pthread_mutex_lock(&mutex_stdin_list);
    list_get(pcbs_stdin, 0);
    pthread_mutex_unlock(&mutex_stdin_list);
    void *stream = malloc(sizeof(op_code) + 3 * sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, IO_STDIN_READ);
    agregar_uint32_t(stream, &offset, proceso->pid);
    agregar_uint32_t(stream, &offset, registro_direccion);
    agregar_uint32_t(stream, &offset, registro_tamanio);
    int socket_generica = (int)dictionary_get(diccionario_interfaces, "STDIN");
    send(socket_generica, stream, offset, 0);
    free(stream);
    sem_wait(&vuelta_io_stdin_read);
}

void hacer_io_stdout_write(proceso_a_interfaz_t* proceso_interfaz) {
    proceso_t* proceso = proceso_interfaz->proceso;
    uint32_t registro_direccion = proceso_interfaz->registro_direccion;
    uint32_t registro_tamanio = proceso_interfaz->registro_tamanio;
    free(proceso_interfaz);
    pthread_mutex_lock(&mutex_stdout_list);
    list_add(pcbs_stdout, proceso);
    pthread_mutex_unlock(&mutex_stdout_list);
    pthread_mutex_lock(&mutex_stdout_exec);
    pthread_mutex_lock(&mutex_stdout_list);
    list_get(pcbs_stdout, 0);
    pthread_mutex_unlock(&mutex_stdout_list);
    void *stream = malloc(sizeof(op_code) + 3 * sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, IO_STDOUT_WRITE);
    agregar_uint32_t(stream, &offset, proceso->pid);
    agregar_uint32_t(stream, &offset, registro_direccion);
    agregar_uint32_t(stream, &offset, registro_tamanio);
    int socket_generica = (int)dictionary_get(diccionario_interfaces, "STDOUT");
    send(socket_generica, stream, offset, 0);
    free(stream);
    sem_wait(&vuelta_io_stdout_write);
}

void volver_a_exec_o_ready(proceso_t* proceso, uint32_t tiempo_en_cpu, t_temporal* timer) {
    temporal_stop(timer);
    int tiempo_restante_exec;
    if(!strcmp("VRR", algoritmo_planificacion)) {
        tiempo_restante_exec = proceso->quantum - (temporal_gettime(timer) - (int) tiempo_en_cpu);
    }
    else {
        tiempo_restante_exec = proceso->quantum - temporal_gettime(timer);
    }
    if(tiempo_restante_exec > 0 || !strcmp(algoritmo_planificacion, "FIFO")) {
            ejecutar_proceso(proceso, logger_kernel, tiempo_restante_exec);
        }
        else {
            liberar_cpu();
            log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <READY>", proceso->pid);
            proceso->quantum = quantum;
            pthread_mutex_unlock(&mutex_ready_list);
            list_add(pcbs_ready, proceso);
            mostrar_pids_ready(pcbs_ready, "READY");
            pthread_mutex_unlock(&mutex_ready_list);
            sem_post(&pcb_esperando_exec);
            ingresar_a_exec();
        }
}