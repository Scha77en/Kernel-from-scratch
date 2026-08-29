#include "../headers/syscalls.h"
#include "../headers/screen.h"
#include "../headers/low_level.h"

// Function pointer type for internal dispatch handlers
typedef u32 (*syscall_fn_t)(u32 arg1, u32 arg2, u32 arg3);

static syscall_fn_t syscall_table[NUM_SYSCALLS];

/* --- System Call Implementations --- */

// SYS_READ (0): Stubbed until process/keyboard buffer queue is introduced
int sys_read(int fd, char *buf, u32 count) {
    if (fd != 0 || !buf) { // 0 = STDIN
        return -1;
    }
    // Returns 0 bytes read for now
    return 0;
}

// SYS_WRITE (1): Routes standard output/error to kernel screen driver
int sys_write(int fd, const char *buf, u32 count) {
    if ((fd != 1 && fd != 2) || !buf) { // 1 = STDOUT, 2 = STDERR
        return -1;
    }

    for (u32 i = 0; i < count; i++) {
        print_char(buf[i], WHITE_ON_BLACK);
    }
    return (int)count;
}

// SYS_REBOOT (2): Pulse CPU reset line via PS/2 Keyboard Controller (Port 0x64)
void sys_reboot(void) {
    printk("\n[KERNEL] Rebooting system...\n");
    
    // Disable interrupts to prevent IRQ interference during reset
    asm volatile ("cli");

    u8 good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE); // 0xFE command pulses CPU reset pin low

    // Fallback triple-fault if 8042 reset fails
    asm volatile ("lgdt 0");
    asm volatile ("int $3");
    
    while (1) {
        asm volatile ("hlt");
    }
}

// SYS_GETPID (3): Returns PID 1 until scheduler thread control blocks exist
int sys_getpid(void) {
    return 1;
}

// SYS_EXIT (4): Logs process termination and halts execution thread
void sys_exit(int status) {
    printk("\n[PROCESS] Exited with status code: %d\n", status);
    
    // Halt CPU loop until task switching is built
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
    // Verify syscall requested in EAX exists
    if (regs->eax >= NUM_SYSCALLS || !syscall_table[regs->eax]) {
        regs->eax = (u32)-1; // Return -1 on invalid syscall
        return;
    }

    syscall_fn_t location = syscall_table[regs->eax];
    
    // Dispatch call passing EBX, ECX, EDX as parameters; place result in EAX
    regs->eax = location(regs->ebx, regs->ecx, regs->edx);
}
