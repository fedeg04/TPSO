#include <../include/registros.h>

void inicializar_listas() {
    pcbs_new = list_create();
    pcbs_ready = list_create();
    pcbs_exec = list_create();
    pcbs_ready_prioritarios = list_create();
    pcbs_generica = list_create();
    pcbs_stdin = list_create();
    pcbs_stdout = list_create();
    pcbs_dialfs = list_create();
    pcbs_recursos = malloc(cantidad_recursos *sizeof(t_list));
    for(int i = 0; i < cantidad_recursos; i++) 
    { 
        pcbs_recursos[i] = list_create();
    }
}

void liberar_listas() {
    list_destroy(pcbs_new);
    list_destroy(pcbs_ready);
    list_destroy(pcbs_ready_prioritarios);
    list_destroy(pcbs_exec);
    list_destroy(pcbs_generica);
    list_destroy(pcbs_stdin);
    list_destroy(pcbs_stdout);
    list_destroy(pcbs_dialfs);
    for(int i = 0; i < cantidad_recursos; i++) 
    { 
        list_destroy(pcbs_recursos[i]);
    }
    free(pcbs_recursos);
}
void get_config(t_config* config) {
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    quantum = config_get_int_value(config, "QUANTUM");
    grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    recursos = config_get_array_value(config, "RECURSOS");
    char** instancias_string = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    cantidad_recursos = cantidadDeRecursos(instancias_string);
    instancias_recursos = malloc(cantidad_recursos * sizeof(int));
    for(int i = 0; i < cantidad_recursos; i++) 
    { 
        instancias_recursos[i] = atoi(instancias_string[i]);
    }
    free(instancias_string);
}

    int cantidadDeRecursos(char** instancias_string)
    { 
        int cant = 0;
        while(instancias_string[cant] != NULL)
        { 
            cant++;
        }
        return cant;
    }