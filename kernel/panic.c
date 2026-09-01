#include <panic.h>
#include <screen.h>
#include <symbols.h>

_bool   in_panic = false;

static panic_stack_snapshot_t panic_snapshot;

void print_registers(const char *msg) {

    u32 eax, ebx, ecx, edx, esi, edi, ebp, esp;
    asm volatile ("mov %%eax, %0" : "=r"(eax));
    asm volatile ("mov %%ebx, %0" : "=r"(ebx));
    asm volatile ("mov %%ecx, %0" : "=r"(ecx));
    asm volatile ("mov %%edx, %0" : "=r"(edx));
    asm volatile ("mov %%esi, %0" : "=r"(esi));
    asm volatile ("mov %%edi, %0" : "=r"(edi));
    asm volatile ("mov %%ebp, %0" : "=r"(ebp));
    asm volatile ("mov %%esp, %0" : "=r"(esp));

    printk("EAX=%08x EBX=%08x ECX=%08x EDX=%08x\n", eax, ebx, ecx, edx);
    printk("ESI=%08x EDI=%08x EBP=%08x ESP=%08x\n", esi, edi, ebp, esp);
}

const char *addr_to_symbol(u32 addr, u32 *offset) {
    const char *best_name = "???";
    u32 best_addr = 0;

    for (u32 i = 0; i < map_func_count; i++) {
        if (map_func[i].addr <= addr && map_func[i].addr > best_addr) {
            best_addr = map_func[i].addr;
            best_name = map_func[i].name;
        }
    }

    if (offset) *offset = addr - best_addr;
    return best_name;
}

void print_stack_trace(u32 max_frames) {
    u32 *ebp;
    asm volatile ("mov %%ebp, %0" : "=r"(ebp));

    printk("Stack trace:\n");
    for (u32 frame = 0; frame < max_frames && ebp; frame++) {
        u32 return_addr = ebp[1];
        if (return_addr == 0) break;

        u32 offset = 0;
        const char *name = addr_to_symbol(return_addr, &offset);

        printk("  [%d] 0x%x  %s+0x%x\n", frame, return_addr, name, offset);

        u32 *next_ebp = (u32 *)ebp[0];

        if (!next_ebp) break;

        if (next_ebp <= ebp) break;

        if (!strcmp(name, "main", 4)) break;

        ebp = next_ebp;
    }
}


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

void kernel_panic(int err_code) {
    in_panic = true;
    char *reason = "Unknown Exception";
    switch (err_code) {
        case 0:  reason = "Division by Zero (#DE)"; break;
        case 6:  reason = "Invalid Opcode (#UD)"; break;
        case 8:  reason = "Double Fault (#DF)"; break;
        case 13: reason = "General Protection Fault (#GP)"; break;
        case 14: reason = "Page Fault (#PF)"; break;
    }

    save_panic_stack();
    printk("\n*** KERNEL PANIC ***: %s (Vector %d)\n", reason, err_code);
    print_stack_trace(21);
    printk("Please force Reboot !");
    panic_clear_registers_and_halt();
}
