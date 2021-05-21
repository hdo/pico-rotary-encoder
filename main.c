#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "rotaryencoder.h"

#define GPIO_ENCODER1_A  16  // BLUE
#define GPIO_ENCODER1_B  17  // YELLOW

#define GPIO_ENCODER2_A  18 
#define GPIO_ENCODER2_B  19 


rotary_encoder_t encoder1, encoder2;


const uint8_t LED_PIN = 25;

uint32_t last_led = 0;
int16_t last_value1 = 0;
int16_t last_value2 = 0;


uint32_t current_tick()  {
    return time_us_32() / 1000;
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

    gpio_init(GPIO_ENCODER2_A);
    gpio_set_dir(GPIO_ENCODER2_A, GPIO_IN);
    gpio_pull_up(GPIO_ENCODER2_A);

    gpio_init(GPIO_ENCODER2_B);
    gpio_set_dir(GPIO_ENCODER2_B, GPIO_IN);
    gpio_pull_up(GPIO_ENCODER2_B);

}

int main() {
    stdio_init_all();

    init_gpio();

    puts("Started ...\n");

    encoder1.gpio_a = GPIO_ENCODER1_A;
    encoder1.gpio_b = GPIO_ENCODER1_B;
    encoder1.min_value = 0;
    encoder1.max_value = 100;
    encoder1.factor = 2;
    encoder1.current_value = 100;

    encoder2.gpio_a = GPIO_ENCODER2_A;
    encoder2.gpio_b = GPIO_ENCODER2_B;
    encoder2.min_value = 0;    
    encoder2.max_value = 200;
    encoder2.factor = 5;
    encoder2.current_value = 100;

    while (1) {

        rotary_task2(&encoder1);
        rotary_task2(&encoder2);

        if (last_value1 != encoder1.current_value) {
            last_value1 = encoder1.current_value;
            printf("value1: %d \r\n", last_value1);
        }

        if (last_value2 != encoder2.current_value) {
            last_value2 = encoder2.current_value;
            printf("value2: %d \r\n", last_value2);
        }

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