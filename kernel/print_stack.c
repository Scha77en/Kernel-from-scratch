#include "../headers/print_stack.h"

typedef struct s_stack_frame {
    struct s_stack_frame *ebp;
    u32 eip;
} t_stack_frame;


 char is_printable(u8 c){
	return (c >= 32 && c <= 126) ;
}

u32 base_ebp = 0;

void print_stack_dump()
{
	u32 esp;
	asm volatile("movl %%esp, %0" : "=r"(esp));
	

	print_hex(base_ebp);
	print_char('\n', (BG_COLOR << 4) + FG_COLOR);
	u32 limit = base_ebp - esp;

	int line = 0;
	for (u32 i = 0; i < limit; i+=16 )
	{
		u32 addr = esp + i;
		print_char('[', (BG_COLOR << 4) + FG_COLOR);
		print_int(line++);
		print_string(" : [", (BG_COLOR << 4) + FG_COLOR);
		print_hex(addr);
		print_string(" ] ", (BG_COLOR << 4) + FG_COLOR);
		print_char(' ', (BG_COLOR << 4) + FG_COLOR);
		u8 * b = (u8 *)(addr);
		for(int j= 0; j < 16; j++){
			if (addr + j >= base_ebp) break;
			if (is_printable(b[j]))
				print_char(b[j], (BG_COLOR << 4) + FG_COLOR);
			else
				print_char('.', (BG_COLOR << 4) + FG_COLOR);
		}
		print_char('\n', (BG_COLOR << 4) + FG_COLOR);
	}

}
