#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "AT_Gestor.h"

#define UART_NUM UART_NUM_1 //UART utilzado para comunicarse con 
#define TX_PIN 11 //Pin TX del UART
#define RX_PIN 10 //Pin RX del UART
#define PWR_PIN 18 //Pin que enciende el modulo SIM
#define UART_BUF_SIZE_RX 1024 //Tamaño del buffer para almazenar mesnajes Rx
#define UART_BUF_SIZE_TX 256 //Tamaño del buffer para almazenar mesnajes Tx
#define TAG "AT_Gestor" //Nombre del modulo
#define DEBUG false //Habilita los prints para el debug

static char command[UART_BUF_SIZE_TX]; //Buffer generico para formar los comandos AT

void init_sim(void){
	//Enciende el modulo sim
	gpio_set_direction(PWR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(PWR_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(5000));

	//Configura la comunicacion UART con el mdoulo sim
	uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config); 
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); 
    uart_driver_install(UART_NUM, UART_BUF_SIZE_RX * 2, 0, 0, NULL, 0); 
}

void write_AT(const char *command) {
    //Envia un comando AT por el UART
    uart_write_bytes(UART_NUM, command, strlen(command));
    if (DEBUG){
        ESP_LOGI(TAG, "Enviado: %s", command);
    }
}

void read_AT(char* rx_buffer) {
    //Mira si hay algun mensaje en el buffer de recepcion y lo copia a la variable que nos han pasado
    int len = uart_read_bytes(UART_NUM, rx_buffer, UART_BUF_SIZE_RX, pdMS_TO_TICKS(100));
    if (DEBUG && len > 0) {
        rx_buffer[len] = '\0';
        ESP_LOGI(TAG, "Recibido: %s", rx_buffer);
    }
}

//Comandos AT a utilizar
void AT(void) {
    write_AT("AT\r\n");
}

void AT_CPIN_D(int pin) {
    sprintf(command, "AT+CPIN=%d\r\n", pin);  
    write_AT(command);
}

void AT_CPIN_Q(void) {
    write_AT("AT+CPIN?\r\n");
}

void AT_CSQ(void) {
    write_AT("AT+CSQ\r\n");
}

void AT_CREG_Q(void) {
    write_AT("AT+CREG?\r\n");
}

void AT_CGREG_Q(void) {
    write_AT("AT+CGREG?\r\n");
}

void AT_CPSI_Q(void) {
    write_AT("AT+CPSI?\r\n");
}

void AT_CGDCONT_D(int cid, const char* ip, const char* APN) {
    sprintf(command, "AT+CGDCONT=%d,\"%s\",\"%s\"\r\n", cid, ip, APN);
    write_AT(command);
}

void AT_CGACT_D(int state, int cid) {
    sprintf(command, "AT+CGACT=%d,%d\r\n", state, cid);
    write_AT(command);
}

void AT_CGACT_Q(void) {
    write_AT("AT+CGACT?\r\n");
}

void AT_MQTT_START(void) {
    write_AT("AT+CMQTTSTART\r\n");
}

void AT_MQTT_ACCQ(int client_num, const char* client_id, int server_type) {
    sprintf(command, "AT+CMQTTACCQ=%d,\"%s\",%d\r\n", client_num, client_id, server_type);
    write_AT(command);
}

void AT_MQTT_WILLTOPIC(int client_num, const char* topic) {
    sprintf(command, "AT+CMQTTWILLTOPIC=%d,%d\r\n", client_num, (int)strlen(topic));
    write_AT(command);
    write_AT(topic);
}


void AT_MQTT_WILLMSG(int client_num, int qos, const char* message) {
    sprintf(command, "AT+CMQTTWILLMSG=%d,%d,%d\r\n", client_num, (int)strlen(message), qos);
    write_AT(command);
    write_AT(message);
}

void AT_MQTT_CONNECT(int client_num, const char* broker_ip, const char* broker_port,int keep_alive, int clean_session) {
    sprintf(command, "AT+CMQTTCONNECT=%d,\"tcp://%s:%s\",%d,%d\r\n", client_num, broker_ip, broker_port,keep_alive, clean_session);
    write_AT(command);
}

void AT_MQTT_SUBSCRIBE(int client_num, const char* topic, int qos) {
    sprintf(command, "AT+CMQTTSUB=%d,%d,%d\r\n", client_num, (int)strlen(topic), qos);
    write_AT(command);
}

void AT_MQTT_SUBSCRIBE_2(const char* topic){
    write_AT(topic);
}

void AT_MQTT_DISCONNECT(int client_num, int timeout) {
    sprintf(command, "AT+CMQTTDISC=%d,%d\r\n", client_num, timeout);
    write_AT(command);
}

void AT_MQTT_RELEASE(int client_num) {
    sprintf(command, "AT+CMQTTREL=%d\r\n", client_num);
    write_AT(command);
}

void AT_MQTT_STOP(void) {
    write_AT("AT+CMQTTSTOP\r\n");
}

void AT_MQTT_TOPIC(int client_num, const char* topic) {
    sprintf(command, "AT+CMQTTTOPIC=%d,%d\r\n", client_num, (int)strlen(topic));
    write_AT(command);
}

void AT_MQTT_TOPIC_2(const char* topic) {
    write_AT(topic);
}

void AT_MQTT_PAYLOAD(int client_num, const char* payload) {
    sprintf(command, "AT+CMQTTPAYLOAD=%d,%d\r\n", client_num, (int)strlen(payload));
    write_AT(command);
}

void AT_MQTT_PAYLOAD_2(const char* payload) {
    write_AT(payload);
}

void AT_PUBLISH(int client_num, int qos, int retain) {
    sprintf(command, "AT+CMQTTPUB=%d,%d,%d\r\n", client_num, qos, retain);
    write_AT(command);
}
