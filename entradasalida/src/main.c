#include <stdlib.h>
#include <stdio.h>
#include <../include/init.h>
#include <../include/main.h>

void get_config(t_config* config)
 {
    tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    ip_kernel = config_get_string_value(config, "IP_KERNEL"); 
    if(!strcmp(tipo_interfaz, "GENERICA"))
    {
        tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");    
    }
    else if(!strcmp(tipo_interfaz, "STDIN") || !strcmp(tipo_interfaz, "STDOUT"))
    {
        puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
        ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    }
    else if(!strcmp(tipo_interfaz, "DIALFS"))
    {
    tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    path_base_dialfs = config_get_string_value(config, "PATH_BASE_DIALFS");
    block_size = config_get_int_value(config, "BLOCK_SIZE");
    block_count = config_get_int_value(config, "BLOCK_COUNT");
    retraso_compactacion= config_get_int_value(config, "RETRASO_COMPACTACION");
    }
}

int main(int argc, char* argv[]) {
    logger_io = iniciar_logger("io.log", "I/O: ");
    log_info(logger_io, "%s", argv[2]); // ./bin/entradasalida "nombre" "unpath"
    //nombre = argv[1];
    nombre = "int1";
    log_info(logger_io, "%s", nombre); 
    //t_config* config_io = iniciar_config(argv[2]);
    t_config* config_io = iniciar_config("dialfs.config");
    get_config(config_io);

    controlar_seniales(logger_io);
    // Se conecta al kernel
    kernel_fd = generar_conexion(logger_io, "kernel", ip_kernel, puerto_kernel, config_io);

    //Se conecta a la memoria
    if(!strcmp(tipo_interfaz, "DIALFS") || !strcmp(tipo_interfaz, "STDIN") || !strcmp(tipo_interfaz, "STDOUT")) 
    {
        memoria_fd = generar_conexion(logger_io, "memoria", ip_memoria, puerto_memoria, config_io);
    }

    conectar_a_kernel(nombre);
    atender_pedidos_kernel();

    terminar_programa(logger_io, config_io);
    liberar_conexion(memoria_fd);
    liberar_conexion(kernel_fd);
    return 0;
}

void conectar_a_kernel(char* nombre) {
  void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) + string_length(nombre) + 1);
  int offset = 0;
  agregar_opcode(stream, &offset, string_to_opcode(tipo_interfaz));
  agregar_string(stream, &offset, nombre);
  send(kernel_fd, stream, offset, 0);  
  free(stream);
}

void atender_pedidos_kernel() {

   if(!strcmp("GENERICA", tipo_interfaz)) {
    generica_atender_kernel();
   } 
   if(!strcmp("STDIN", tipo_interfaz)) {
    stdin_atender_kernel();
   } 
   if(!strcmp("STDOUT", tipo_interfaz)) {
    stdout_atender_kernel();
   } 
   if(!strcmp("DIALFS", tipo_interfaz)) {
    iniciar_fs();
    dialfs_atender_kernel();
   } 
}

void generica_atender_kernel() {
    while(1) {
        op_code opcode;
        recv(kernel_fd, &opcode, sizeof(op_code), 0);
        switch(opcode) {
            case IO_GEN_SLEEP:
            uint32_t unis_de_trabajo;
            recv(kernel_fd, &unis_de_trabajo, sizeof(uint32_t), 0);
            log_info(logger_io, "TIEMPO UNI: %d", tiempo_unidad_trabajo);
            log_info(logger_io, "UNIS: %d", unis_de_trabajo);
            log_info(logger_io, "TOTAL: %d", unis_de_trabajo * tiempo_unidad_trabajo);
            usleep(tiempo_unidad_trabajo * unis_de_trabajo * 1000);
            log_info(logger_io, "LLegue sleep");
            fin_de(FIN_DE_SLEEP);
                break;
            default:
        }
    }
}

