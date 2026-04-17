/*
 * syscalls.c - Implementación FINAL de syscalls para Newlib-nano
 * Apache License 2.0
 * Compatible con usart.c (redirige printf a USART1 por defecto)
 */

#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <usart.h>

/* Símbolos del linker script */
extern char _sheap;
extern char _eheap;
static char *heap_end = &_sheap;

/* ====================== Syscalls mínimos ====================== */

int _fstat  (int fd, struct stat *st)      { (void)fd; (void)st; errno = ENOSYS; return -1; }
int _isatty (int fd)                       { (void)fd; return 1; }
int _close  (int fd)                       { (void)fd; return -1; }
int _lseek  (int fd, int ptr, int dir)     { (void)fd; (void)ptr; (void)dir; return 0; }
int _read   (int fd, char *ptr, int len)   { (void)fd; (void)ptr; (void)len; return 0; }


/* ====================== Syscalls básicos ====================== */

/* ====================== Heap (malloc) ====================== */
void *_sbrk(int incr)
{
    char *prev_heap_end = heap_end;
    if (heap_end + incr > &_eheap) {
        errno = ENOMEM;
        return (void*)-1;
    }
    heap_end += incr;
    return prev_heap_end;
}

/* ====================== _write (printf) ====================== */
/* Redirige printf a USART1 por defecto (igual que en usart.c) */
extern uint32_t usart_stdio_base;   /* declarado en usart.c */

int _write(int fd, char *ptr, int len)
{
    (void)fd;
    if (ptr == NULL || len <= 0) return 0;

    for (int i = 0; i < len; i++) {
        /* Usamos la función blocking de USART (definida en usart.c) */
        extern void usart_send_blocking(uint32_t usart_base, uint8_t data);
        usart_send_blocking(usart_stdio_base, (uint8_t)ptr[i]);
    }
    return len;
}

/* Versiones reentrantes que pide Newlib-nano */
int _fstat_r  (struct _reent *r, int fd, struct stat *st)      { (void)r; return _fstat(fd, st); }
int _isatty_r (struct _reent *r, int fd)                       { (void)r; return _isatty(fd); }
int _close_r  (struct _reent *r, int fd)                       { (void)r; return _close(fd); }
int _lseek_r  (struct _reent *r, int fd, int ptr, int dir)     { (void)r; return _lseek(fd, ptr, dir); }
int _read_r   (struct _reent *r, int fd, char *ptr, int len)   { (void)r; return _read(fd, ptr, len); }
void *_sbrk_r (struct _reent *r, int incr)                     { (void)r; return _sbrk(incr); }
int _write_r  (struct _reent *r, int fd, char *ptr, int len)   { (void)r; return _write(fd, ptr, len); }
