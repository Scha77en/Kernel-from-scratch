#ifndef PANIC_H
#define PANIC_H

#include <types.h>

#define PANIC_STACK_DUMP_SIZE 512

extern  _bool in_panic;

typedef struct {
    u32 ebp;
    u32 esp;
    u8  stack_raw[PANIC_STACK_DUMP_SIZE];
} panic_stack_snapshot_t;

void save_panic_stack(void);
void panic_clear_registers_and_halt(void) __attribute__((noreturn));
void kernel_panic(int err_code);
void print_stack_trace(u32 max_frames);

#endif

