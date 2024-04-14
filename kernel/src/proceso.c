#include <../include/proceso.h>


proceso_t* crear_pcb(uint32_t pid) {
    proceso_t* nuevo_pcb = malloc(sizeof(proceso_t));
    nuevo_pcb->pid = pid;
    nuevo_pcb->quantum = (uint32_t) quantum;
    nuevo_pcb->registros = inicializar_registros();
    return nuevo_pcb;
}