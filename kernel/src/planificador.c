#include <../include/planificador.h>

void planificar_nuevo_proceso(proceso_t* proceso, t_log* logger)
{
    list_add(pcbs_new, proceso);
    log_info(logger, "Se crea el proceso <%d> en NEW", proceso->pid);
    if(procesos_activos < grado_multiprogramacion)
    {
        log_info(logger, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", proceso->pid);
        procesos_activos++;
        list_add(pcbs_ready,proceso);
        list_remove_element(pcbs_new, proceso);
        if(list_is_empty(pcbs_exec))
        {
            log_info(logger, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", proceso->pid);
            list_add(pcbs_exec, proceso);
            list_remove_element(pcbs_ready,proceso);
            ejecutar_proceso(proceso, logger);
        }
    }
}

void ejecutar_proceso(proceso_t* proceso, t_log* logger) {
    log_info(logger, "Algoritmo: %s", algoritmo_planificacion);
    if(!strcmp(algoritmo_planificacion, "FIFO"))
    {
        log_info(logger, "Se envia el proceso <%d> a CPU", proceso->pid);
        enviar_proceso_a_cpu(proceso);
    }
    else if(algoritmo_planificacion == "RR" || algoritmo_planificacion == "VRR")
    {
        enviar_proceso_a_cpu_con_timer(proceso);
    }
    esperar_contexto_de_ejecucion(proceso, logger);
}

void enviar_proceso_a_cpu(proceso_t* proceso)
{
    void* stream = malloc(sizeof(op_code) + 9 * sizeof(uint32_t) + 4 * sizeof(uint8_t));
    int offset = 0;
    agregar_opcode(stream, &offset, ENVIAR_PCB);
    agregar_uint32_t(stream, &offset, proceso->pid);
    agregar_uint32_t(stream, &offset, proceso->quantum);
    agregar_uint32_t(stream, &offset, proceso->registros->PC);
    agregar_uint8_t(stream, &offset, proceso->registros->AX);
    agregar_uint8_t(stream, &offset, proceso->registros->BX);
    agregar_uint8_t(stream, &offset, proceso->registros->CX);
    agregar_uint8_t(stream, &offset, proceso->registros->DX);
    agregar_uint32_t(stream, &offset, proceso->registros->EAX);
    agregar_uint32_t(stream, &offset, proceso->registros->EBX);
    agregar_uint32_t(stream, &offset, proceso->registros->ECX);
    agregar_uint32_t(stream, &offset, proceso->registros->EDX);
    agregar_uint32_t(stream, &offset, proceso->registros->SI);
    agregar_uint32_t(stream, &offset, proceso->registros->DI);
    send(cpu_dispatch_fd, stream, offset, 0);
}

void agregar_pcb(void* stream, int* offset, proceso_t* proceso)
{
    agregar_opcode(stream, &offset, ENVIAR_PCB);
    agregar_uint32_t(stream, &offset, proceso->pid);
    agregar_uint32_t(stream, &offset, proceso->quantum);
    agregar_uint32_t(stream, &offset, proceso->registros->PC);
    agregar_uint8_t(stream, &offset, proceso->registros->AX);
    agregar_uint8_t(stream, &offset, proceso->registros->BX);
    agregar_uint8_t(stream, &offset, proceso->registros->CX);
    agregar_uint8_t(stream, &offset, proceso->registros->DX);
    agregar_uint32_t(stream, &offset, proceso->registros->EAX);
    agregar_uint32_t(stream, &offset, proceso->registros->EBX);
    agregar_uint32_t(stream, &offset, proceso->registros->ECX);
    agregar_uint32_t(stream, &offset, proceso->registros->EDX);
    agregar_uint32_t(stream, &offset, proceso->registros->SI);
    agregar_uint32_t(stream, &offset, proceso->registros->DI);
}

void enviar_proceso_a_cpu_con_timer(proceso_t* proceso) {

}

void esperar_contexto_de_ejecucion(proceso_t* proceso, t_log* logger)
{
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
    uint32_t size_motivo;
    char* motivo_de_desalojo;

    recv(cpu_dispatch_fd, &pid, sizeof(uint32_t), 0);
    recv(cpu_dispatch_fd, &quantum, sizeof(uint32_t), 0);
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

    char** substrings = string_split(motivo_de_desalojo, " ");
    char* instruccion_de_motivo_string = substrings[0];
    op_code instruccion_de_motivo = string_to_opcode(instruccion_de_motivo_string);

    switch(instruccion_de_motivo){
            case IO_GEN_SLEEP:
                char* interfaz_sleep = substrings[1];
                uint32_t uni_de_trabajo = atoi(substrings[2]);
                break;
            case IO_STDIN_READ:
                char* interfaz_stdin = substrings[1];
                uint32_t registro_direccion_stdin = atoi(substrings[2]);
                uint32_t registro_tamanio_stdin = atoi(substrings[3]);
                break;
            case IO_STDOUT_WRITE:
                char* interfaz_stdout = substrings[1];
                uint32_t registro_direccion_stdout = atoi(substrings[2]);
                uint32_t registro_tamanio_stdout = atoi(substrings[3]);
                break;
            case IO_FS_CREATE:
                char* interfaz_create = substrings[1];
                char* nombre_archivo_create = substrings[2];
                break;
            case IO_FS_DELETE:
                char* interfaz_delete = substrings[1];
                char* nombre_archivo_delete = substrings[2];
                break;
            case IO_FS_TRUNCATE:
                char* interfaz_truncate = substrings[1];
                char* nombre_archivo_truncate = substrings[2];
                uint32_t registro_tamanio_truncate = atoi(substrings[3]);
                break;
            case IO_FS_WRITE:
                char* interfaz_write = ssubstrings[1];
                char* nombre_archivo_write = substrings[2];
                uint32_t registro_direccion_write = atoi(substrings[3]);
                uint32_t registro_tamanio_write = atoi(substrings[4]);
                // TODO: registro puntero archivo
            case IO_FS_READ:
                char* interfaz_read = substrings[1];
                char* nombre_archivo_read = substrings[2];
                uint32_t registro_direccion_read = atoi(substrings[3]);
                uint32_t registro_tamanio_read = atoi(substrings[4]);
                // TODO: registro puntero archivo
            case WAIT:
                char* recurso_wait = substrings[1];
                break;
            case SIGNAL:
                char* recurso_signal = substrings[1];
                break;
            case EXIT:
            break;
            //case TIMER:
            default:
    }

}