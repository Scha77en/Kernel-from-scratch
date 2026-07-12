#include "../headers/print_stack.h"

typedef struct s_stack_frame {
    struct s_stack_frame *ebp;
    u32 eip;
} t_stack_frame;


 char is_printable(u8 c){
	return (c >= 32 && c <= 126) ;
}

void print_stack_dump()
{
	u32 esp, ebp;

	asm volatile("movl %%esp, %0" : "=r"(esp));
	asm volatile("movl %%ebp, %0" : "=r"(ebp));
	u32 limit = ebp - esp;

	int line = 0;
	for (u32 i = 0; i < limit; i+=16 )
	{
		u32 addr = esp + i;
		print_char('[', WHITE_ON_BLACK);
		print_int(line++);
		print_string(" : [", WHITE_ON_BLACK);
		print_hex(addr);
		print_char(' ', WHITE_ON_BLACK);
		u8 * b = (u8 *)(addr);
		for(int j= 0; j < 16; j++){
			if (addr + j >= ebp) break;
			if (is_printable(b[j]))
				print_char(b[j], WHITE_ON_BLACK);
			else
				print_char('.', WHITE_ON_BLACK);
		}
		print_char('\n',WHITE_ON_BLACK);
	}

}