void stdin_atender_kernel() {
    while(1) {
        op_code opcode;
        recv(kernel_fd, &opcode, sizeof(op_code), 0);
        switch(opcode) {
            case IO_STDIN_READ:
                uint32_t proceso_pid;
                uint32_t cant_paginas;
                uint32_t tamanio_string;
                recv(kernel_fd, &proceso_pid, sizeof(uint32_t), 0);
                recv(kernel_fd, &cant_paginas, sizeof(uint32_t), 0);
                recv(kernel_fd, &tamanio_string, sizeof(uint32_t), 0);
                char* direcciones_bytes = malloc(tamanio_string);
                recv(kernel_fd, direcciones_bytes, tamanio_string, 0);
                char* leido = readline("> ");
                enviar_pedido_stdin(proceso_pid, cant_paginas, direcciones_bytes, leido);
                for(int i=0; i < cant_paginas; i++) {
                    op_code op_code_recep;
                    recv(memoria_fd, &op_code_recep, sizeof(op_code), 0);
                }
                fin_de(FIN_DE_STDIN);
                break;
            default:
        }
    }
}

void stdout_atender_kernel() {
  while(1) {
        op_code opcode;
        recv(kernel_fd, &opcode, sizeof(op_code), 0);
        switch(opcode) {
            case IO_STDOUT_WRITE:
                uint32_t proceso_pid;
                uint32_t cant_paginas;
                uint32_t tamanio_string;
                recv(kernel_fd, &proceso_pid, sizeof(uint32_t), 0);
                recv(kernel_fd, &cant_paginas, sizeof(uint32_t), 0);
                recv(kernel_fd, &tamanio_string, sizeof(uint32_t), 0);
                char* direcciones_bytes = malloc(tamanio_string);
                recv(kernel_fd, direcciones_bytes, tamanio_string, 0);
                enviar_pedido_stdout(proceso_pid, cant_paginas, direcciones_bytes);
                char* resultado = string_new();
                for(int i = 0; i < cant_paginas; i++){
                    uint16_t tam;
                    char* string_recv;
                    recv(memoria_fd, &tam, sizeof(uint16_t), 0);
                    string_recv = malloc(tam + 1);
                    recv(memoria_fd, string_recv, tam, 0);
                    string_recv[tam] = '\0';
                    string_append(&resultado, string_recv);  
                    free(string_recv);
                }
                log_info(logger_io, "RESULTADO: %s", resultado);
                free(resultado);
                fin_de(FIN_DE_STDOUT);
                break;
            default:
        }
    }  
}

void dialfs_atender_kernel() {
     while(1) {
        op_code opcode;
        recv(kernel_fd, &opcode, sizeof(op_code), 0);
        uint32_t proceso_pid;
        recv(kernel_fd, &proceso_pid, sizeof(uint32_t), 0);
        uint32_t tam_archivo;
        recv(kernel_fd, &tam_archivo, sizeof(uint32_t), 0);
        char* nombre_arch = malloc(tam_archivo+1);
        recv(kernel_fd, nombre_arch, tam_archivo, 0);
        switch(opcode) {
            case IO_FS_CREATE:
                crear_archivo(nombre_arch);
                break;
            case IO_FS_DELETE:
                break;
            case IO_FS_TRUNCATE:
                uint32_t tamanio;
                recv(kernel_fd, &tamanio, sizeof(uint32_t), 0);
                truncar_archivo(nombre_arch, tamanio);
                break;
            case IO_FS_READ:
                uint32_t puntero_r;
                recv(kernel_fd, &puntero_r, sizeof(uint32_t), 0);
                uint32_t tam_archivo_dir_bytes_r;
                recv(kernel_fd, &tam_archivo_dir_bytes_r, sizeof(uint32_t), 0);
                char* dir_bytes_r;
                recv(kernel_fd, dir_bytes_r, tam_archivo_dir_bytes_r, 0);
                break;
            case IO_FS_WRITE:
                uint32_t puntero_w;
                recv(kernel_fd, &puntero_w, sizeof(uint32_t), 0);
                uint32_t tam_archivo_dir_bytes_w;
                recv(kernel_fd, &tam_archivo_dir_bytes_w, sizeof(uint32_t), 0);
                char* dir_bytes_w;
                recv(kernel_fd, dir_bytes_w, tam_archivo_dir_bytes_w, 0);
                break;
            default:
        }   
    }
}

void fin_de(op_code opcode) {
    void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) + string_length(nombre) + 1);
    int offset = 0;
    agregar_opcode(stream, &offset, opcode);
    agregar_string(stream, &offset, nombre);
    send(kernel_fd, stream, offset, 0);
    log_info(logger_io, "STREAM: %d", *((int*) stream));
    free(stream);
}

