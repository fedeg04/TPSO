#include <../include/sockets.h>

int iniciar_servidor(t_log* logger, char* puerto)
{
   int socket_servidor;
   struct addrinfo hints, *servinfo, *p;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   getaddrinfo(NULL, puerto, &hints, &servinfo);

   socket_servidor = socket(servinfo->ai_family,
                            servinfo->ai_socktype,
                            servinfo->ai_protocol);
   bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
   listen(socket_servidor, SOMAXCONN);
   freeaddrinfo(servinfo);
   log_info(logger, "Servidor a la espera de un cliente!");
   return socket_servidor;
}

int server_escuchar(int socket_server, t_log* logger) {

    int socket_cliente = esperar_cliente(socket_server, logger);
    if(socket_cliente != -1) {

        //TODO: agregarle hilos probablemente, y hacer procesar conexión en todos los módulos server (protocolo de cada uno)
        //procesar_conexion(logger, socket_cliente);
    }
}

int esperar_cliente(int socket_servidor, t_log* logger) {
   int socket_cliente = accept(socket_servidor, NULL, NULL);
   log_info(logger, "Se conecto un cliente!");
   return socket_cliente;
}

void liberar_conexion(int socket_cliente) {
   close(socket_cliente);
}

int crear_conexion(t_log* logger, const char* nombre_server, char* ip, char* puerto) {
    struct addrinfo hints, *servinfo;

    // Init de hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    // Crea un socket con la informacion recibida (del primero, suficiente)
    int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    // Fallo en crear el socket
    if(socket_cliente == -1) {
        log_error(logger, "Error creando el socket para %s:%s", ip, puerto);
        return 0;
    }

    // Error conectando
    if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        log_error(logger, "Error al conectar (a %s)\n", nombre_server);
        freeaddrinfo(servinfo);
        return 0;
    } else
        log_info(logger, "Cliente conectado en %s:%s (a %s)\n", ip, puerto, nombre_server);

    freeaddrinfo(servinfo);

    return socket_cliente;
}