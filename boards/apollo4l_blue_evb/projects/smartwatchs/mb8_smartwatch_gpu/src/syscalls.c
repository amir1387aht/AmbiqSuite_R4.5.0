#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "am_util.h"

extern int __heap_start__;
extern int __heap_end__;
static char* heap_end = (char*)&__heap_start__;

int _write(int file, char *ptr, int len) {
    am_util_stdio_printf(ptr);
    return len;
}

int _read(int file, char *ptr, int len) {
    errno = ENOSYS;
    return -1;
}

int _sbrk(int incr) {
    char *prev_heap = heap_end;
    if ((heap_end + incr) > (char*)&__heap_end__) {
        errno = ENOMEM;
        return -1;
    }
    heap_end += incr;
    return (int)prev_heap;
}

int _close(int file) { return -1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _fstat(int file, struct stat *st) { st->st_mode = S_IFCHR; return 0; }
int _isatty(int file) { return 1; }
int _kill(int pid, int sig) { errno = EINVAL; return -1; }
int _getpid(void) { return 1; }
void _exit(int status) { while (1); }