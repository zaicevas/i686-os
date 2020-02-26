#include <stddef.h>
#include <stdint.h>
 
#if defined(__linux__)
	#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
	#error "This code must be compiled with an x86-elf compiler"
#endif
 
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
const int VGA_COLS = 80;
const int VGA_ROWS = 25;
 
int term_col = 0;
int term_row = 0;
uint8_t term_color = 0x0F; // Black background, White foreground

void term_init() {
	// Clear the textmode buffer
	for (int col = 0; col < VGA_COLS; col ++) {
		for (int row = 0; row < VGA_ROWS; row ++) {
			// The VGA textmode buffer has size (VGA_COLS * VGA_ROWS).
			// Given this, we find an index into the buffer for our character
			const size_t index = (VGA_COLS * row) + col;
			// Entries in the VGA buffer take the binary form BBBBFFFFCCCCCCCC, where:
			// - B is the background color
			// - F is the foreground color
			// - C is the ASCII character
			vga_buffer[index] = ((uint16_t)term_color << 8) | ' '; // Set the character to blank (a space character)
		}
	}
}
 
// This function places a single character onto the screen
void term_putc(char c) {
	// Remember - we don't want to display ALL characters!
	switch (c)
	{
	case '\n': // Newline characters should return the column to 0, and increment the row
		{
			term_col = 0;
			term_row ++;
			break;
		}
 
	default: // Normal characters just get displayed and then increment the column
		{
			const size_t index = (VGA_COLS * term_row) + term_col; // Like before, calculate the buffer index
			vga_buffer[index] = ((uint16_t)term_color << 8) | c;
			term_col ++;
			break;
		}
	}
 
	// What happens if we get past the last column? We need to reset the column to 0, and increment the row to get to a new line
	if (term_col >= VGA_COLS) {
		term_col = 0;
		term_row ++;
	}
 
	// What happens if we get past the last row? We need to reset both column and row to 0 in order to loop back to the top of the screen
	if (term_row >= VGA_ROWS) {
		term_col = 0;
		term_row = 0;
	}
}
 
void term_print(const char* str) {
	for (size_t i = 0; str[i] != '\0'; i ++)
		term_putc(str[i]);
}
 
extern "C"
void kernel_main() {
		term_init();
	 
		term_print("Hello, World!\n");
		term_print("Welcome to the kernel.\n");
}
 
