#include <../include/planificador.h>

void planificar_nuevo_proceso(void *void_args)
{
    nuevo_proceso_t *args = (nuevo_proceso_t *)void_args;
    proceso_t *proceso = args->proceso;
    t_log* logger = args->logger;
    free(args);
    ingresar_a_new(proceso);
    ingresar_a_ready();
    ingresar_a_exec();
}

void ingresar_a_new(proceso_t *proceso)
{
    pthread_mutex_lock(&mutex_new_list);
    list_add(pcbs_new, (void *)proceso);
    pthread_mutex_unlock(&mutex_new_list);
    log_info(logger_kernel, "Se crea el proceso <%d> en NEW", proceso->pid);

    sem_post(&pcb_esperando_ready);
}

void ingresar_a_ready()
{
    sem_wait(&pcb_esperando_ready);
    sem_wait(&multiprogramacion);

    proceso_t *proceso = obtenerSiguienteAReady();
    log_info(logger_kernel, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", proceso->pid);
    pthread_mutex_lock(&mutex_ready_list);
    list_add(pcbs_ready, (void *)proceso);
    mostrar_pids_ready(pcbs_ready, "READY");
    pthread_mutex_unlock(&mutex_ready_list);
    sem_post(&pcb_esperando_exec);
}

void mostrar_pids_ready(t_list* ready_list, char* cola) {
    t_list* pids = list_map(ready_list, (void*) _get_pid);
    char* buffer = string_new();
    for(int i=0; i < list_size(ready_list); i++) {
        string_append(&buffer, string_itoa(list_get(pids, i)));
        string_append(&buffer, " ");
    }
    log_info(logger_kernel, "Cola Ready <%s>: %s", cola, buffer);
    free(buffer);
    list_destroy(pids);
}

uint32_t _get_pid(proceso_t* proceso) {
    return proceso->pid;
}

proceso_t *obtenerSiguienteAReady()
{
    pthread_mutex_lock(&mutex_new_list);
    proceso_t *pcb = list_remove(pcbs_new, 0);
    pthread_mutex_unlock(&mutex_new_list);
    return pcb;
}

void ingresar_a_exec()
{
    sem_wait(&pcb_esperando_exec);

    pthread_mutex_lock(&mutex_exec_list);
    proceso_t *proceso = obtenerSiguienteAExec();
    list_add(pcbs_exec, (void *)proceso);
    log_info(logger_kernel, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", proceso->pid);
    ejecutar_proceso(proceso, logger_kernel, proceso->quantum);
}

proceso_t *obtenerSiguienteAExec()
{
    proceso_t *pcb;
    if (!list_is_empty(pcbs_ready_prioritarios))
    {
        pthread_mutex_lock(&mutex_ready_prioritario_list);
        pcb = list_remove(pcbs_ready_prioritarios, 0);
        pthread_mutex_unlock(&mutex_ready_prioritario_list);
    }
    else
    {
        pthread_mutex_lock(&mutex_ready_list);
        pcb = list_remove(pcbs_ready, 0);
        pthread_mutex_unlock(&mutex_ready_list);
    }
    return pcb;
}

void liberar_cpu()
{
    list_remove(pcbs_exec, 0);
    pthread_mutex_unlock(&mutex_exec_list);
}

void ejecutar_proceso(proceso_t *proceso, t_log *logger, int unQuantum)
{
    t_temporal *timer = temporal_create();
    log_info(logger, "Algoritmo: %s", algoritmo_planificacion);
    if (!strcmp(algoritmo_planificacion, "FIFO"))
    {
        log_info(logger, "Se envia el proceso <%d> a CPU", proceso->pid);
        enviar_proceso_a_cpu(proceso, logger);
        esperar_llegada_de_proceso_fifo(proceso, logger);
        esperar_contexto_de_ejecucion(proceso, logger, timer, 0);
    }
    else if (!strcmp(algoritmo_planificacion, "RR") || !strcmp(algoritmo_planificacion, "VRR"))
    {
        enviar_proceso_a_cpu(proceso, logger);
        pthread_t hilo_interrupcion;
        interrupcion_proceso_t *args_interrupcion = malloc(sizeof(interrupcion_proceso_t));
        args_interrupcion->proceso = proceso;
        args_interrupcion->timer = timer;
        args_interrupcion->logger = logger;
        args_interrupcion->quantum = unQuantum;
        pthread_create(&hilo_interrupcion, NULL, (void *)manejar_interrupcion_de_timer, (void *)args_interrupcion);
        esperar_llegada_de_proceso_rr_vrr(proceso, timer, logger);
        pthread_detach(hilo_interrupcion);
    }
}

void enviar_proceso_a_cpu(proceso_t *proceso, t_log *logger)
{
    void *stream = malloc(sizeof(op_code) + 9 * sizeof(uint32_t) + 4 * sizeof(uint8_t));
    int offset = 0;
    agregar_pcb(stream, &offset, proceso);
    send(cpu_dispatch_fd, stream, offset, 0);
    free(stream);
}

void agregar_pcb(void *stream, int *offset, proceso_t *proceso)
{
    agregar_opcode(stream, offset, ENVIAR_PCB);
    agregar_uint32_t(stream, offset, proceso->pid);
    agregar_uint32_t(stream, offset, proceso->quantum);
    agregar_uint32_t(stream, offset, proceso->registros->PC);
    agregar_uint8_t(stream, offset, proceso->registros->AX);
    agregar_uint8_t(stream, offset, proceso->registros->BX);
    agregar_uint8_t(stream, offset, proceso->registros->CX);
    agregar_uint8_t(stream, offset, proceso->registros->DX);
    agregar_uint32_t(stream, offset, proceso->registros->EAX);
    agregar_uint32_t(stream, offset, proceso->registros->EBX);
    agregar_uint32_t(stream, offset, proceso->registros->ECX);
    agregar_uint32_t(stream, offset, proceso->registros->EDX);
    agregar_uint32_t(stream, offset, proceso->registros->SI);
    agregar_uint32_t(stream, offset, proceso->registros->DI);
}

void esperar_llegada_de_proceso_fifo(proceso_t *proceso, t_log *logger)
{
    uint32_t pid;
    uint32_t quantum;
    recv(cpu_dispatch_fd, &pid, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &quantum, sizeof(uint32_t), 0);
}

void manejar_interrupcion_de_timer(void *args_void)
{
    interrupcion_proceso_t *args = (interrupcion_proceso_t *)args_void;
    t_log *logger = args->logger;
    t_temporal *timer = args->timer;
    proceso_t *proceso = args->proceso;
    int unQuantum = args->quantum;
    free(args);
    usleep(unQuantum * 1000);
    if (timer != NULL && timer->status == TEMPORAL_STATUS_RUNNING)
    {
        temporal_stop(timer);
        log_info(logger_kernel, "PID: %d - Desalojado por fin de Quantum", proceso->pid);
        log_info(logger_kernel, "TIMER DE PROCESO DESALOJADO %d: %d", proceso->pid, temporal_gettime(timer));
        mandar_fin_de_quantum_de(proceso);
    }
}

void mandar_fin_de_quantum_de(proceso_t *proceso)
{
    void *stream = malloc(sizeof(op_code) + sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, INTERRUMPIR);
    agregar_uint32_t(stream, &offset, proceso->pid);
    send(cpu_interrupt_fd, stream, offset, 0);
    free(stream);
}

void esperar_llegada_de_proceso_rr_vrr(proceso_t *proceso, t_temporal *timer, t_log *logger)
{
    uint32_t pid;
    uint32_t quantum;
    recv(cpu_dispatch_fd, &pid, sizeof(uint32_t), 0);
    uint32_t tiempo_en_cpu = (uint32_t)temporal_gettime(timer);
    recv(cpu_dispatch_fd, &quantum, sizeof(uint32_t), 0);
    if (!strcmp(algoritmo_planificacion, "VRR"))
    {
        if (tiempo_en_cpu < proceso->quantum)
        {
            proceso->quantum -= tiempo_en_cpu;
        }
        else
        {
            proceso->quantum = quantum;
        }
    }
    esperar_contexto_de_ejecucion(proceso, logger, timer, tiempo_en_cpu);
}

void esperar_contexto_de_ejecucion(proceso_t *proceso, t_log *logger, t_temporal* timer, uint32_t tiempo_en_cpu)
{
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
    uint32_t size_motivo;
    char *motivo_de_desalojo;

    recv(cpu_dispatch_fd, &PC, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &AX, sizeof(uint8_t), 0);
    recv(cpu_dispatch_fd, &BX, sizeof(uint8_t), 0);
    recv(cpu_dispatch_fd, &CX, sizeof(uint8_t), 0);
    recv(cpu_dispatch_fd, &DX, sizeof(uint8_t), 0);
    recv(cpu_dispatch_fd, &EAX, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &EBX, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &ECX, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &EDX, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &SI, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &DI, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &size_motivo, sizeof(uint32_t), 0);
    motivo_de_desalojo = malloc(size_motivo);
    recv(cpu_dispatch_fd, motivo_de_desalojo, size_motivo, 0);

    proceso->registros->PC = PC;
    proceso->registros->AX = AX;
    proceso->registros->BX = BX;
    proceso->registros->CX = CX;
    proceso->registros->DX = DX;
    proceso->registros->EAX = EAX;
    proceso->registros->EBX = EBX;
    proceso->registros->ECX = ECX;
    proceso->registros->EDX = EDX;
    proceso->registros->SI = SI;
    proceso->registros->DI = DI;

    log_info(logger, "Motivo: %s", motivo_de_desalojo);
    char **substrings;
    char *instruccion_de_motivo_string;
    if (motivo_de_desalojo[strlen(motivo_de_desalojo) - 1] == '\n')
        motivo_de_desalojo[strlen(motivo_de_desalojo) - 1] = '\0';

    if (string_contains(motivo_de_desalojo, " "))
    {
        substrings = string_split(motivo_de_desalojo, " ");
        instruccion_de_motivo_string = substrings[0];
    }
    else
    {
        instruccion_de_motivo_string = motivo_de_desalojo;
    }

    op_code instruccion_de_motivo = string_to_opcode(instruccion_de_motivo_string);

    if(instruccion_de_motivo != WAIT && instruccion_de_motivo != SIGNAL) {
        temporal_stop(timer);
        liberar_cpu();    
    }


    log_info(logger, "Instruccion: %s", instruccion_de_motivo_string);
    proceso_a_interfaz_t* proceso_interfaz = malloc(sizeof(proceso_a_interfaz_t));
    proceso_interfaz->proceso = proceso;
    switch (instruccion_de_motivo)
    {
    case IO_GEN_SLEEP:
        char *interfaz_sleep = substrings[1];
        uint32_t uni_de_trabajo = atoi(substrings[2]);
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        log_info(logger_kernel, "PID: <%d> - Bloqueado por: <GENERICA>", proceso->pid);
        enviar_proceso_io_gen_sleep(proceso, interfaz_sleep, uni_de_trabajo);
        hacer_io_gen_sleep();
        break;
    case IO_STDIN_READ:
        proceso_interfaz->interfaz = substrings[1];
        proceso_interfaz->registro_direccion = atoi(substrings[2]);
        proceso_interfaz->registro_tamanio = atoi(substrings[3]);
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        log_info(logger_kernel, "PID: <PID> - Bloqueado por: <STDIN>");
        enviar_proceso_a_interfaz(proceso_interfaz, "STDIN", hacer_io_stdin_read);
        break;
    case IO_STDOUT_WRITE:
        proceso_interfaz->interfaz = substrings[1];
        proceso_interfaz->registro_direccion = atoi(substrings[2]);
        proceso_interfaz->registro_tamanio = atoi(substrings[3]);
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        log_info(logger_kernel, "PID: <PID> - Bloqueado por: <STDOUT>");
        enviar_proceso_a_interfaz(proceso_interfaz, "STDOUT", hacer_io_stdout_write);
        break;
    case IO_FS_CREATE:
        char *interfaz_create = substrings[1];
        char *nombre_archivo_create = substrings[2];
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        log_info(logger_kernel, "PID: <PID> - Bloqueado por: <DIALFS>");
        break;
    case IO_FS_DELETE:
        char *interfaz_delete = substrings[1];
        char *nombre_archivo_delete = substrings[2];
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        log_info(logger_kernel, "PID: <PID> - Bloqueado por: <DIALFS>");
        break;
    case IO_FS_TRUNCATE:
        char *interfaz_truncate = substrings[1];
        char *nombre_archivo_truncate = substrings[2];
        uint32_t registro_tamanio_truncate = atoi(substrings[3]);
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        log_info(logger_kernel, "PID: <PID> - Bloqueado por: <DIALFS>");
        break;
    case IO_FS_WRITE:
        char *interfaz_write = substrings[1];
        char *nombre_archivo_write = substrings[2];
        uint32_t registro_direccion_write = atoi(substrings[3]);
        uint32_t registro_tamanio_write = atoi(substrings[4]);
        // TODO: registro puntero archivo
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        log_info(logger_kernel, "PID: <PID> - Bloqueado por: <DIALFS>");
        break;
    case IO_FS_READ:
        char *interfaz_read = substrings[1];
        char *nombre_archivo_read = substrings[2];
        uint32_t registro_direccion_read = atoi(substrings[3]);
        uint32_t registro_tamanio_read = atoi(substrings[4]);
        // TODO: registro puntero archivo
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        log_info(logger_kernel, "PID: <PID> - Bloqueado por: <DIALFS>");
        break;
    case WAIT:
        char *recurso_wait = substrings[1];
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        enviar_proceso_a_wait(proceso, recurso_wait, tiempo_en_cpu, timer);
        break;
    case SIGNAL:
        char *recurso_signal = substrings[1];
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED>", proceso->pid);
        enviar_proceso_a_signal(proceso, recurso_signal);
        break;
    case EXIT:
        log_info(logger_kernel, "Finaliza el proceso %d - Motivo: SUCCESS", proceso->pid);
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso->pid);
        entrar_a_exit(proceso);
        break;
    case TIMER:
        log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <READY>", proceso->pid);
        proceso->quantum=  quantum;
        pthread_mutex_unlock(&mutex_ready_list);
        list_add(pcbs_ready, proceso);
        mostrar_pids_ready(pcbs_ready, "READY");
        pthread_mutex_unlock(&mutex_ready_list);
        sem_post(&pcb_esperando_exec);
        ingresar_a_exec();
    default:
    }
    if (string_contains(motivo_de_desalojo, " "))
    {
        string_array_destroy(substrings);
    }
    free(motivo_de_desalojo);
    free(proceso_interfaz);
    temporal_destroy(timer);
}



void finalizar_proceso(proceso_t *proceso){
    void *stream = malloc(sizeof(op_code) + sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, FINALIZAR_PROCESO);
    agregar_uint32_t(stream, &offset, proceso->pid);
    send(memoria_interrupt_fd, stream, offset, 0);
    free(stream);
    free(proceso->registros);
    free(proceso);
}

void ingresar_a_exit(proceso_t* proceso) {
    pthread_mutex_lock(&mutex_exit_queue);
	queue_push(pcbs_exit, (void *)proceso);
	pthread_mutex_unlock(&mutex_exit_queue);

    sem_post(&pcb_esperando_exit);
}

void realizar_exit() {
    sem_wait(&pcb_esperando_exit);

    pthread_mutex_lock(&mutex_exit_queue);
	proceso_t* proceso = queue_pop(pcbs_exit);
    finalizar_proceso(proceso);
	pthread_mutex_unlock(&mutex_exit_queue);
}

void entrar_a_cola_generica()
{
}

void entrar_a_cola_stdin()
{
}

void entrar_a_cola_stdout()
{
}

void entrar_a_cola_dialfs()
{
}

void entrar_a_cola_recurso()
{
}

void finalizar_proceso_de_pid(uint32_t pid_proceso) {
    // Habria que poner un semaforo que de mutua exclusion sobre pid_a_finalizar
    pid_a_finalizar = pid_proceso;
    buscar_en_cola_y_finalizar_proceso(pcbs_new, mutex_new_list);
    buscar_en_cola_y_finalizar_proceso(pcbs_ready, mutex_ready_list);
    buscar_en_cola_y_finalizar_proceso(pcbs_ready_prioritarios, mutex_ready_prioritario_list);
    buscar_en_cola_de_bloqueados_y_finalizar_proceso(pcbs_generica, mutex_generica_list, &fin_a_proceso_sleep);
    // faltan las demas interfaces y proceso en ejecucion
}

bool tiene_el_pid(proceso_t* proceso) {
    return pid_a_finalizar == proceso->pid;
}

void buscar_en_cola_y_finalizar_proceso(t_list* cola, pthread_mutex_t mutex)  {
    pthread_mutex_lock(&mutex);
    proceso_t* proceso =list_remove_by_condition(cola, (void*) tiene_el_pid);
    pthread_mutex_unlock(&mutex);
    if(proceso != NULL) {
    ingresar_a_exit(proceso); // revisar multiprogramacion para los que ya estan ready
    realizar_exit();
    }
}

bool tiene_el_pid_sleep(proceso_sleep_t* proceso_sleep){
    return pid_a_finalizar == proceso_sleep->proceso->pid;
}

void buscar_en_cola_de_bloqueados_y_finalizar_proceso(t_list* cola, pthread_mutex_t mutex, int* flag) {
    proceso_sleep_t* proceso_de_sleep;
     pthread_mutex_lock(&mutex);
    if(tiene_el_pid(list_get(cola, 0))) {
        //fin a proceso sleep debe tener su mutex
        *flag = 1;
        //wait a que el planificador avise que se lo puede eliminar
        *flag = 0;
        proceso_de_sleep = list_remove(pcbs_generica, 0);
    }
    else {
        proceso_de_sleep =list_remove_by_condition(cola, (void*) tiene_el_pid_sleep); 
    }
    pthread_mutex_unlock(&mutex);
    if(proceso_de_sleep != NULL) {
        proceso_t* proceso_a_exit= proceso_de_sleep->proceso;
        free(proceso_de_sleep);
        entrar_a_exit(proceso_a_exit);
    }
}

void cambiar_grado_de_multiprogramacion(int nuevo_grado_multiprogramacion) {
    if(nuevo_grado_multiprogramacion > grado_multiprogramacion) {
        for(int i = 0; i < nuevo_grado_multiprogramacion - grado_multiprogramacion; i++) {
            sem_post(&multiprogramacion);
        }
    }
    else {
        disminuciones_multiprogramacion = grado_multiprogramacion - nuevo_grado_multiprogramacion;
    }
    grado_multiprogramacion = nuevo_grado_multiprogramacion;
}

void entrar_a_exit(proceso_t* proceso) {
    if(disminuciones_multiprogramacion > 0) {
        disminuciones_multiprogramacion--; // hay que inicializar el valor
    }
    else {
        sem_post(&multiprogramacion);
    }
    ingresar_a_exit(proceso);
    realizar_exit();
}