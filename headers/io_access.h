#ifndef IO_ACCESS
#define IO_ACCESS

#include <types.h>

static inline void outb(u16 port, u8 val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline u8 inb(u16 port)
{
    u8 ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

static inline void io_wait(void)
{
    outb(0x80, 0);
}

static inline void outw(u16 port, u16 data) {
    __asm__ volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}


#endif

