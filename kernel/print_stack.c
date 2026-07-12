#include "../headers/print_stack.h"

typedef struct s_stack_frame {
    struct s_stack_frame *ebp;
    u32 eip;
} t_stack_frame;

void print_stack(u32 max_frames)
{
    t_stack_frame *frame;
    u32 i;

    asm volatile("movl %%ebp, %0" : "=r"(frame));

    print_string((s8 *)"Stack trace:\n", 0x0F);

    i = 0;
    while (frame != 0 && i < max_frames)
    {
        print_string((s8 *)"  #", 0x0F);
        print_int(i);
        print_string((s8 *)"  eip=", 0x0F);
        print_hex(frame->eip);
        print_char('\n', 0x0F);

        if ((u32)frame->ebp <= (u32)frame || (u32)frame->ebp < 0x1000)
            break;

        frame = frame->ebp;
        i++;
    }
}
