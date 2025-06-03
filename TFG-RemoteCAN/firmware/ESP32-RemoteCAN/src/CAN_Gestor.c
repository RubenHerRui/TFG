#include <stdio.h>
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "CAN_Gestor.h"

#define CAN_TX_PIN GPIO_NUM_16
#define CAN_RX_PIN GPIO_NUM_15
#define DEBUG true

// Definir el nombre del logger para ESP_LOG
static const char *TAG = "CAN";

void init_CAN(void) {
    // Configuración general
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
    // Configuración de la velocidad (500 kbps)
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    // Configuración de filtros (aceptar todo)
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // Inicializar el driver de TWAI
    twai_driver_install(&g_config, &t_config, &f_config);
    // Iniciar TWAI
    twai_start();
}

void stop_CAN(void) {
    twai_stop(); 
    twai_driver_uninstall();
}

void write_CAN(int ID, int mode, int rtr, int payload_length, uint8_t* payload) {
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
        if (DEBUG){
            ESP_LOGI(TAG, "Mensaje enviado.");
        }
    } else {
        if (DEBUG){
            ESP_LOGE(TAG, "Error al enviar el mensaje CAN: %s", esp_err_to_name(err));
        }
    }
}

bool read_CAN(twai_message_t *message) {
    esp_err_t err = twai_receive(message, pdMS_TO_TICKS(10));
    if (err == ESP_OK) {
        return true;
    }
    return false;
}


void write_CAN_speed(void){
    uint8_t payload[8] = {0x02,0x01,0x0D,0x00,0x00,0x00,0x00,0x00};
    write_CAN(0x7DF,0,0,8,payload);
}

void write_CAN_rpm(void){
    uint8_t payload[8] = {0x02,0x01,0x0C,0x00,0x00,0x00,0x00,0x00};
    write_CAN(0x7DF,0,0,8,payload);
}