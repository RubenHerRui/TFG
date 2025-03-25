#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define UART_NUM UART_NUM_1
#define TX_PIN 11
#define RX_PIN 10
#define PWR_PIN 18
#define BUF_SIZE 1024

static const char *TAG = "SIM7670G";

void power_on_sim7670g() {
    gpio_set_direction(PWR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(PWR_PIN, 1);
    ESP_LOGI(TAG, "Módulo SIM7670G encendido");
    vTaskDelay(pdMS_TO_TICKS(5000)); // Esperar a que arranque
}

void sim7670g_task(void *pvParameters) {
    uint8_t data[BUF_SIZE];
    const char *at_cmd = "AT\r\n";
    
    while (1) {
        // Enviar comando AT
        uart_write_bytes(UART_NUM, at_cmd, strlen(at_cmd));
        ESP_LOGI(TAG, "Enviado: %s", at_cmd);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Leer respuesta
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(500));
        if (len > 0) {
            data[len] = '\0'; // Asegurar que la respuesta es una cadena válida
            ESP_LOGI(TAG, "Recibido: %s", data);
        } else {
            ESP_LOGW(TAG, "No se recibió respuesta del módulo SIM7670G");
        }
    }
}

void app_main(void) {
    // Encender el módulo SIM7670G
    power_on_sim7670g();
    
    // Configurar la UART
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    
    // Crear tarea para comunicación con SIM7670G
    xTaskCreate(sim7670g_task, "sim7670g_task", 4096, NULL, 5, NULL);
}