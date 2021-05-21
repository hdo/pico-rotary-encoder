#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"


#define GPIO_ENCODER1_A  16  // BLUE
#define GPIO_ENCODER1_B  17  // YELLOW


const uint8_t LED_PIN = 25;

uint32_t last_led = 0;

uint32_t last_event = 0;
uint32_t last_pulse = 0;

uint32_t last_pulse_a = 0;
uint32_t last_pulse_b = 0;

uint32_t last_a_rise = 0;
uint32_t last_a_fall = 0;
uint32_t last_b_rise = 0;
uint32_t last_b_fall = 0;

uint32_t last_event_a, last_event_b;

uint8_t last_a, last_b, a_event, b_event;

uint8_t triggered = 0;
uint32_t triggered_at = 0;
uint32_t last_trigger = 0;
uint32_t wait_until = 0;

int8_t last_dir = 0;
uint32_t last_dir_change = 0;
uint32_t last_state_change = 0;

uint8_t filtered_a, filtered_b, last_af, last_bf;

int16_t value = 0;

uint32_t current_tick()  {
    return time_us_32() / 1000;
}

void rotary_task6() {
    uint32_t current_us = time_us_32();

    uint8_t a = gpio_get(GPIO_ENCODER1_A);
    uint8_t b = gpio_get(GPIO_ENCODER1_B);

    if (triggered && current_us > wait_until) {
        triggered = 0;
        filtered_a = a;
    }

    if (a != last_a) {
        triggered = 1;
        wait_until = current_us + 2000;
    }

    // state change on filtered A
    if (filtered_a != last_af) {        
        // only allow signal everey 5ms
        if (current_us - last_trigger > 5000) {
            int8_t dir = 0;
            if (filtered_a == b) {
                //puts("R");
                dir = 1;
            }
            else {
                //puts("L");
                dir = -1;
            }
            value += dir;
            printf("%d \r\n", value);
        }
        last_trigger = current_us;
    }

    last_af = filtered_a;    
    last_a = a;
}



void rotary_task5() {
    uint32_t current_us = time_us_32();

    uint8_t a = gpio_get(GPIO_ENCODER1_A);
    uint8_t b = gpio_get(GPIO_ENCODER1_B);

    if (triggered && current_us > wait_until) {
        triggered = 0;
        filtered_a = a;
    }

    if (a != last_a) {
        triggered = 1;
        wait_until = current_us + 2000;
    }

    // state change on filtered A
    if (filtered_a != last_af) {
        int8_t dir = 0;
        if (filtered_a == b) {
            puts("R");
            dir = 1;
        }
        else {
            puts("L");
            dir = -1;
        }
        value += dir;
        printf("%d \r\n", value);
    }

    last_af = filtered_a;    
    last_a = a;
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

    while (1) {
        //rotary_task5();
        rotary_task6();

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