#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "ili9341-driver.h"
#include "hardware/spi.h"
#include "ili9341-display.h"
#include "ili9341-font.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"
#define ILI9341_PIN_MISO 4
#define ILI9341_PIN_CS 10
#define ILI9341_PIN_SCK 6
#define ILI9341_PIN_MOSI 7
#define ILI9341_PIN_DC 8
#define ILI9341_PIN_RESET 9
// #define PIN_LED -> 3.3V

static ili9341_display_t ili9341_display = {0};



typedef void (*ili9341_spi_write)(const uint8_t* data, uint32_t size);
typedef void (*ili9341_spi_read)(uint8_t* buffer, uint32_t length);
typedef void (*ili9341_gpio_cs_write)(bool level);
typedef void (*ili9341_gpio_dc_write)(bool level);
typedef void (*ili9341_gpio_reset_write)(bool level);
typedef void (*ili9341_delay_ms)(uint32_t ms);

void rp2040_gpio_cs_write(bool level)
{
	gpio_put(ILI9341_PIN_CS, level);
}

void rp2040_gpio_dc_write(bool level)
{
	gpio_put(ILI9341_PIN_DC, level);
}

void rp2040_gpio_reset_write(bool level)
{
	gpio_put(ILI9341_PIN_RESET, level);
}

void rp2040_delay_ms(uint32_t ms){
    sleep_ms(ms);
}


void rp2040_spi_write(const uint8_t *data, uint32_t size)
{
	spi_write_blocking(spi0, data, size);
}

void rp2040_spi_read(uint8_t *buffer, uint32_t length)
{
	spi_read_blocking(spi0, 0, buffer, length);
}

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_call(){
    led_task_state_set(LED_STATE_ON);
}

void led_off_call(){
    led_task_state_set(LED_STATE_OFF);
}

void led_blink_call(){
    led_task_state_set(LED_STATE_BLINK);
}

void led_period_call(char* args){
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);
    led_task_set_blink_period_ms(period_ms);
}

void mem_call(char* args){
    uint32_t* addr = 0;
    sscanf(args, "%X", &addr);
    printf("%u\n",*addr);
}

void wmem_call(char* args){
    uint32_t* addr=0;
    uint32_t val=0;
    sscanf(args, "%X %u", &addr, &val);
    *addr = val;
}

void disp_screen_callback(const char* args)
{
	uint32_t c = 0;
	int result = sscanf(args, "%x", &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 1)
	{
		color = RGB888_2_RGB565(c);
	}
	
	ili9341_fill_screen(&ili9341_display, color);
}

void disp_px_callback(const char* args)
{
	uint32_t c = 0;
    uint16_t x = 0;
    uint16_t y = 0;
	sscanf(args, "%hu %hu %x", &x, &y, &c);
	uint16_t color = RGB888_2_RGB565(c);
	ili9341_draw_pixel(&ili9341_display, x, y, color);

	
}

void disp_line_call(const char* args){
    uint32_t c = 0;
    uint16_t x0=0, y0=0, x1=0, y1=0;
    sscanf(args, "%hu %hu %hu %hu %x", &x0, &y0, &x1, &y1, &c);
    uint16_t color = RGB888_2_RGB565(c);
    ili9341_draw_line(&ili9341_display, x0, y0, x1, y1, color);
}

void disp_rect_call(const char* args){
    uint32_t c = 0;
    uint16_t x=0, y=0, w=0, h=0;
    sscanf(args, "%hu %hu %hu %hu %x", &x, &y, &w, &h, &c);
    uint16_t color = RGB888_2_RGB565(c);
    ili9341_draw_rect(&ili9341_display, x, y, w, h, color);
}

void disp_frect_call(const char* args){
    uint32_t c = 0;
    uint16_t x=0, y=0, w=0, h=0;
    sscanf(args, "%hu %hu %hu %hu %x", &x, &y, &w, &h, &c);
    uint16_t color = RGB888_2_RGB565(c);
    ili9341_draw_filled_rect(&ili9341_display, x, y, w, h, color);
}



api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
    {"on", led_on_call, "led power on"},
    {"off", led_off_call, "led power on"},
    {"blink", led_blink_call, "led blink"},
    {"set_period", led_period_call, "set period of blink"},
    {"mem", mem_call, "read from memory"},
    {"wmem", wmem_call, "write to memory"},
    {"disp_screen", disp_screen_callback, "fill display"},
    {"disp_px", disp_px_callback, "draw pixel"},
    {"disp_line", disp_line_call, "draw line"},
    {"disp_rect", disp_rect_call, "draw rect"},
    {"disp_frect", disp_frect_call, "draw frect"},
	{NULL, NULL, NULL},
};

int main()
{
    protocol_task_init(device_api);
    stdio_init_all();
    stdio_task_init();
    led_init();
    spi_init(spi0, 62500000);
    gpio_set_function(ILI9341_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_SCK, GPIO_FUNC_SPI);
    gpio_init(ILI9341_PIN_CS);
    gpio_init(ILI9341_PIN_DC);
    gpio_init(ILI9341_PIN_RESET);
    gpio_set_dir(ILI9341_PIN_CS, GPIO_OUT);
    gpio_set_dir(ILI9341_PIN_DC, GPIO_OUT);
    gpio_set_dir(ILI9341_PIN_RESET, GPIO_OUT);
    gpio_put(ILI9341_PIN_CS, 1);
    gpio_put(ILI9341_PIN_DC, 0);
    gpio_put(ILI9341_PIN_RESET, 0);
    ili9341_hal_t ili9341_hal = {0};
    ili9341_hal.spi_write = rp2040_spi_write;
    ili9341_hal.spi_read = rp2040_spi_read;
    ili9341_hal.gpio_cs_write = rp2040_gpio_cs_write;
    ili9341_hal.gpio_dc_write = rp2040_gpio_dc_write;

    ili9341_hal.gpio_reset_write = rp2040_gpio_reset_write;
    ili9341_hal.delay_ms = rp2040_delay_ms;

    ili9341_init(&ili9341_display, &ili9341_hal);
    ili9341_set_rotation(&ili9341_display, ILI9341_ROTATION_90);
    ili9341_fill_screen(&ili9341_display, COLOR_BLACK);
    sleep_ms(300);
    /* 2. Coloured rectangles */
    ili9341_draw_filled_rect(&ili9341_display, 10, 10, 100, 60, COLOR_RED);
    ili9341_draw_filled_rect(&ili9341_display, 120, 10, 100, 60, COLOR_GREEN);
    ili9341_draw_filled_rect(&ili9341_display, 230, 10, 80, 60, COLOR_BLUE);
    /* 3. Hollow rectangle outline */
    ili9341_draw_rect(&ili9341_display, 10, 90, 300, 80, COLOR_WHITE);
    /* 4. Diagonal lines */
    ili9341_draw_line(&ili9341_display, 0, 0, 319, 239, COLOR_YELLOW);
    ili9341_draw_line(&ili9341_display, 319, 0, 0, 239, COLOR_CYAN);
    ili9341_draw_text(&ili9341_display, 20, 100, "Hello, ILI9341!", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(&ili9341_display, 20, 116, "RP2040 / Pico SDK", &jetbrains_font, COLOR_YELLOW, COLOR_BLACK);
    while (1)
    {   
        protocol_task_handle(stdio_task_handle());
        led_handle();
    }
}