void enviar_pedido_stdout(uint32_t proceso_pid, uint32_t cant_paginas, char* direcciones_bytes) {
    char** substrings = string_split(direcciones_bytes, "-");
    uint16_t direccion;
    uint16_t bytes;
    for(int i = 0; i < cant_paginas * 2; i+=2) {
        void* stream = malloc(sizeof(op_code) + 3 * sizeof(uint32_t));
        int offset = 0;
        agregar_opcode(stream, &offset, LEER); // LEER en vez de IO_STDOUT_WRITE
        agregar_uint32_t(stream, &offset, proceso_pid);
        direccion = atoi(substrings[i]); 
        bytes = atoi(substrings[i+1]);
        agregar_uint16_t(stream, &offset, direccion); 
        agregar_uint16_t(stream, &offset, bytes);  
        send(memoria_fd, stream, offset, 0);
        free(stream);
     }   
    free(direcciones_bytes);
    string_array_destroy(substrings);
}

void enviar_pedido_stdin(uint32_t proceso_pid, uint32_t cant_paginas, char* direcciones_bytes, char* leido) {
    char** substrings = string_split(direcciones_bytes, "-");
    uint16_t direccion;
    uint32_t bytes;
    int offset_leido = 0;
    char* valor_a_enviar;
    for(int i = 0; i < cant_paginas * 2; i+=2) {
        direccion = atoi(substrings[i]); 
        bytes = atoi(substrings[i+1]);
        void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) + 2 * sizeof(uint16_t) + bytes);
        int offset = 0;
        agregar_opcode(stream, &offset, ESCRIBIR); // ESCRIBIR en vez de IO_STDIN_READ
        agregar_uint32_t(stream, &offset, proceso_pid);
        agregar_uint16_t(stream, &offset, direccion);
        valor_a_enviar = malloc(bytes + 1);
        memcpy(valor_a_enviar, leido + offset_leido, bytes);
        valor_a_enviar[bytes] = '\0';
        agregar_string_sin_barra0(stream, &offset, valor_a_enviar);
        send(memoria_fd, stream, offset, 0);
        free(stream);
        offset_leido+= bytes;
        free(valor_a_enviar);
    }
    free(leido);
    free(direcciones_bytes);
    string_array_destroy(substrings);
}

void iniciar_fs() {
    iniciar_bloques();
    iniciar_bitmap();
}

void iniciar_bloques() {
    char* path = string_new();
    string_append(&path, path_base_dialfs);
    string_append(&path, "/bloques.dat");
    f_bloques = fopen(path, "r+b"); 
    if(!f_bloques) {
        f_bloques = fopen(path, "w+b");
        int fd = fileno(f_bloques);
        ftruncate(fd, block_size*block_count);
    }
    fclose(f_bloques);
    free(path);
}

void iniciar_bitmap() {
    char* path = string_new();
    string_append(&path, path_base_dialfs);
    string_append(&path, "/bitmap.dat");
    f_bitmap = fopen(path, "r+b"); 
    if(!f_bitmap) {
        f_bitmap = fopen(path, "w+b");
        int fd = fileno(f_bitmap);
        ftruncate(fd, (block_count+7)/8);
        char *zero_buffer = calloc((block_count+7)/8, sizeof(char));
        fwrite(zero_buffer, sizeof(char), (block_count+7)/8, f_bitmap);
        free(zero_buffer);
    }
    fclose(f_bitmap);
    free(path);
}

void crear_archivo(char* nombre) {
    char* path = string_new();
    path_para_archivo(&path, nombre);
    FILE* f_metadata = fopen(path, "r+b");
    if (!f_metadata) {   
        f_metadata = fopen(path, "w+");
        int bloque_libre = buscar_y_setear_bloque_libre();
        char* bloque_libre_string = string_itoa(bloque_libre);
        if(bloque_libre != -1) { 
            char* metadata_contenido = string_new();
            string_append(&metadata_contenido, "BLOQUE_INICIAL=");
            string_append(&metadata_contenido, bloque_libre_string);
            string_append(&metadata_contenido, "\n");
            string_append(&metadata_contenido, "TAMANIO_ARCHIVO=0");
            fwrite(metadata_contenido, sizeof(char), strlen(metadata_contenido), f_metadata);
            free(bloque_libre_string);
            free(metadata_contenido);
        }
    }
    fclose(f_metadata);
    free(path);
}

