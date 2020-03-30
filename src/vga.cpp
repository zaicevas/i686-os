#include <vga.h>
#include <string.h>
#include <stdint.h>
#include <debug.h>

#define BLACK_WHITE_TEXT 0x7 

// doesn't support scrolling, cursor
namespace vga {
	
	void init();
    void clear();
    static void kputc(const char c);
    void kprint(const char *s); 
    
    static terminal::canvas_t screen_canvas;

    void init(terminal::canvas_t screen_canvas) {
        vga::screen_canvas = screen_canvas;

        clear();
    }

    void clear() {
        uint8_t *video_memory = screen_canvas.framebuffer_addr;
        const uint8_t BYTES_PER_CHAR = 2;

        for (uint16_t i=0; i<screen_canvas.width * screen_canvas.height * BYTES_PER_CHAR; i+=BYTES_PER_CHAR) {
            video_memory[i] = ' ';
            video_memory[i+1] = BLACK_WHITE_TEXT; 		
        }
    }

    void kprint(const char *s) {
        for (uint32_t i=0; i<strlen(s); i++) {
            kputc(s[i]);
        }
    }

    static void kputc(const char c) {
        uint8_t *video_memory = screen_canvas.framebuffer_addr;
        uint16_t chars_x = terminal::get_chars_x();
        uint16_t chars_y = terminal::get_chars_y();

        uint16_t offset = chars_x * 2 + chars_y * screen_canvas.bytes_per_line;
        
        if (c == '\b') {
            if (chars_x != 0) 
                chars_x--;
        }
        else if (c == '\t') {
            chars_x = (chars_x + TAB_LENGTH) & ~(TAB_LENGTH - 1);
        }
        else if (c == '\r') {
            chars_x = 0;
        }
        else if (c == '\n') {
            chars_x = 0;
            terminal::set_chars_y(chars_y + 1);
        }	
        else {
            video_memory[offset] = c;
            video_memory[offset+1] = BLACK_WHITE_TEXT;
            terminal::set_chars_x(chars_x + 1);
        }

        if (chars_x >= screen_canvas.width) {
            chars_x = 0;
            terminal::set_chars_y(chars_y + 1);
        }

    }


}
