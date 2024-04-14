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
            ejecutar_proceso(proceso);
        }
    }
}

void ejecutar_proceso(proceso_t* proceso) {

}