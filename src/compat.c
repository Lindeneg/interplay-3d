#include "compat.h"

FILE *open_file(const char *path) {
#if defined(_WIN32) || defined(WIN32)
    FILE *file;
    fopen_s(&file, path, "r");
    if (!file) {
        return NULL;
    }
    return file;
#else
    return fopen(path, "r");
#endif
}
