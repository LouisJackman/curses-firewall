#include "utils.h"

_Noreturn void vdief(char const fmt[], va_list args) {
    fputs("[X] ", stderr);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

_Noreturn void dief(char const fmt[], ...) {
    va_list args;
    va_start(args, fmt);
    vdief(fmt, args);
    va_end(args);
}

_Noreturn void die(char const msg[]) { dief("%s", msg); }

static FILE *get_log_file() {
    static FILE *file = NULL;
    static pthread_mutex_t mutex;

    if (file == NULL) {
        pthread_mutex_lock(&mutex);
        if (file == NULL) {
            file = fopen("log.txt", "w");
            if (file == NULL) {
                die("log file could not be written");
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    return file;
}

void vinformf(char const fmt[], va_list args) {
    FILE *file = get_log_file();
    fprintf(file, "[i] ");
    vfprintf(file, fmt, args);
    fputc('\n', file);
}

void informf(char const fmt[], ...) {
    va_list args;
    va_start(args, fmt);
    vinformf(fmt, args);
    va_end(args);
}

void inform(char const msg[]) { informf("%s", msg); }

