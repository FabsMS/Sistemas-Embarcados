#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

// Definição dos Pinos
#define PINO_LED1 4
#define PINO_LED2 5
#define PINO_LED3 6
#define PINO_LED4 7
#define PINO_BUZZER 8

// Definição do Delay
#define DELAY_MS 50

void app_main(void)
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT, 
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ledc_timer_config(&timer);

    int pins[5] = {PINO_LED1, PINO_LED2, PINO_LED3, PINO_LED4, PINO_BUZZER};

    for (int i = 0; i < 5; i++) {
        ledc_channel_config_t channel = {
            .gpio_num = pins[i],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = (ledc_channel_t)i,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0,
            .hpoint = 0
        };

        ledc_channel_config(&channel);
    }

    while (1) {

        // FASE 1 - Fade sincronizado
        for (int duty = 0; duty < 1023; duty += 20) {
            for (int i = 0; i < 4; i++) {
                ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i, duty);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i);
            }
            vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
        }

        for (int duty = 1023; duty > 0; duty -= 20) {
            for (int i = 0; i < 4; i++) {
                ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i, duty);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i);
            }
            vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
        }

        // FASE 2 - LEDs sequenciais
        for (int i = 0; i < 4; i++) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i, 1023);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i);

            vTaskDelay(pdMS_TO_TICKS(300));

            ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i);
        }

        // FASE 3 - Buzzer variando frequência
        for (int freq = 500; freq <= 2000; freq += 100) {
            ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);

            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 512);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);

            vTaskDelay(pdMS_TO_TICKS(100));
        }

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);
    }
}