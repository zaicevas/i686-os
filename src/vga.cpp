#include <vga.h>
#include <terminal.h>
#include <string.h>
#include <stdint.h>

#define BLACK_WHITE_TEXT 0x7 

using namespace terminal;

namespace vga {

    static void kputc(const char c);
    void clear();

    void init() {
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
        uint16_t offset = chars_x * 2 + chars_y * screen_canvas.bytes_per_line;
        
        if (c == BACKSPACE) {
            if (chars_x != 0) 
                chars_x--;
        }
        else if (c == TAB) {
            chars_x = (chars_x + TAB_LENGTH) & ~(TAB_LENGTH - 1);
        }
        else if (c == '\r') {
            chars_x = 0;
        }
        else if (c == '\n') {
            chars_x = 0;
            chars_y++;
        }	
        else {
            video_memory[offset] = c;
            video_memory[offset+1] = BLACK_WHITE_TEXT;
            chars_x++;
        }

        if (chars_x >= screen_canvas.width) {
            chars_x = 0;
            chars_y++;
        }

    }


}