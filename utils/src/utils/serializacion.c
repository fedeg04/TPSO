#include <../include/serializacion.h>

void enviar_paquete(op_code op_code, int socket, char* arg1) {
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = string_length(arg1)+1;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, arg1, string_length(arg1));


}