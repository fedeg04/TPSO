#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

typedef enum {
    SET = 1,
    MOV_IN,
    MOV_OUT,
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    EXIT,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS,
    FIN_DE_SLEEP,
    ENVIAR_PCB,
    INTERRUMPIR,
    FETCH,
    DATOS_PCB,
    GENERICA_BYE,
    STDIN_BYE,
    STDOUT_BYE,
    DIALFS_BYE,
    MSG
} op_code;

#endif