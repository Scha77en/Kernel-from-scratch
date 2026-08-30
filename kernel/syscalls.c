#include <syscalls.h>
#include <screen.h>
#include <low_level.h>

typedef u32 (*syscall_fn_t)(u32 arg1, u32 arg2, u32 arg3);

static syscall_fn_t syscall_table[NUM_SYSCALLS];


int sys_read(int fd, char *buf, u32 count) {
    if (fd != 0 || !buf) {
        return -1;
    }
    return 0;
}

int sys_write(int fd, const char *buf, u32 count) {
    if ((fd != 1 && fd != 2) || !buf) {
        return -1;
    }

    for (u32 i = 0; i < count; i++) {
        print_char(buf[i], WHITE_ON_BLACK);
    }
    return (int)count;
}

void sys_reboot(void) {
    printk("\n[KERNEL] Rebooting system...\n");

    asm volatile ("cli");

    u8 good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);

    asm volatile ("lgdt 0");
    asm volatile ("int $3");

    while (1) {
        asm volatile ("hlt");
    }
}

int sys_getpid(void) {
    return 1;
}

void sys_exit(int status) {
    printk("\n[PROCESS] Exited with status code: %d\n", status);

    asm volatile ("cli");
    while (1) {
        asm volatile ("hlt");
    }
}

/* --- Core Kernel Syscall Dispatcher --- */

void syscall_init(void) {
    syscall_table[SYS_READ]   = (syscall_fn_t)sys_read;
    syscall_table[SYS_WRITE]  = (syscall_fn_t)sys_write;
    syscall_table[SYS_REBOOT] = (syscall_fn_t)sys_reboot;
    syscall_table[SYS_GETPID] = (syscall_fn_t)sys_getpid;
    syscall_table[SYS_EXIT]   = (syscall_fn_t)sys_exit;
}

void syscall_handler(registers_t *regs) {
    if (regs->eax >= NUM_SYSCALLS || !syscall_table[regs->eax]) {
        regs->eax = (u32)-1;
        return;
    }

    syscall_fn_t location = syscall_table[regs->eax];

    regs->eax = location(regs->ebx, regs->ecx, regs->edx);
}
