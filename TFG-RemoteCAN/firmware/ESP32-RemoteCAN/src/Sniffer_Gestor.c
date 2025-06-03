#include <stdio.h>        
#include <stdint.h>     
#include "driver/twai.h"   
#include "driver/uart.h"

#define SEPARATOR ","
#define TERMINATOR "\n"

void print_hex(uint32_t value) {
    printf("%02lX", value);  
}

void print_sniffing(twai_message_t message){
    print_hex(message.identifier);
    printf(SEPARATOR);
    print_hex(message.rtr);  
    printf(SEPARATOR);  
    print_hex(message.extd);  
    printf(SEPARATOR);  
    for (uint8_t i = 0; i < message.data_length_code; i++) {
        print_hex(message.data[i]);
    }
    printf(TERMINATOR);
}

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