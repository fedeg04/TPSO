#include <../include/serializacion.h>
/*
void enviar_paquete(op_code op_code, int socket, char* arg1, t_log* logger) {
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = string_length(arg1)+1;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, arg1, string_length(arg1)+1);

    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->op_code = op_code;
    paquete->buffer = buffer;

    void* a_enviar = malloc(buffer->size + sizeof(op_code));
    int offset = 0;

    memcpy(a_enviar, &(paquete->op_code), sizeof(op_code));
    offset += sizeof(op_code);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, &(paquete->buffer->stream), paquete->buffer->size);

    log_info(logger, "Buffersize: %d\n sizeof: %d \n a_evniar: %s", buffer->size, sizeof(op_code), (char*) a_enviar);
    send(socket, a_enviar, buffer->size + sizeof(op_code), 0);
    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
*/

void enviar_paquete(op_code op_code, int socket, char* arg1, t_log* logger) {
    void* a_enviar = malloc(sizeof(op_code));
    memcpy(a_enviar, &op_code, sizeof(op_code));
    send(socket, a_enviar, sizeof(op_code), 0);

}