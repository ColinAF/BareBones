#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "terminal.h"
#include "string.h"

// TODO 
// - Factor out a VGA driver
// - Write a kprintf 

/* Hardware text mode color constants. */
enum vga_color 
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static const size_t VGA_WIDTH = 80;   /**< The width of the VGA terminal. */
static const size_t VGA_HEIGHT = 25;  /**< The height of the VGA terminal. */
static uint16_t* const terminal_buffer = (uint16_t*)0xB8000;  /**< Pointer to the VGA text buffer. */

/** 
 * @brief Terminal structure definition (opaque to other modules).
 */
// More elements can be added later such as a history buffer
struct terminal 
{
    size_t row;           /**< The current row in the terminal. */
    size_t column;        /**< The current column in the terminal. */
    uint8_t color;        /**< The current vga color entry for the terminal. */
};

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
 * @param term Pointer to the terminal handle.
 */
void 
terminal_initialize(terminal_t* term) 
{
	term->row = 0;
    term->column = 0;
    term->color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);

	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', term->color);
		}
	}

	return;
}

/**
 * @brief Scrolls the terminal up by one line, shifting the content up and clearing the bottom line.
 * @param term Pointer to the terminal handle.
 */
// Use memmove evntually
void
terminal_scroll_up(terminal_t* term)
{
	term->row--;
	term->column = 0;

	for (size_t y = 0; y < (VGA_HEIGHT - 1); y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			size_t next_index = (y+1) * VGA_WIDTH + x;
			size_t current_index = (y) * VGA_WIDTH + x;
			terminal_buffer[current_index] = 
				vga_entry(terminal_buffer[next_index], term->color);
		}
	}
	return;
}

/**
 * @brief Clears the terminal screen by filling it with blank spaces.
 * @param term Pointer to the terminal handle.
 */
// Use memset eventually
void
terminal_clear(terminal_t* term)
{
	term->row = 0;
	term->column = 0;

	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', term->color);
		}
	}

	return;
}

/**
 * @brief Sets the terminal's current text color.
 * @param term Pointer to the terminal handle.
 * @param color The 8-bit color value to set.
 */
static void 
terminal_setcolor(terminal_t* term, uint8_t color) 
{
	term->color = color;
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
 * @param term Pointer to the terminal handle.
 * @param c The character to output.
 */
void 
terminal_putchar(terminal_t* term, char c) 
{
	if(c == '\n')
	{
		term->column = 0; 
		term->row++;
		if (term->row == VGA_HEIGHT)
		{
			terminal_scroll_up(term);
		}

		return; 
	}

	terminal_putentryat(c, term->color, term->column, term->row);
	
	if(++term->column == VGA_WIDTH) 
	{
		term->column = 0;
		if (++term->row == VGA_HEIGHT)
		{
			terminal_scroll_up(term);
		}
	}

	return;
}

/**
 * @brief Writes a sequence of characters to the terminal.
 * @param term Pointer to the terminal handle.
 * @param data The character data to write.
 * @param size The number of characters to write.
 */
static void 
terminal_write(terminal_t* term, const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
	{
		terminal_putchar(term, data[i]);
	}

	return; 
}

/**
 * @brief Writes a null-terminated string to the terminal.
 * @param term Pointer to the terminal handle.
 * @param data The string to write.
 */
void 
terminal_write_string(terminal_t* term, const char* data) 
{
	terminal_write(term, data, strlen(data));
	return;
}

