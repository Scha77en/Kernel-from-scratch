#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "types.h"

// System Call Numbers (EAX register)
#define SYS_READ    0
#define SYS_WRITE   1
#define SYS_REBOOT  2
#define SYS_GETPID  3
#define SYS_EXIT    4

#define NUM_SYSCALLS 5

// Register state passed from Assembly ISR 128 stub
typedef struct registers {
    u32 ds;                                     // Data segment selector
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pushad
    u32 int_no, err_code;                       // Interrupt number and error code
    u32 eip, cs, eflags, useresp, ss;           // Pushed by hardware automatically
} registers_t;

// API Prototypes
void syscall_init(void);
void syscall_handler(registers_t *regs);

// Syscall Base Implementation Prototypes
int  sys_read(int fd, char *buf, u32 count);
int  sys_write(int fd, const char *buf, u32 count);
void sys_reboot(void);
int  sys_getpid(void);
void sys_exit(int status);

#endif
