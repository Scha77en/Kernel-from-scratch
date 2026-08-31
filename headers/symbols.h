#ifndef SYMBOLS_H
#define SYMBOLS_H
#include <types.h>
#include <signal.h>
#include <syscalls.h>

extern void print_stack_trace(u32 max_frames);
extern void divide_by_zero(void);
extern void isr_handler(struct registers *r);
extern void kernal_panic(const char * reason);
extern int main(void);
extern void interrupt_handler(u32 irq_num);
extern void*	isr_stub_table[];

struct symbol {
    u32 addr;
    const char *name;
};

struct symbol map_func[] = {
    {(u32)main,                       "main"},
    {(u32)kernel_panic,               "panic"},
    {(u32)print_stack_trace,          "print_stack_trace"},
    {(u32)divide_by_zero,             "divide_by_zero"},
    {(u32)signal_init,                "signal_init"},
    {(u32)signal_register,            "signal_register"},
    {(u32)signal_emit,                "signal_emit"},
    {(u32)schedule_signal,            "schedule_signal"},
    {(u32)dispatch_scheduled_signals, "dispatch_scheduled_signals"},
    {(u32)interrupt_handler,           "interrupt_handler"},
};
const u32 map_func_count = sizeof(map_func) / sizeof(map_func[0]);

extern struct symbol map_func[];
extern const u32 map_func_count;

#endif