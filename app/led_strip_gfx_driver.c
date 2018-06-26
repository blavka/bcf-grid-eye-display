#include <led_strip_gfx_drive.h>
#include <bc_led_strip.h>

bool is_ready(void *self)
{
    return bc_led_strip_is_ready((bc_led_strip_t *)self);
}

void clear(void *self)
{
    bc_led_strip_fill((bc_led_strip_t *)self, 0);
}

void draw_pixel_16x16(void *self, int left, int top, uint32_t color)
{
    int position = left * 16;

    if (left % 2 == 1)
    {
        position += 15 - top;
    }
    else
    {
        position += top;
    }

    bc_led_strip_set_pixel((bc_led_strip_t *)self, position , color << 8);
}

void draw_pixel_8x8(void *self, int left, int top, uint32_t color)
{
    int position = left * 8;

    if (left % 2 == 1)
    {
        position += 7 - top;
    }
    else
    {
        position += top;
    }

    bc_led_strip_set_pixel((bc_led_strip_t *)self, position , color << 8);
}

bool update(void *self)
{
    return bc_led_strip_write((bc_led_strip_t *)self);
}

bc_gfx_caps_t get_caps_16x16(void *self)
{
    (void) self;
    static bc_gfx_caps_t size = { .width = 16, .height = 16};
    return size;
}

bc_gfx_caps_t get_caps_8x8(void *self)
{
    (void) self;
    static bc_gfx_caps_t size = { .width = 8, .height = 8};
    return size;
}

const bc_gfx_driver_t led_strp_gfx_driver_16x16 = {
       .is_ready = is_ready,
       .clear = clear,
       .draw_pixel = draw_pixel_16x16,
       .get_pixel = NULL,
       .update = update,
       .get_caps = get_caps_16x16
};

const bc_gfx_driver_t led_strp_gfx_driver_8x8 = {
       .is_ready = is_ready,
       .clear = clear,
       .draw_pixel = draw_pixel_8x8,
       .get_pixel = NULL,
       .update = update,
       .get_caps = get_caps_8x8
};
