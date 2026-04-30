#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task.h"
#include "bme-280.h"
#include "led-task/led-task.h"
#include "hardware/i2c.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
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

void read_reg_call(char* args){
    uint32_t addr=0;
    uint32_t len=0;
    sscanf(args, "%X %u", &addr, &len);
    uint8_t buffer[256] = {0};
    if((addr <= 0xFF)&&(len <= 0xFF)&&(addr + len <= 0x100)){
        bme280_read_regs(addr, buffer, len);
        for (int i = 0; i < len; i++)
        {
            printf("bme280 register [0x%X] = 0x%X\n", addr + i, buffer[i]);
        }
    }
    else{
        printf("error");
    }
}

void write_reg_call(char* args){
    uint32_t addr=0;
    uint32_t val=0;
    sscanf(args, "%X %X", &addr, &val);
    if((addr <= 0xFF)&&(val <= 0xFF)){
        bme280_write_reg(addr, val);
    }
    else{
        printf("error");
    }
}

void temp_call(char* args){
    uint32_t res = bme280_read_temp_raw();
    printf("%u\n", res);
}
void pres_call(char* args){
    uint32_t res = bme280_read_pres_raw();
    printf("%u\n", res);
}
void hum_call(char* args){
    uint32_t res = bme280_read_hum_raw();
    printf("%u\n", res);
}
void temp_si_call(char* args){
    double res = bme280_read_temp_si();
    printf("%f\n", res);
}
void pres_si_call(char* args){
    double res = bme280_read_pres_si();
    printf("%f\n", res);
}
void hum_si_call(char* args){
    double res = bme280_read_hum_si();
    //res = bme280_compensate_H_int32(res);
    printf("%f\n", res);
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
    {"read_reg", read_reg_call, "reading from bme280 reg"},
    {"write_reg", write_reg_call, "writing to bme280 reg"},
    {"temp_raw", temp_call, "read temp"},
    {"press_raw", pres_call, "read pressure"},
    {"hum_raw", hum_call, "read hum"},
    {"temp_si", temp_si_call, "read temp"},
    {"press_si", pres_si_call, "read pressure"},
    {"hum_si", hum_si_call, "read hum"},
	{NULL, NULL, NULL},
};

int main()
{
    protocol_task_init(device_api);
    stdio_init_all();
    stdio_task_init();
    led_init();
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);

    bme280_init(rp2040_i2c_read,  rp2040_i2c_write);
    while (1)
    {   
        protocol_task_handle(stdio_task_handle());
        led_handle();
    }
}
