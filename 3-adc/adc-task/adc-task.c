#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "stdio.h"

#define PIN_NUMBER 26
#define ADC_NUMBER 0

int adc_state = 0;
uint64_t time = 0;
uint64_t adc_period = 100000;

void adc_task_init(){
    adc_init();
    adc_gpio_init(PIN_NUMBER);
    adc_set_temp_sensor_enabled(true);
}

float measure(){
    adc_select_input(ADC_NUMBER);
    uint16_t voltage_counts = adc_read();
    float voltage = voltage_counts*3.3f/4096.0f;
    return voltage;
}

void adc_set_state(int v){
    adc_state = v;
}


float measure_temp(){
    adc_select_input(4);
    uint16_t voltage_counts = adc_read();
    float voltage = voltage_counts*3.3f/4096.0f;
    float temp_C = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temp_C;
}

void adc_handle(){
    if(adc_state == 1){
        if(time_us_64() > time){
            time = time_us_64() + adc_period;
            float v = measure();
            float t = measure_temp();
            printf("%f %f\n", v, t);
        }
    }
}


