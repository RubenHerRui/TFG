#include <stdio.h>
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "CAN_Gestor.h"

#define CAN_TX_PIN GPIO_NUM_5
#define CAN_RX_PIN GPIO_NUM_4

// Definir el nombre del logger para ESP_LOG
static const char *TAG = "CAN";

void init_can(void) {
    // Configuración general
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
    // Configuración de la velocidad (500 kbps)
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    // Configuración de filtros (aceptar todo)
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // Inicializar el driver de TWAI
    esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
    // Iniciar TWAI
    err = twai_start();
    ESP_LOGI(TAG, "Controlador TWAI iniciado correctamente");
}

void stop_can(void) {
    // Detener TWAI
    twai_stop(); 
    // Desinstalar el driver TWAI
    twai_driver_uninstall();
    ESP_LOGI(TAG, "Controlador TWAI detenido y desinstalado");
}

void transmit_can(int ID, int mode, int rtr, int payload_length, uint8_t* payload) {
    twai_message_t message;
    message.identifier = ID;
    message.extd = mode;  // Modo estándar (11 bits) o extendido (29 bits)
    message.rtr = rtr;    // Si es solicitud remota (RTR) o no
    message.data_length_code = payload_length;  // Longitud del payload
    
    // Copiar los datos del payload en la estructura
    for (int i = 0; i < payload_length; i++) {
        message.data[i] = payload[i];
    }

    // Intentar transmitir el mensaje
    esp_err_t err = twai_transmit(&message, pdMS_TO_TICKS(1000));  // 1000 ms de timeout
    if (err == ESP_OK) {
        //ESP_LOGI(TAG, "Mensaje CAN enviado! ID: 0x%08X", message.identifier);
    } else {
        ESP_LOGE(TAG, "Error al enviar el mensaje CAN: %s", esp_err_to_name(err));
    }
}

void receive_can(void) {
    twai_message_t message;
    // Esperar a recibir un mensaje CAN con un timeout de 1000 ms
    esp_err_t err = twai_receive(&message, pdMS_TO_TICKS(1000));
    if (err == ESP_OK) {
        //ESP_LOGI(TAG, "Mensaje recibido! ID: 0x%08X, Datos: ", message.identifier);
        // Mostrar los datos recibidos
        for (int i = 0; i < message.data_length_code; i++) {
            ESP_LOGI(TAG, "0x%02X ", message.data[i]);
        }
        if (message.identifier == 0x7E8 && message.data[1] == 0x41 && message.data[2] == 0x0D) {
            uint8_t speed = message.data[3];  // Extraer la velocidad en km/h
            ESP_LOGI(TAG, "Velocidad del coche: %d km/h", speed);
        }
    } 
    else {
        ESP_LOGW(TAG, "No se recibió mensaje CAN en el tiempo esperado");
    }
}

void tx_speed(void){
    uint8_t payload[8] = {0x02,0x01,0x0D,0x00,0x00,0x00,0x00,0x00};
    transmit_can(0x7DF,0,0,8,payload);
}