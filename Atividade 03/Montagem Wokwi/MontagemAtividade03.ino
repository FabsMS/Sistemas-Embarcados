#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define LED1   4
#define LED2   5
#define LED3   6
#define LED4   7
#define BUZZER 8

#define DELAY_MS 500

void setup()
{
    // Timer 0: LEDs 
    ledc_timer_config_t timer_led = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT, 
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = 1000,
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_led);

    // Timer 1: Buzzer
    ledc_timer_config_t timer_buz = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num       = LEDC_TIMER_1,
        .freq_hz         = 1000,
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_buz);

    // Canais dos LEDs
    int pins[4] = {LED1, LED2, LED3, LED4};

    for (int i = 0; i < 4; i++) {
        ledc_channel_config_t ch = {
            .gpio_num   = pins[i],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel    = (ledc_channel_t)i, 
            .timer_sel  = LEDC_TIMER_0,
            .duty       = 0,
            .hpoint     = 0
        };
        ledc_channel_config(&ch);
    }

    // Canal do buzzer
    ledc_channel_config_t ch_buz = {
        .gpio_num   = BUZZER,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_4,
        .timer_sel  = LEDC_TIMER_1,  
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ch_buz);


}

void loop()
{
    // FASE 1: Fading sincronizado
    for (int duty = 0; duty < 1023; duty += 100) {
        for (int i = 0; i < 4; i++) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i);
        }
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
    }
    for (int duty = 1023; duty >= 0; duty -= 100) {
        for (int i = 0; i < 4; i++) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i);
        }
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
    }


    // FASE 2: Fading sequencial
    for (int i = 0; i < 4; i++) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i, 1023);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i);

        vTaskDelay(pdMS_TO_TICKS(300));

        ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i);
    }


    // FASE 3: Buzzer variando frequência
    for (int freq = 500; freq <= 2000; freq += 100) {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, freq);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 512);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    for (int freq = 2000; freq >= 500; freq -= 100) {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, freq);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);
}