void truncar_archivo(char* nombre, uint32_t tamanio_nuevo) {
    char* path = string_new();
    path_para_archivo(&path, nombre);
    t_config* config_metadata = config_create(path);
    int tamanio = config_get_int_value(config_metadata, "TAMANIO_ARCHIVO");
    int bloque_inicial = config_get_int_value(config_metadata, "BLOQUE_INICIAL");
    config_destroy(config_metadata);
    free(path);
    if(tamanio_archivo(nombre) == tamanio_nuevo) {}
    if(tamanio_archivo(nombre) > tamanio_nuevo) {
        cambiar_tamanio_archivo(nombre, tamanio_nuevo, tamanio, bloque_inicial, 0); //Achicar
    }
    if(tamanio_archivo(nombre) < tamanio_nuevo) {
        cambiar_tamanio_archivo(nombre, tamanio_nuevo, tamanio, bloque_inicial, 1); //Agrandar
    }
}

int tamanio_archivo(char* nombre) {
    char* path = string_new();
    path_para_archivo(&path, nombre);
    t_config* config_metadata = config_create(path);
    int tamanio = config_get_int_value(config_metadata, "TAMANIO_ARCHIVO");
    config_destroy(config_metadata);
    free(path);
    return tamanio;
}

void path_para_archivo(char** path, char* nombre) {
    string_append(path, path_base_dialfs);
    string_append(path, "/");
    string_append(path, nombre);
}

void cambiar_tamanio_archivo(char* nombre, int tamanio_nuevo, int tamanio, int bloque_inicial, int agrandar) {
    int cant_bloques_nuevos = cant_bloques_archivo(tamanio_nuevo);
    int cant_bloques_actuales = cant_bloques_archivo(tamanio);
    int primer_bloque = cant_bloques_nuevos + bloque_inicial;
    int ultimo_bloque = cant_bloques_actuales + bloque_inicial - 1;
    if (agrandar) {
        if (!puede_agrandar_sin_compactar(nombre, tamanio_nuevo, tamanio, bloque_inicial)) {
            bloque_inicial = compactar(nombre);
        }
        primer_bloque = cant_bloques_actuales + bloque_inicial;
        ultimo_bloque = cant_bloques_nuevos + cant_bloques_actuales + bloque_inicial -1;
    }
    cambiar_tamanio_metadata(nombre, tamanio_nuevo);
    t_bitarray* bitarray = setear_bitarray();
    for (int i = primer_bloque; i <= ultimo_bloque; i++) {
        if(agrandar) {
            bitarray_set_bit(bitarray, i);
        } else {
            bitarray_clean_bit(bitarray, i);
        }
    }
    fseek(f_bitmap, 0, SEEK_SET);
    fwrite(bitarray->bitarray, sizeof(char), tamanio_archivo, f_bitmap);
    free(bitarray->bitarray);
    fclose(f_bitmap);
    bitarray_destroy(bitarray);  
}

int cant_bloques_archivo(int tamanio) {
    return (tamanio + block_size) / block_size;
}

void cambiar_tamanio_metadata(char* nombre, int tamanio) {
    char* path = string_new();
    path_para_archivo(&path, nombre);
    t_config* config_metadata = config_create(path);
    config_set_value(config_metadata, "TAMANIO_ARCHIVO", tamanio);
    config_destroy(config_metadata);
    free(path);
}

void cambiar_bloque_inicial_metadata(char* nombre, int bloque_inicial) {
    char* path = string_new();
    path_para_archivo(&path, nombre);
    t_config* config_metadata = config_create(path);
    config_set_value(config_metadata, "BLOQUE_INICIAL", bloque_inicial);
    config_destroy(config_metadata);
    free(path);
}

bool puede_agrandar_sin_compactar(char* nombre, int tamanio_nuevo, int tamanio, int bloque_inicial) {
    int cant_bloques_nuevos = cant_bloques_archivo(tamanio_nuevo);
    int cant_bloques_actuales = cant_bloques_archivo(tamanio);
    int bloques_a_verificar = cant_bloques_nuevos - cant_bloques_actuales;
    int primer_bloque_a_verificar = bloque_inicial + cant_bloques_actuales;
    t_bitarray* bitarray = setear_bitarray();
    fclose(f_bitmap);
    for (int i = primer_bloque_a_verificar; i < primer_bloque_a_verificar + bloques_a_verificar; i++) {
        if (!bitarray_test_bit(bitarray, i)) {
            free(bitarray->bitarray);
            bitarray_destroy(bitarray);
            return false;
        }
    }
    free(bitarray->bitarray);
    bitarray_destroy(bitarray);
    return true;   
}

