#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdlib.h"
#include "stdio.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"
const uint LED_PIN = 25;

int main()
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    while (1)
    {   
        char symbol = getchar();
        if ((int)symbol != 10){
            printf("received char: %c [ ASCII code: %d ]\n", symbol, symbol);
        }
        switch(symbol)
        {
        case 'e':
            gpio_put(LED_PIN, true);
            printf("led enable done\n");
            break;
        case 'd':
            gpio_put(LED_PIN, 0);
            printf("led disable done\n");
            break;
        case 'v':
            printf("name: %s, version: %s\n", DEVICE_NAME, DEVICE_VRSN);
            break;

        default:
            break;
        }
    }
}
