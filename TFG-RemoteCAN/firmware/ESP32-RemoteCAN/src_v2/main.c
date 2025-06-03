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

void send_CAN_speeds(void){
    tx_speed();
    vTaskDelay(pdMS_TO_TICKS(5000));
    tx_rpm();
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void read_CAN_processor(void){
    twai_message_t message;
    if (receive_can(&message)){
        if (SNIFFING){
            print_sniffing(message);
        }
        if (message.identifier == 0x7E8 && message.data[1] == 0x41 && message.data[2] == 0x0D) {
                uint8_t speed = message.data[3];  // Extraer la velocidad en km/h
                char buffer[8];
                sprintf(buffer,"%d",speed);
                ESP_LOGI("Main", "Velocidad del coche: %d km/h", speed);
                tx_mqtt("car/speed", buffer);
        }
        else if (message.identifier == 0x7E8 && message.data[1] == 0x41 && message.data[2] == 0x0C) {
                uint16_t speed = (256*message.data[3]+message.data[4])/4;  // Extraer la velocidad en km/h
                char buffer[8];
                sprintf(buffer,"%d",speed);
                ESP_LOGI("Main", "Velocidad del coche: %d rpm", speed);
                tx_mqtt("car/rpm", buffer);
        }
    }
    
}

void can_task(void *pvParameters){
    //init_can();
    while (true){
        read_CAN_processor();
    }
}

void read_MQTT_processor(void){
    //Comprueba si se ha recibido un mensaje y lo gestiona en caso afirmativo
    if (rx_mqtt_available()){
        char local_buffer_mqtt[MQTT_BUF_SIZE];
        rx_mqtt(local_buffer_mqtt);
        ESP_LOGI("Main","%s", local_buffer_mqtt);      
    }
}
void general_state_task(void *pvParameters) {
    //Proceso que mantiene activo el modulo de gestion de mqtt y comprueba si se ha recibido un mensaje
    while (true) {
        general_state_machine();
        read_MQTT_processor();
    }
}

void app_main(void) {
    init_can();
    vTaskDelay(pdMS_TO_TICKS(10000));
    //init_can();
    xTaskCreate(can_task, "can_task", 4096, NULL, 5, NULL);
    if (!SNIFFING){
    xTaskCreate(general_state_task, "GeneralStateTask", 4096, NULL, 5, NULL);
    }
    ESP_LOGI("Main","Puedes enchufar");
    //uint16_t speed = 6969;  // Extraer la velocidad en km/h
    //char buffer[8];
    //sprintf(buffer,"%d",speed);
    while (1) {
        //tx_mqtt("car/speed", buffer);
        //vTaskDelay(pdMS_TO_TICKS(5000));
        if (!SNIFFING){
            send_CAN_speeds();
        }
    }
}

/*
void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(5000)); // Espera inicial
    // Crear la tarea de la máquina de estados
    xTaskCreate(general_state_task, "GeneralStateTask", 4096, NULL, 5, NULL);
    while (true) {
        tx_mqtt("test/topic", "Hola desde ESP");3
        vTaskDelay(pdMS_TO_TICKS(10000)); 
    }
}
*/


/*
void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(5000)); // Espera inicial
    twai_message_t message1 = {
    .identifier = 13,
    .rtr = 0,
    .extd = 0,
    .data_length_code = 5,
    .data = {1, 2, 3, 4, 5}
    };
    while (true) {
        print_sniffing(message1);
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}
*/
