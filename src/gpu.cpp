#include <gpu.h>
#include <terminal.h>
#include <string.h>

#define RGB_DEPTH 24

using namespace terminal;

namespace gpu {

    struct color_scheme_t {
        uint8_t red_position;
        uint8_t green_position;
        uint8_t blue_position;
        uint8_t alpha_position;
    };

    static void putc(char c);
    static void draw_pixel(canvas_t canvas, uint32_t x, uint32_t y, const pixel_t pixel);
    static uint8_t get_alpha_byte_position(multiboot_framebuffer_color_info color_info);
    static color_scheme_t framebuffer_color_info_to_color_scheme(multiboot_framebuffer framebuffer);

    static const pixel_t WHITE = { 0xff, 0xff, 0xff };
    static const pixel_t BLACK = { 0x00, 0x00, 0x00 };
    static const pixel_t GRAY = { 190, 190, 190 };

    static color_scheme_t color_scheme = { 2, 1, 0, 3 }; // BGRA by default


    void init(multiboot_framebuffer framebuffer) {
        color_scheme = framebuffer_color_info_to_color_scheme(framebuffer);
    }

    void printf(const char *text) {
        for (uint32_t i=0; i<strlen(text); i++)
            putc(text[i]);
    }

    void clear() {
       ; 
    }

    static void draw_pixel(canvas_t canvas, uint32_t x, uint32_t y, const pixel_t pixel) {
        uint8_t *location = (uint8_t*) canvas.framebuffer_addr + (canvas.bytes_per_pixel * x) + (canvas.bytes_per_line * y);

        if (canvas.bytes_per_pixel > 3)
            location[color_scheme.alpha_position] = pixel.alpha;

        location[color_scheme.blue_position] = pixel.blue;
        location[color_scheme.green_position] = pixel.green;
        location[color_scheme.red_position] = pixel.red;
    }

    static void putc(char c) {
        const uint8_t *bmp = get_font(c);

        for (uint8_t width = 0; width < FONT_WIDTH; width++) {
            for (uint8_t height = 0; height < FONT_HEIGHT; height++) {
                uint8_t mask = 1 << (7 - width); // start with the most significant bit
                draw_pixel(screen_canvas, chars_x * FONT_WIDTH + width, chars_y * FONT_HEIGHT + height, bmp[height] & mask ? GRAY: BLACK);
            }
        }	

        chars_x++;

        uint32_t chars_per_line = screen_canvas.bytes_per_line / (screen_canvas.bytes_per_pixel * 8);

        if (chars_x >= chars_per_line) {
            chars_x = 0;
            chars_y++;
        }
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

    static color_scheme_t framebuffer_color_info_to_color_scheme(multiboot_framebuffer framebuffer) {
        multiboot_framebuffer_color_info color_info = framebuffer.color_info;

        return {
            color_info.framebuffer_red_field_position / 8,
            color_info.framebuffer_green_field_position / 8,
            color_info.framebuffer_blue_field_position / 8,
            framebuffer.framebuffer_bpp <= RGB_DEPTH ? -1 : get_alpha_byte_position(color_info) / 8
        };
    }

}