#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "MQTT_Gestor.h"

// Función para la tarea de la máquina de estados
void general_state_task(void *pvParameters) {
    while (true) {
        general_state_machine();
    }
}

void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(500)); // Espera inicial

    // Crear la tarea de la máquina de estados
    xTaskCreate(general_state_task, "GeneralStateTask", 4096, NULL, 5, NULL);
    vTaskDelay(pdMS_TO_TICKS(50000)); // Publicación cada 1 segundo
    while (true) {
        tx_mqtt("test/topic", "Hola desde ESP");
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

