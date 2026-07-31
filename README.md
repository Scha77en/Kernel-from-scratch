# Kernel From Scratch

### What's in it:

- **VGA**: a screen driver to handle printing to the screen.

- **PS/2 Keyboard Driver**: a driver to handle the keyboard entries.

- **IDT (Interrupt Descriptor Table)**: a table containing CPU exceptions and hardware Interrupts.

- **GDT (Global Descriptor Table)**: The Global Descriptor Table (GDT) is a core part of Intel's x86 architecture that helps manage how memory is accessed and protected. it plays a key role in defining memory segments and their attributes: the base address, the size, and access privileges like executability and writability.

- **A POSIX Shell**: a small shell to help interacting with the kernel. type `help` in the command line for all the available commands.

- **Multi-screen implementation**:You can press "Control" key plus "Left" or "Right" arrows to switch between different screens up to 3.

- **A dedicated Cross compiler** `i386-elf-gcc` and its linker `i386-elf-ld`.

Other features were implemented like scrolling and cursor support etc..

### How to run the kernel iso in your computer:

You have to have qemu installed to run the iso provided in this repository.
```bash
qemu-system-i386 -cdrom kfs.iso -m 256M
```

### Makefile Commands:

```bash
make iso # Generates the iso file
make clean # Cleans the Object files
make fclean # Cleans the Object files and the iso
make run # Runs the kernel
```

### Note:
I haven't added any docker or environment setup for you to successfully run the make commands with no issues. I'll add that Soon.
If you want to test building the kernel in your locale computer, replace the Cross compiler in the Makefile `i386-elf-gcc` with `gcc` instead and `i386-elf-ld` with `ld` and try to install the tools you will need.
If you don't know what tools to install or needed just run `make iso && make run` and the missing tools will be reported to you to install.
