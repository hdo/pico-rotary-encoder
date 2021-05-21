#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"


#define GPIO_ENCODER1_A  16  // BLUE
#define GPIO_ENCODER1_B  17  // YELLOW



typedef struct ROTARY_ENCODER_DATA
{
    uint8_t gpio_a, gpio_b;       // required
    int16_t min_value, max_value; // required
    uint8_t triggered;  
    uint32_t wait_until, wait_until_trigger;
    uint8_t filtered_a, last_a, last_filtered_a;
    int8_t dir;
    int16_t current_value;      

} rotary_encoder_t;



rotary_encoder_t encoder1;


const uint8_t LED_PIN = 25;

uint32_t last_led = 0;


uint32_t current_tick()  {
    return time_us_32() / 1000;
}

void rotary_task(rotary_encoder_t *data) {
    uint32_t current_us = time_us_32();

    uint8_t a = gpio_get(data->gpio_a);
    uint8_t b = gpio_get(data->gpio_b);

    if (data->triggered && current_us > data->wait_until) {
        data->triggered = 0;
        data->filtered_a = a;
    }

    if (a != data->last_a) {
        data->triggered = 1;
        data->wait_until = current_us + 2000;
    }

    // state change on filtered A
    if (data->filtered_a != data->last_filtered_a) {        
        // only allow signal everey 5ms
        if (current_us > data->wait_until_trigger) {
            data->dir = 0;
            if ((data->filtered_a) == b) {
                //puts("R");
                data->dir = 1;
            }
            else {
                //puts("L");
                data->dir = -1;
            }
            data->current_value += data->dir;
            if (data->current_value < data->min_value) {
                data->current_value = data->min_value;
            }
            if (data->current_value > data->max_value) {
                data->current_value = data->max_value;
            }
            printf("%d \r\n", data->current_value);
        }
        data->wait_until_trigger = current_us + 5000;
    }

    data->last_filtered_a = data->filtered_a;    
    data->last_a = a;
}

void init_gpio() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(GPIO_ENCODER1_A);
    gpio_set_dir(GPIO_ENCODER1_A, GPIO_IN);
    gpio_pull_up(GPIO_ENCODER1_A);

    gpio_init(GPIO_ENCODER1_B);
    gpio_set_dir(GPIO_ENCODER1_B, GPIO_IN);
    gpio_pull_up(GPIO_ENCODER1_B);

}

int main() {
    stdio_init_all();

    init_gpio();

    puts("Started ...\n");

    //gpio_set_irq_enabled_with_callback(GPIO_ENCODER1_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    //gpio_set_irq_enabled_with_callback(GPIO_ENCODER1_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    encoder1.gpio_a = GPIO_ENCODER1_A;
    encoder1.gpio_b = GPIO_ENCODER1_B;
    encoder1.min_value = 0;
    encoder1.max_value = 100;


    while (1) {
        //rotary_task5();
        //rotary_task(&encoder1);

        rotary_task(&encoder1);

        if (current_tick() - last_led > 200) {
            last_led = current_tick();
            uint8_t led_status = gpio_get(LED_PIN);
            if (led_status) {
                gpio_put(LED_PIN, 0);
            }
            else {
                gpio_put(LED_PIN, 1);
            }
        }
    }
}