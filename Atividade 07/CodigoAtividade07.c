#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define UART_PORT UART_NUM_2
#define TXD_PIN 17
#define RXD_PIN 16

#define LED_PIN 4

void app_main(void) {
  // Configuração do LED
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  // Configuração da UART
  uart_config_t uart_config = {
  .baud_rate = 115200,
  .data_bits = UART_DATA_8_BITS,
  .parity = UART_PARITY_DISABLE,
  .stop_bits = UART_STOP_BITS_1,
  .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };

  uart_driver_install(UART_PORT, 1024, 0, 0, NULL, 0);
  uart_param_config(UART_PORT, &uart_config);
  uart_set_pin(UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  char *msg_ligar = "LIGAR\n";
  char *msg_desligar = "DESLIGAR\n";

  uint8_t data[128];

  while (1) {
    // Envia LIGAR
    uart_write_bytes(UART_PORT, msg_ligar, strlen(msg_ligar));
    printf("Enviado: %s", msg_ligar);

    vTaskDelay(pdMS_TO_TICKS(2000));

    int len = uart_read_bytes(UART_PORT, data, sizeof(data) - 1, pdMS_TO_TICKS(100));
      if (len > 0) {
      data[len] = '\0';
      printf("Recebido: %s", data);

      if (strstr((char*)data, "LIGAR")) {
        gpio_set_level(LED_PIN, 0);
      }
    }

    // Envia DESLIGAR
    uart_write_bytes(UART_PORT, msg_desligar, strlen(msg_desligar));
    printf("Enviado: %s", msg_desligar);

    vTaskDelay(pdMS_TO_TICKS(2000));

    len = uart_read_bytes(UART_PORT, data, sizeof(data) - 1, pdMS_TO_TICKS(100));
    if (len > 0) {
      data[len] = '\0';
      printf("Recebido: %s", data);

      if (strstr((char*)data, "DESLIGAR")) {
      gpio_set_level(LED_PIN, 1);
      }
    }
  }
}