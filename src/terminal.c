#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "terminal.h"
#include "string.h"

// TODO 
// - Do we need to reassign terminal_buffer everywhere
// - Add bounds checking
// - Get rid of the magic numbers 
// - Factor out a VGA driver
// - Add an opaque ref to the handler

static const size_t VGA_WIDTH = 80;   /**< The width of the VGA terminal. */
static const size_t VGA_HEIGHT = 25;  /**< The height of the VGA terminal. */

size_t terminal_row;                  /**< The current row in the terminal. */
size_t terminal_column;               /**< The current column in the terminal. */
uint8_t terminal_color;               /**< The current text color in the terminal. */
uint16_t* terminal_buffer;            /**< Pointer to the VGA text buffer. */


/**
 * @brief Combines foreground and background colors into a single 8-bit value.
 * @param fg Foreground color.
 * @param bg Background color.
 * @return The combined 8-bit color value.
 */
static inline uint8_t 
vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

/**
 * @brief Combines a character and a color into a single 16-bit VGA entry.
 * @param uc The character to display.
 * @param color The 8-bit color value.
 * @return The combined 16-bit VGA entry.
 */
static inline uint16_t 
vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

/**
 * @brief Initializes the terminal by clearing the screen and setting up default values.
 */
void 
terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;

	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

/**
 * @brief Scrolls the terminal up by one line, shifting the content up and clearing the bottom line.
 */
void
terminal_scroll_up(void)
{
	terminal_row--;
	terminal_column = 0;
	terminal_buffer = (uint16_t*) 0xB8000;

	for (size_t y = 0; y < (VGA_HEIGHT - 1); y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			size_t next_index = (y+1) * VGA_WIDTH + x;
			size_t current_index = (y) * VGA_WIDTH + x;
			terminal_buffer[current_index] = 
				vga_entry(terminal_buffer[next_index], terminal_color);
		}
	}
	return;
}

/**
 * @brief Clears the terminal screen by filling it with blank spaces.
 */
void
terminal_clear(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_buffer = (uint16_t*) 0xB8000;

	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}

	return;
}

/**
 * @brief Sets the terminal's current text color.
 * @param color The 8-bit color value to set.
 */
static void 
terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

/**
 * @brief Puts a character entry at a specific position on the screen.
 * @param c The character to put.
 * @param color The color to use for the character.
 * @param x The x-coordinate (column) of the character.
 * @param y The y-coordinate (row) of the character.
 */
static void 
terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

/**
 * @brief Outputs a character to the terminal, handling special cases like newlines.
 * @param c The character to output.
 */
void 
terminal_putchar(char c) 
{
	if(c == '\n')
	{
		terminal_column = 0; 
		terminal_row++;
		if (terminal_row == VGA_HEIGHT)
		{
			terminal_scroll_up();
		}

		return; 
	}

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	
	if(++terminal_column == VGA_WIDTH) 
	{
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
		{
			terminal_scroll_up();
		}
	}
}

/**
 * @brief Writes a sequence of characters to the terminal.
 * @param data The character data to write.
 * @param size The number of characters to write.
 */
static void 
terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
	{
		terminal_putchar(data[i]);
	}
}

/**
 * @brief Writes a null-terminated string to the terminal.
 * @param data The string to write.
 */
void 
terminal_write_string(const char* data) 
{
	terminal_write(data, strlen(data));
}

