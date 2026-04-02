#include "pico/stdlib.h"
//#include "hardware/gpio.h"
#include "stdlib.h"
#include "stdio.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"


int main()
{
    stdio_init_all();
    
    //printf("received char: %c [ ASCII code: %d ]\n", symbol, symbol);
    while (1)
    {   
        char symbol = getchar();
        if ((int)symbol != 10){
            printf("received char: %c [ ASCII code: %d ]\n", symbol, symbol);
        }
    }
}
