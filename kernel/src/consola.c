#include <../include/consola.h>
#include <readline/readline.h>


void empezar_hilo_consola(pthread_t* hilo_consola, t_log* logger, int socket) {
    leer_consola_t* args = malloc(sizeof(leer_consola_t));;
    args->logger = logger;
    args->socket = socket;
    pthread_create(hilo_consola, NULL, (void*) leer_consola, (void*) args);
}

void leer_consola(void* args_void)
{
    leer_consola_t* args = (leer_consola_t*) args_void;
    t_log* logger = args->logger;
    int socket = args->socket;
    char *leido;

    while (1) {
       leido = readline("> ");
       if (!strcmp(leido, "")) {
           break;
       }
       procesar_instruccion(leido, logger, socket);
       free(leido);
   }
   free(leido);
}

void procesar_instruccion(char* instruccion, t_log* logger, int socket) {

    //INICIAR_PROCESO DOCUMENTOS/prueba.txt
    
    char* comando = strtok(instruccion, " ");
    log_info(logger, comando);

    if(!strcmp(comando, "INICIAR_PROCESO")) {
        char* path = strtok(NULL, " ");
        enviar_inicio_proceso(socket, path, logger);
    }
    
    //le mandamos a la memoria --> send(socket, estructura con opcode y path, tamanio de struct, )
}

