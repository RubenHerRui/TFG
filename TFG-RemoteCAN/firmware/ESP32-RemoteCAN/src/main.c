#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/twai.h"
#include "MQTT_Gestor.h"
#include "CAN_Gestor.h"
#include "Sniffer_Gestor.h"

#define SNIFFING false
#define MQTT_BUF_SIZE 256  // Tamaño del buffer mqtt
#define DEBUG true

void write_CAN_speeds(void){
    write_CAN_speed();
    vTaskDelay(pdMS_TO_TICKS(5000));
    write_CAN_rpm();
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void read_CAN_processor(void){
    twai_message_t message;
    if (read_CAN(&message)){
        if (SNIFFING){
            print_sniffing(message);
        }
        if (message.identifier == 0x7E8 && message.data[1] == 0x41 && message.data[2] == 0x0D) {
                uint8_t speed = message.data[3];  // Extraer la velocidad en km/h
                char buffer[8];
                sprintf(buffer,"%d",speed);
                if (DEBUG){
                    ESP_LOGI("Main", "Velocidad del coche: %d km/h", speed);
                }
                write_MQTT("car/speed", buffer);
        }
        else if (message.identifier == 0x7E8 && message.data[1] == 0x41 && message.data[2] == 0x0C) {
                uint16_t speed = (256*message.data[3]+message.data[4])/4;  // Extraer la velocidad en rpm
                char buffer[8];
                sprintf(buffer,"%d",speed);
                if (DEBUG){
                    ESP_LOGI("Main", "Velocidad del coche: %d rpm", speed);
                }
                write_MQTT("car/rpm", buffer);
        }
    }
    
}

void CAN_task(void *pvParameters){
    init_CAN();
    while (true){
        read_CAN_processor();
    }
}

void read_MQTT_processor(void){
    //Comprueba si se ha recibido un mensaje y lo gestiona en caso afirmativo
    if (read_MQTT_available()){
        char local_buffer_mqtt[MQTT_BUF_SIZE];
        read_MQTT(local_buffer_mqtt);
        if (DEBUG){
            ESP_LOGI("Main","%s", local_buffer_mqtt);      
        }
    }
}
void MQTT_task(void *pvParameters) {
    //Proceso que mantiene activo el modulo de gestion de mqtt y comprueba si se ha recibido un mensaje
    while (true) {
        sim_state_machine();
        read_MQTT_processor();
    }
}

void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(10000));
    xTaskCreate(CAN_task, "CAN_task", 4096, NULL, 5, NULL);
    if (!SNIFFING){
        xTaskCreate(MQTT_task, "MQTT_task", 4096, NULL, 5, NULL);
    }
    if (DEBUG){
        ESP_LOGI("Main","Puedes enchufar");
    }
    while (1) {
        if (!SNIFFING){
            write_CAN_speeds();
            //write_MQTT("car/speed", "10");
            //vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}


