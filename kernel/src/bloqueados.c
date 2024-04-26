#include <../include/bloqueados.h>
void enviar_proceso_io_gen_sleep(proceso_t* proceso,char* interfaz_sleep, uint32_t uni_de_trabajo) {
    if(!strcmp("GENERICA", interfaz_sleep)) {
        if(estaConectada("GENERICA")) {
            proceso_sleep_t* proceso_a_sleep = malloc(sizeof(proceso_a_sleep));
            proceso_a_sleep->proceso = proceso;
            proceso_a_sleep->uni_de_trabajo = uni_de_trabajo;
            list_add(pcbs_generica, proceso_a_sleep);
            hacer_io_gen_sleep();
        }
        else {
            finalizar_proceso(proceso);
        }
    }
    else {
        finalizar_proceso(proceso);
        }
    }

int estaConectada(char* interfaz) {
    return dictionary_has_key(diccionario_interfaces, interfaz);
}

void hacer_io_gen_sleep() {
    proceso_sleep_t* proceso_a_sleep = list_get(pcbs_generica, 0);
    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));
    int offset = 0;
    agregar_opcode(stream, &offset, IO_GEN_SLEEP);
    agregar_uint32_t(stream, &offset, proceso_a_sleep->uni_de_trabajo);
    int socket_generica = (int) dictionary_get(diccionario_interfaces, "GENERICA");
    send(socket_generica, stream, offset, 0);
}

void recibir_fin_de_sleep() {
    proceso_sleep_t* proceso_de_sleep = list_remove(pcbs_generica, 0);
    proceso_t* proceso_a_desbloquear = proceso_de_sleep->proceso;
    desbloquear_proceso(proceso_a_desbloquear);
    free(proceso_de_sleep);
}

void desbloquear_proceso(proceso_t* proceso) {
    if(proceso->quantum == quantum) {
        list_add(pcbs_ready, proceso);
    }
    else {
        list_add(pcbs_ready_prioritarios, proceso);
    }
}