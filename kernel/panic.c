#include <panic.h>
#include <screen.h>
#include <symbols.h>

static panic_stack_snapshot_t panic_snapshot;

__attribute__((noreturn))
void print_registers(const char *msg) {
    // asm volatile ("cli");

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
    // return;
}

// void print_stack_trace(u32 max_frames) {
//     u32 *ebp;
//     asm volatile ("mov %%ebp, %0" : "=r"(ebp));

//     printk("Stack trace:\n");
//     for (u32 frame = 0; frame < max_frames && ebp; frame++) {
//         u32 return_addr = ebp[1]; // saved return address
//         if (return_addr == 0) break;

//         printk("  [%d] 0x%x\n", frame, return_addr);

//         ebp = (u32 *)ebp[0]; // follow saved ebp (previous frame)
//     }
//     return;
// }

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
        u32 return_addr = ebp[1]; // saved return address
        if (return_addr == 0) break;

        u32 offset;
        const char *name = addr_to_symbol(return_addr, &offset);

        printk("  [%d] 0x%x  %s+0x%x\n", frame, return_addr, name, offset);

        ebp = (u32 *)ebp[0]; // follow saved ebp (previous frame)
    }
    return;
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

void kernel_panic(const char *reason) {
    save_panic_stack();

    printk("\n*** KERNEL PANIC ***: %s\n", reason);
    // print_registers(reason);
    // printk("Captured ESP: 0x%x | Captured EBP: 0x%x\n", panic_snapshot.esp, panic_snapshot.ebp);
    print_stack_trace(21);
    printk("Please Reboot !");
    panic_clear_registers_and_halt();
}