t_bitarray* setear_bitarray() {
    char* path = string_new();
    path_para_archivo(&path, "bitmap.dat");
    f_bitmap = fopen(path, "r+b");
    size_t tamanio_archivo = (block_count+7)/8;
    char* lectura_f_bloques = malloc(tamanio_archivo);
    fseek(f_bitmap, 0, SEEK_SET);
    fread(lectura_f_bloques, sizeof(char), tamanio_archivo, f_bitmap);
    t_bitarray* bitarray = bitarray_create(lectura_f_bloques,tamanio_archivo);
    free(path);
    return bitarray;
}

int compactar(char* nombre) {
    t_list* bloques_archivos = list_create();
    leer_txt_y_agregar_a_lista(bloques_archivos);
    
    bool mismo_nombre(bloques_archivo_t* bloques_archivo) {
        return bloques_archivo->nombre == nombre;
    }

    bloques_archivo_t* archivo_a_agrandar = list_remove_by_condition(bloques_archivos, (void *)mismo_nombre);
    list_add(bloques_archivos, archivo_a_agrandar);
    void* bloques = leer_bloques_dat();
    void* disco_aux = malloc(block_size*block_count);
    int offset_escritura = 0;
    int bloque_inicial;
    for (int i = 0; i < list_size(bloques_archivos); i++) {
        bloques_archivo_t* bloques_archivo = list_get(bloques_archivos, i);
        int comienzo_lectura = bloques_archivo->bloque_inicial*block_size;
        memcpy(disco_aux + offset_escritura, bloques + comienzo_lectura, bloques_archivo->tamanio);
        cambiar_bloque_inicial_metadata(bloques_archivo->nombre, offset_escritura/block_size);
        bloque_inicial = offset_escritura/block_size;
        offset_escritura += cant_bloques_archivo(bloques_archivo->tamanio)*block_size;
    }
    bloques = disco_aux;
    escribir_bloques_dat(bloques);
    free(bloques);
    list_destroy_and_destroy_elements(bloques_archivos, (void*)bloques_archivo_destroyer);
    return bloque_inicial;
}

void bloques_archivo_destroyer(bloques_archivo_t* bloques_archivo) {
    free(bloques_archivo->nombre);
    free(bloques_archivo);
}

void leer_txt_y_agregar_a_lista(t_list* bloques_archivos) {
    struct dirent *dir;
    DIR *dp = opendir(path_base_dialfs);
    while ((dir = readdir(dp))) {
        if (dir->d_type == DT_REG) {
            size_t len = strlen(dir->d_name);
            if (len > 4 && strcmp(dir->d_name + len - 4, ".txt") == 0) {
                bloques_archivo_t* bloques_archivo = malloc(sizeof(bloques_archivo_t));
                t_config* config_metadata = config_create(dir->d_name);
                int bloque_inicial = config_get_int_value(config_metadata, "BLOQUE_INICIAL");
                int tamanio = config_get_int_value(config_metadata, "TAMANIO");
                bloques_archivo->tamanio = tamanio;
                bloques_archivo->bloque_inicial = bloque_inicial;
                bloques_archivo->nombre = dir->d_name;
                list_add(bloques_archivos, bloques_archivo);
                config_destroy(config_metadata);
            }
        }
    }
    closedir(dp);
}

void* leer_bloques_dat() {
    char* path = string_new();
    path_para_archivo(&path, "bloques.dat");
    f_bloques = fopen(path, "r+b");
    size_t tamanio_archivo = block_count*block_size;
    void* lectura_f_bloques = malloc(tamanio_archivo);
    fseek(f_bloques, 0, SEEK_SET);
    fread(lectura_f_bloques, sizeof(char), tamanio_archivo, f_bloques);
    fclose(f_bloques);
    free(path);
    return lectura_f_bloques;
}

void escribir_bloques_dat(void* bloques) {
    char* path = string_new();
    path_para_archivo(&path, "bloques.dat");
    f_bloques = fopen(path, "r+b");
    size_t tamanio_archivo = block_count*block_size;
    fseek(f_bloques, 0, SEEK_SET);
    fwrite(bloques, sizeof(char), tamanio_archivo, f_bloques);
    fclose(f_bloques);
    free(path);
}