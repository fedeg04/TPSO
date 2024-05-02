#include <../include/bloqueados.h>
void enviar_proceso_io_gen_sleep(proceso_t *proceso, char *interfaz_sleep, uint32_t uni_de_trabajo)
{
    if (!strcmp("GENERICA", interfaz_sleep))
    {
        /*
        if(estaConectada("GENERICA")) {
            proceso_sleep_t* proceso_a_sleep = malloc(sizeof(proceso_sleep_t));
            proceso_a_sleep->proceso = proceso;
            proceso_a_sleep->uni_de_trabajo = uni_de_trabajo;
            list_add(pcbs_generica, proceso_a_sleep);
            hacer_io_gen_sleep();
        }
        else {
            finalizar_proceso(proceso);
        }
        */
        pthread_mutex_lock(&mutex_generica_list);
        proceso_sleep_t* proceso_a_sleep = malloc(sizeof(proceso_sleep_t));
        proceso_a_sleep->proceso = proceso;
        proceso_a_sleep->uni_de_trabajo = uni_de_trabajo;
        list_add(pcbs_generica, proceso_a_sleep);
        pthread_mutex_unlock(&mutex_generica_list);
        sem_post(&pcb_esperando_generica);
        hacer_io_gen_sleep();
    }
    else
    {
        ingresar_a_exit(proceso);
        realizar_exit();
    }
}

int estaConectada(char *interfaz)
{
    return dictionary_has_key(diccionario_interfaces, interfaz);
}

void hacer_io_gen_sleep()
{
    sem_wait(&pcb_esperando_generica);
    if (estaConectada("GENERICA"))
    {
        pthread_mutex_lock(&mutex_generica_exec);
        pthread_mutex_lock(&mutex_generica_list);
        proceso_sleep_t *proceso_a_sleep = list_get(pcbs_generica, 0);
        pthread_mutex_unlock(&mutex_generica_list);
        void *stream = malloc(sizeof(op_code) + sizeof(uint32_t));
        int offset = 0;
        agregar_opcode(stream, &offset, IO_GEN_SLEEP);
        log_info(logger_kernel, "UNIS: %d", proceso_a_sleep->uni_de_trabajo);
        agregar_uint32_t(stream, &offset, proceso_a_sleep->uni_de_trabajo);
        int socket_generica = (int)dictionary_get(diccionario_interfaces, "GENERICA");
        send(socket_generica, stream, offset, 0);
        log_info(logger_kernel, "llegu{e ac{a");
        free(stream);
    }
    else
    {
        /*
        ingresar_a_exit(proceso_a_sleep->proceso);
        free(proceso_a_sleep);
        realizar_exit();
        */
    }
}

void recibir_fin_de_sleep()
{
    pthread_mutex_lock(&mutex_generica_list);
    proceso_sleep_t *proceso_de_sleep = list_remove(pcbs_generica, 0);
    pthread_mutex_unlock(&mutex_generica_list);
    pthread_mutex_unlock(&mutex_generica_exec);
    proceso_t *proceso_a_desbloquear = proceso_de_sleep->proceso;
    desbloquear_proceso(proceso_a_desbloquear);
    free(proceso_de_sleep);
}

void desbloquear_proceso(proceso_t *proceso)
{
    if (proceso->quantum == quantum)
    {
        pthread_mutex_unlock(&mutex_ready_list);
        list_add(pcbs_ready, proceso);
        pthread_mutex_unlock(&mutex_ready_list);
        sem_post(&pcb_esperando_exec);
        ingresar_a_exec();
    }
    else
    {
        pthread_mutex_unlock(&mutex_ready_prioritario_list);
        list_add(pcbs_ready_prioritarios, proceso);
        pthread_mutex_unlock(&mutex_ready_prioritario_list);
        sem_post(&pcb_esperando_exec);
        ingresar_a_exec();
    }

}