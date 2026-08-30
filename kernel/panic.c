#include <panic.h>
#include <screen.h>

static panic_stack_snapshot_t panic_snapshot;

void save_panic_stack(void) {
    u32 current_esp, current_ebp;

    asm volatile ("movl %%esp, %0" : "=r"(current_esp));
    asm volatile ("movl %%ebp, %0" : "=r"(current_ebp));

    panic_snapshot.esp = current_esp;
    panic_snapshot.ebp = current_ebp;

    u8 *src = (u8 *)current_esp;
    for (u32 i = 0; i < PANIC_STACK_DUMP_SIZE; i++) {
        panic_snapshot.stack_raw[i] = src[i];
    }
}

void kernel_panic(const char *reason) {
    save_panic_stack();

    printk("\n*** KERNEL PANIC ***: %s\n", reason);
    printk("Captured ESP: 0x%x | Captured EBP: 0x%x\n", panic_snapshot.esp, panic_snapshot.ebp);

    panic_clear_registers_and_halt();
}
