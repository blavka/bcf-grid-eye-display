#include <application.h>
#include <led_strip_gfx_drive.h>
#include <bc_base64.h>

#define APPLICATION_TASK_ID 0

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

uint8_t button_flag = 0;
uint8_t debug_col = 1;
uint8_t debug_row = 0;

uint8_t brightness = 32;
uint8_t display_temperature = 0;

int16_t sensorData[64];
float temperatures[64];

bc_module_infra_grid_t infra_grid;
bc_gfx_t gfx;

//the colors we will be using
const uint16_t camColors[] = {0x480F,
0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
0xF080,0xF060,0xF040,0xF020,0xF800,};

bc_led_strip_t led_strip;
static uint32_t _dma_buffer[(16 * 16) * BC_LED_STRIP_TYPE_RGB * 2];
const bc_led_strip_buffer_t _led_strip_buffer =
{
        .type = BC_LED_STRIP_TYPE_RGB,
        .count = (16 * 16),
        .buffer = _dma_buffer
};

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        //bc_led_set_mode(&led, BC_LED_MODE_TOGGLE);
        //button_flag = !button_flag;

        brightness >>= 1;

        if(brightness == 0)
        {
            brightness = 128;
        }

        bc_led_strip_set_brightness(&led_strip, brightness);
    }

    if(event == BC_BUTTON_EVENT_HOLD)
    {
        display_temperature = !display_temperature;
    }
}


void infra_grid_event_handler(bc_module_infra_grid_t *self, bc_module_infra_grid_event_t event, void *event_param)
{
    if (event == BC_MODULE_INFRA_GRID_EVENT_UPDATE)
    {
        bc_module_infra_grid_get_temperatures_celsius(self, temperatures);

        bc_scheduler_plan_now(APPLICATION_TASK_ID);
    }
}


void bc_radio_node_on_led_strip_brightness_set(uint64_t *id, uint8_t *brightness)
{
    (void) id;

    bc_led_strip_set_brightness(&led_strip, *brightness);
}

void photo_get(uint64_t *id, const char *topic, void *value, void *param)
{
    static uint16_t photo[64];
    static char str_base64[64];
    size_t str_length;;

    for (size_t i = 0; i < 64; i++)
    {
        photo[i] = (uint16_t) (temperatures[i] * 10.f);
    }

    str_length = sizeof(str_base64);

    bc_base64_encode(str_base64, &str_length, (uint8_t *) photo, 32);

    bc_radio_pub_string("photo0", str_base64);

    str_length = sizeof(str_base64);

    bc_base64_encode(str_base64, &str_length, ((uint8_t *) photo) + 32, 32);

    bc_radio_pub_string("photo1", str_base64);

    str_length = sizeof(str_base64);

    bc_base64_encode(str_base64, &str_length, ((uint8_t *) photo) + 64, 32);

    bc_radio_pub_string("photo2", str_base64);

    str_length = sizeof(str_base64);

    bc_base64_encode(str_base64, &str_length, ((uint8_t *) photo) + 92, 32);

    bc_radio_pub_string("photo3", str_base64);
}

void application_init(void)
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    bc_module_infra_grid_init(&infra_grid);
    bc_module_infra_grid_set_event_handler(&infra_grid, infra_grid_event_handler, NULL);
    bc_module_infra_grid_set_update_interval(&infra_grid, 100);

    bc_module_power_init();
    bc_led_strip_init(&led_strip, bc_module_power_get_led_strip_driver(), &_led_strip_buffer);
    bc_led_strip_set_brightness(&led_strip, brightness);

    #if LED_STRIP_DRIVER == LED_STRIP_DRIVER_8X8
    bc_gfx_init(&gfx, &led_strip, &led_strp_gfx_driver_8x8);
    #else
    bc_gfx_init(&gfx, &led_strip, &led_strp_gfx_driver_16x16);
    #endif
    // bc_gfx_set_rotation(&gfx, BC_GFX_ROTATION_90);
    bc_gfx_clear(&gfx);
    bc_gfx_update(&gfx);

    #if UART != 0
    bc_uart_init(BC_UART_UART2, BC_UART_BAUDRATE_115200, BC_UART_SETTING_8N1);
    bc_uart_write(BC_UART_UART2, "\r\n", 2);
    #endif

    // Initialize radio
    bc_radio_init(BC_RADIO_MODE_NODE_LISTENING);

    static bc_radio_sub_t subs[] = {
        {"infra-grid/-/photo/get", BC_RADIO_SUB_PT_NULL, photo_get, NULL},
    };

    bc_radio_set_subs((bc_radio_sub_t *) subs, sizeof(subs)/sizeof(bc_radio_sub_t));

    bc_radio_pairing_request("grid-eye-display", VERSION);
}

