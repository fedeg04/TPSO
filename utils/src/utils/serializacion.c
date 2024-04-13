#include <../include/serializacion.h>

void enviar_inicio_proceso(int socket, char* path, t_log* logger) {
    void* stream = malloc(sizeof(op_code) + string_length(path) + 1);
    int offset = 0;
    agregar_opcode(stream, &offset, INICIAR_PROCESO);
    agregar_string(stream, &offset, path);
    send(socket, stream, offset, 0);
    free(stream);
}
/*
void enviar_paquete(op_code op_code, int socket, char* arg1, t_log* logger) {
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = string_length(arg1)+1;
    buffer->stream = malloc(buffer->size);
    memcpy(buffer->stream, arg1, string_length(arg1)+1);


    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->op_code = op_code;
    paquete->buffer = buffer;

    void* a_enviar = malloc(buffer->size + sizeof(uint32_t) + sizeof(op_code));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->op_code), sizeof(op_code));
    offset += sizeof(op_code);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, (paquete->buffer->stream), paquete->buffer->size);

    send(socket, a_enviar, buffer->size + sizeof(op_code) + sizeof(uint32_t), 0);
    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
*/
void agregar_opcode(void* stream, int* offset, op_code op_code) {
    memcpy(stream, &op_code, sizeof(op_code));
    *offset += sizeof(op_code);

}

void agregar_uint32_t(void* stream, int* offset, uint32_t uint32) {
    memcpy(stream + (*offset), &uint32, sizeof(uint32_t));
    *offset += sizeof(uint32_t);
}  

void agregar_uint8_t(void* stream, int* offset, uint8_t uint8) {
    memcpy(stream + (*offset), &uint8, sizeof(uint8_t));
    *offset += sizeof(uint8_t);
} 

void agregar_string(void* stream, int* offset, char* string) {
    int32_t tamanio = string_length(string)+1;
    memcpy(stream + (*offset), &tamanio, sizeof(uint32_t));
    *offset += sizeof(uint32_t);
    memcpy(stream + (*offset), string, tamanio);
    *offset += tamanio;
} 
/*
void enviar_paquete(op_code op_code, int socket, char* arg1, t_log* logger) {
    void* a_enviar = malloc(sizeof(op_code));
    memcpy(a_enviar, &op_code, sizeof(op_code));
    send(socket, a_enviar, sizeof(op_code), 0);

} */