#include <../include/procesos.h>
#include <stdbool.h>
#include <stddef.h>

bool existe_archivo(char* path) {
    return fopen(path, "r") != NULL;
}