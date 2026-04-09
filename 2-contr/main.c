#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

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

api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
    {"on", led_on_call, "led power on"},
    {"off", led_off_call, "led power on"},
    {"blink", led_blink_call, "led blink"},
    {"set_period", led_period_call, "set period of blink"},
    {"mem", mem_call, "read from memory"},
    {"wmem", wmem_call, "write to memory"},
	{NULL, NULL, NULL},
};

int main()
{
    protocol_task_init(device_api);
    stdio_init_all();
    stdio_task_init();
    led_init();
    while (1)
    {   
        protocol_task_handle(stdio_task_handle());
        led_handle();
    }
}
