#include <gpu.h>
#include <terminal.h>
#include <string.h>
#include <debug.h>
#include <font.h>

#define RGB_DEPTH 24

namespace gpu {

    struct color_scheme_t {
        uint8_t red_position;
        uint8_t green_position;
        uint8_t blue_position;
        uint8_t alpha_position;
    };

    void kputc(char c);
    void kprintf(const char *s, va_list args); 

    static void kputstr(char *s);
    static void kputint(int32_t i);
    static void kputhexdigit(uint8_t i);
    static void kputhex(uint32_t i);

    static void draw_pixel(terminal::canvas_t canvas, uint32_t x, uint32_t y, const pixel_t pixel);
    static uint8_t get_alpha_byte_position(multiboot_framebuffer_color_info color_info);
    static color_scheme_t framebuffer_color_info_to_color_scheme(multiboot_framebuffer framebuffer);

    static const pixel_t WHITE = { 0xff, 0xff, 0xff };
    static const pixel_t BLACK = { 0x00, 0x00, 0x00 };
    static const pixel_t GRAY = { 190, 190, 190 };

    static color_scheme_t color_scheme = { 2, 1, 0, 3 }; // BGRA by default
    static uint16_t chars_per_line = 0;

    static uint8_t font_width;
    static uint8_t font_height;

    void init(multiboot_framebuffer framebuffer) {
        color_scheme = framebuffer_color_info_to_color_scheme(framebuffer);
        font_width = font::get_font_width();
        font_height = font::get_font_height();
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
            terminal::set_chars_y(chars_y + 1);
            terminal::set_chars_x(0);
        }	
        else {
            const uint8_t *bmp = font::get_bitmap(c);

            for (uint8_t height = 0; height < font_height; height++) {
                for (uint8_t width = 0; width < font_width; width++) {	
                    uint8_t mask = 1 << 7 - (width % 8);    // start with the most significant bit
                    pixel_t pixel = bmp[height * ((font_width + 7) / 8) + width / 8] & mask ? GRAY : BLACK;
                    draw_pixel(terminal::get_screen_canvas(), chars_x * font_width + width, chars_y * font_height + height, pixel);
                }
            }

            terminal::set_chars_x(chars_x + 1);
        }

        uint32_t chars_per_line = terminal::get_screen_canvas().bytes_per_line / (terminal::get_screen_canvas().bytes_per_pixel * font_width);

        if (terminal::get_chars_x() >= chars_per_line) {
            terminal::set_chars_x(0);
            terminal::set_chars_y(chars_y + 1);
        }

    }

    inline static void kputstr(char *s) {
        for (uint32_t i=0; i<strlen(s); i++)
            kputc(s[i]);
    }

    inline static void draw_pixel(terminal::canvas_t canvas, uint32_t x, uint32_t y, const pixel_t pixel) {
        uint8_t *location = (uint8_t *) canvas.framebuffer_addr + (canvas.bytes_per_pixel * x) + (canvas.bytes_per_line * y);

        if (canvas.bytes_per_pixel > 3)
            location[color_scheme.alpha_position] = pixel.alpha;

        location[color_scheme.blue_position] = pixel.blue;
        location[color_scheme.green_position] = pixel.green;
        location[color_scheme.red_position] = pixel.red;
    }

    static uint8_t get_alpha_byte_position(multiboot_framebuffer_color_info color_info) {
        const uint8_t positions[] = {0, 8, 16, 24};

        const uint8_t given_positions[] = {
            color_info.framebuffer_blue_field_position, 
            color_info.framebuffer_green_field_position, 
            color_info.framebuffer_red_field_position
        };

        for (uint8_t i=0; i<4; i++) {
            bool exist = false;
            for (uint8_t j=0; j<3; j++) {
                if (positions[i] == given_positions[j])
                    exist = true;	
            }

            if (!exist)
                return positions[i] / 8;
        }

    }

    inline static color_scheme_t framebuffer_color_info_to_color_scheme(multiboot_framebuffer framebuffer) {
        multiboot_framebuffer_color_info color_info = framebuffer.color_info;

        return {
            color_info.framebuffer_red_field_position / 8,
            color_info.framebuffer_green_field_position / 8,
            color_info.framebuffer_blue_field_position / 8,
            framebuffer.framebuffer_bpp <= RGB_DEPTH ? -1 : get_alpha_byte_position(color_info) / 8
        };
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

}
