#include <gpu.h>
#include <terminal.h>
#include <string.h>
#include <debug.h>
#include <font.h>
#include <system.h>

namespace gpu {

    void kputc(char c);
    void kprintf(const char *s, va_list args); 

    static void kputstr(char *s);
    static void kputint(int32_t i);
    static void kputhexdigit(uint8_t i);
    static void kputhex(uint32_t i);

    static void draw_pixel(terminal::canvas_t canvas, uint32_t x, uint32_t y, const pixel_t pixel);
    static void scroll_down();
    static void clear_line(uint8_t y);

    static const pixel_t WHITE = { 0xff, 0xff, 0xff };
    static const pixel_t BLACK = { 0x00, 0x00, 0x00 };
    static const pixel_t GRAY = { 190, 190, 190 };

    static uint8_t font_width;
    static uint8_t font_height;

    static uint32_t lines_per_screen;
    static uint32_t chars_per_line;

    static terminal::canvas_t screen_canvas;

    void init(terminal::canvas_t screen_canvas) {
        gpu::screen_canvas = screen_canvas;
        font_width = font::get_width();
        font_height = font::get_height();
        lines_per_screen = screen_canvas.height / font::get_height();
        chars_per_line = screen_canvas.bytes_per_line / (screen_canvas.bytes_per_pixel * font_width);
    }

    void clear() {
       ; 
    }

    // https://github.com/dthain/basekernel/blob/master/kernel/printf.c
    void kprintf(const char *s, va_list args) {
        uint32_t u;
        int32_t i;
        char *str;

        while (*s) {
            if (*s != '%') {
                kputc(*s);
            } else {
                s++;
                switch (*s) {
                case 'd':
                    i = va_arg(args, int32_t);
                    kputint(i);

                    break;
                case 'u':
                    u = va_arg(args, uint32_t);
                    kputstr(itoa(u));

                    break;
                case 'x':
                    u = va_arg(args, uint32_t);
                    kputhex(u);
                    break;
                case 's':
                    str = va_arg(args, char *);

                    kputstr(str);
                    break;
                case 'c':
                    u = va_arg(args, int32_t);
                    kputc(u);

                    break;
                case 0:
                    return;
                    break;
                default:
                    kputc(*s);
                    break;
                }
            }
            s++;
        }

        //swap_buffers();
    }

    void kputc(char c) {
        const uint16_t chars_x = terminal::get_chars_x();
        const uint16_t chars_y = terminal::get_chars_y();

        if (c == '\b') {
            if (chars_x != 0) {
                terminal::set_chars_x(chars_x - 1);
                kputc(' ');
                terminal::set_chars_x(chars_x - 1);
            }
        }
        else if (c == '\t') {
            terminal::set_chars_x((chars_x + TAB_LENGTH) & ~(TAB_LENGTH - 1));
        }
        else if (c == '\r') {
            terminal::set_chars_x(0);
        }
        else if (c == '\n') {
            if (chars_y < lines_per_screen)
                terminal::set_chars_y(chars_y + 1);
            terminal::set_chars_x(0);
        }	
        else {
            const uint8_t *bmp = font::get_bitmap(c);

            for (uint8_t height = 0; height < font_height; height++) {
                for (uint8_t width = 0; width < font_width; width++) {	
                    uint8_t mask = 1 << 7 - (width % 8);    // start with the most significant bit
                    pixel_t pixel = bmp[height * ((font_width + 7) / 8) + width / 8] & mask ? GRAY : BLACK;
                    draw_pixel(screen_canvas, chars_x * font_width + width, chars_y * font_height + height, pixel);
                }
            }

            terminal::set_chars_x(chars_x + 1);
        }

        if (terminal::get_chars_x() >= chars_per_line) {
            terminal::set_chars_x(0);
            terminal::set_chars_y(chars_y + 1);
        }

        if (terminal::get_chars_y() >= lines_per_screen) {
            scroll_down();
            //swap_buffers();
        }

    }

    static void scroll_down() {
        /* could be optimized by saving each line's last symbol position and
            checking for redundant coppies (empty lines, same letters)
        */

        uint8_t *framebuffer = screen_canvas.framebuffer_addr;
        uint32_t bytes_per_line = screen_canvas.bytes_per_line;

        memcpy(framebuffer, framebuffer + bytes_per_line * font_height, bytes_per_line * (lines_per_screen - 1) * font_height);

        clear_line(lines_per_screen - 1);

        terminal::set_chars_y(lines_per_screen - 1);
    }

    static void clear_line(uint8_t y) {
        for (uint16_t height = 0; height < font_height; height++) {
            for (uint16_t width = 0; width < screen_canvas.width; width++) {
                draw_pixel(screen_canvas, width, y * font_height + height, BLACK);
            }
        }
    }

    inline static void kputstr(char *s) {
        for (uint32_t i=0; i<strlen(s); i++)
            kputc(s[i]);
    }

    inline static void draw_pixel(terminal::canvas_t canvas, uint32_t x, uint32_t y, const pixel_t pixel) {
        uint8_t *location = (uint8_t *) canvas.framebuffer_addr + (canvas.bytes_per_pixel * x) + (canvas.bytes_per_line * y);

        if (canvas.bytes_per_pixel > 3)
            location[screen_canvas.color_scheme.alpha_position] = pixel.alpha;

        location[screen_canvas.color_scheme.blue_position] = pixel.blue;
        location[screen_canvas.color_scheme.green_position] = pixel.green;
        location[screen_canvas.color_scheme.red_position] = pixel.red;
    }


    static void kputint(int32_t i) {
        int f, d;

        if (i < 0 && i != 0) {
            kputc('-');
            i = -i;
        }

        f = 1;

        while((i / f) >= 10) {
            f *= 10;
        }

        while(f > 0) {
            d = i / f;
            kputc('0' + d);
            i = i - d * f;
            f = f / 10;
        }
    }

    inline static void kputhexdigit(uint8_t i) {
            if(i < 10) {
                kputc('0' + i);
            } else {
                kputc('a' + i - 10);
            }
    }

    inline static void kputhex(uint32_t i) {
        for(int j = 28; j >= 0; j = j - 4) {
            kputhexdigit((i >> j) & 0x0f);
        }
    }

    inline static void swap_buffers() {
        /*
        unsigned int visibleBytesPerLine = screen_canvas.bytes_per_line;
        uint8_t *dest = screen_canvas.framebuffer_addr;
        uint8_t *src = video_memory;

        for(int y = 0; y < screen_canvas.height; y++) {
            memcpy(dest, src, visibleBytesPerLine);
            dest += screen_canvas.bytes_per_line;
            src += visibleBytesPerLine;
        }
        */
    }

}

