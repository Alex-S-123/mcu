#include "led-task.h"
#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const uint LED_PIN = 25;
uint LED_BLINK_PERIOD_US = 500000;
bool enabled = true;

uint64_t led_ts;
led_state_t led_state;

void led_init(){
    led_state = LED_STATE_OFF;
    led_ts = 0;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

void led_handle(){
    switch (led_state)
    {
    case LED_STATE_OFF:
        gpio_put(LED_PIN, 0);
    break;
    case LED_STATE_ON:
        gpio_put(LED_PIN, 1);
    break;
    case LED_STATE_BLINK:
        if (time_us_64() > led_ts)
        {
        led_ts = time_us_64() + (LED_BLINK_PERIOD_US / 2);
        if(enabled) gpio_put(LED_PIN, 0);
        if(!enabled) gpio_put(LED_PIN, 1);
        enabled = !enabled;
        }
    break;
    default:
    break;
    }
}

void led_task_state_set(led_state_t state){
    led_state = state;
}

void led_task_set_blink_period_ms(uint32_t period_ms){
    if(period_ms <= 0){
        printf("wrong period");
        return;
    }
    LED_BLINK_PERIOD_US = 1000*period_ms;
}