int32_t map_c(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
  int32_t val = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

    if (val > out_max)
    {
        val = out_max;
    }
    if (val < out_min)
    {
        val = out_min;
    }

  return val;
}

void application_task()
{
    uint8_t row;
    uint8_t col;
    float temperature;
    uint8_t map_index;
    uint16_t rgb565;
    uint32_t index_temp;
    float min_temperature = 20;
    float max_temperature = 24;

    for (int i = 0; i < 64; i++)
    {
        if (temperatures[i] > max_temperature)
        {
            max_temperature = temperatures[i];
        }
        if (min_temperature > temperatures[i])
        {
            min_temperature = temperatures[i];
        }
    }

#if LED_STRIP_DRIVER == LED_STRIP_DRIVER_8X8
    for (row = 0; row < 8; row++)
    {
        for (col = 0; col < 8; col++)
        {
            index_temp = row + col * 8;

            temperature = temperatures[index_temp];

#else
    for (row = 0; row < 16; row++)
    {
        for (col = 0; col < 16; col++)
        {

            index_temp = (row/2) + (col/2) * 8;

            if (((col % 2) == 0) && ((row % 2) == 0))
            {
                temperature = temperatures[index_temp];
            }
            else if (((row % 2) == 1) && ((col % 2) == 0))
            {
                temperature = (temperatures[index_temp] + temperatures[index_temp + 1]) / 2;
            }
                else
            {
                temperature = (temperatures[row / 2 + (col/2)*8] + temperatures[row / 2 + ((col+1)/2)*8]) / 2;
            }

            if (col == 15 || row == 15)
            {
                bc_gfx_draw_pixel(&gfx, col, row, 0);

                continue;
            }
#endif
            map_index = map_c(((uint32_t)temperature), min_temperature, max_temperature, 0, 255);

            rgb565 = camColors[map_index];

            uint8_t r = (rgb565 >> 8) & 0xF8;
            uint8_t g = (rgb565 >> 3) & 0xFC;
            uint8_t b = (rgb565 << 3) & 0xF8;

            if (button_flag && debug_col == col && debug_row == row)
            {
                r = 200;
                g = 0;
                b = 0;
            }

            uint32_t color = (uint32_t) r << 16 | (uint32_t) g << 8 | (uint32_t) b;

            bc_gfx_draw_pixel(&gfx, col, row, color);
        }
    }

    if (display_temperature)
    {
        char buff[8];
        uint32_t color = 128;
        snprintf(buff, sizeof(buff), "%2.0f", temperatures[52]);
        bc_gfx_draw_string(&gfx, 3, -2, buff, color<<16);

        bc_led_strip_set_pixel_rgbw(&led_strip, 12*16+7, 0, 0, color, 0);
    }

    bc_gfx_update(&gfx);

    #if UART != 0
    static char usb_str[512];
    static char str_buffer[16];

    snprintf(usb_str, sizeof(usb_str), "[\"%012llx/thermo/-/values\", [", bc_radio_get_my_id());

    uint32_t i;
    for (i = 0; i < 64; i++)
    {
        snprintf(str_buffer, sizeof(str_buffer), "%.1f", temperatures[i]);

        if(i != 63)
        {
            strncat(str_buffer, ",", sizeof(str_buffer));
        }

        strncat((char*)usb_str, str_buffer, sizeof(str_buffer));
    }
    strncat((char*)usb_str, "]]\n", sizeof(str_buffer));

    bc_uart_write(BC_UART_UART2, (char*)usb_str, strlen((const char*)usb_str));

    #endif
}

