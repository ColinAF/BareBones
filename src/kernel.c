#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "string.h"
#include "terminal.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

// TODO
// - Add header comments for all files
// - Update the project structure using the meaty skeleton as inspo 
// - Add a timer 
// - Write the interrupt handlers 

terminal_t* terminal;

static void print_ascii(void)
{
	size_t i = 0; 
	char c = 'a';
	
	for(i = 0; i < 256; i++)
	{
		(c)++;
		terminal_write_string(terminal, &c);

	}

	return;
}

static void 
loop_on_string(const char* str)
{
	size_t i = 0;
	while(1)
	{
		terminal_write_string(terminal, str);
		for(i = 0; i < 50000000; i++); // Lame Delay 
	}
	
	return; 
}

void 
kernel_main(void) 
{
	terminal_initialize(terminal);
	terminal_write_string(terminal, "This is my os!\n");	
	print_ascii();
	loop_on_string("Joy to the world!\n");	
}







