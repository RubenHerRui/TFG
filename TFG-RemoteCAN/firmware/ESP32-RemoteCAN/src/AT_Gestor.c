#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AT_Gestor.h"

#define UART_NUM UART_NUM_1
#define TX_PIN 11
#define RX_PIN 10
#define PWR_PIN 18
#define BUF_SIZE 1024

static const char *TAG = "SIM7670G";

//Interaccion directa

void init_sim(void){
	//POWER ON
	gpio_set_direction(PWR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(PWR_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(5000)); // Esperar a que arranque

	//UART SIM
	uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config); //NUM USART Y CONFIIG
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); //NUM, TX, RX, RTC Y CTS
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0); //NUM, BUFF RX, BUFF TX
}

void write_AT(const char *comand) {
    uart_write_bytes(UART_NUM, comand, strlen(comand));
    ESP_LOGI(TAG, "Enviado: %s", comand);
    vTaskDelay(pdMS_TO_TICKS(500));
}

void read_AT(char* rx_buffer, size_t buffer_size) { //Bien
    int len = uart_read_bytes(UART_NUM, rx_buffer, buffer_size - 1, pdMS_TO_TICKS(1000));
    if (len > 0) {
        rx_buffer[len] = '\0';
        ESP_LOGI(TAG, "Recibido: %s", rx_buffer);
    } else {
        ESP_LOGW(TAG, "No se recibió respuesta del módulo SIM7670G");
        rx_buffer[0] = '\0'; // Limpiar buffer en caso de error
    }
}

//Gestion de mensajes

bool contieneOK(const char *str) {
    return strstr(str, "OK") != NULL;
}

bool contieneCLOST(const char *str){
    return strstr(str, "CMQTTCONNLOST") != NULL;
}

//Funciones externas RX/TX

bool read_MQTT(char *output) {
    char buffer[BUF_SIZE];
    read_AT(buffer, BUF_SIZE);

    const char *inicio = strstr(buffer, "+CMQTTRXPAYLOAD");
    if (inicio) {
        inicio = strchr(inicio, '\n'); // Avanzar al siguiente salto de línea
        inicio++; // Mover a la primera letra del mensaje
        const char *fin = strstr(inicio, "\n+CMQTTRXEND");
        size_t len = fin - inicio;
        strncpy(output, inicio, len);
        output[len] = '\0'; // Asegurar terminación
        return true;
    }
    if (contieneCLOST(buffer)) return false;
    return true;

}

bool AT(void) {
	char command[BUF_SIZE];
    write_AT("AT\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

void AT_CPIN_D(int pin) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CPIN=%d\r\n", pin);  
    write_AT(command);
    read_AT(command, BUF_SIZE);
}

bool AT_CPIN_Q(void) {
    char command[BUF_SIZE];
    write_AT("AT+CPIN?\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_CSQ(void) {
    char command[BUF_SIZE];
    write_AT("AT+CSQ\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_CREG_Q(void) {
    char command[BUF_SIZE];
    write_AT("AT+CREG?\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_CGREG_Q(void) {
    char command[BUF_SIZE];
    write_AT("AT+CGREG?\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_CPSI_Q(void) {
    char command[BUF_SIZE];
    write_AT("AT+CPSI?\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_CGDCONT_D(int cid, const char* ip, const char* APN) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CGDCONT=%d,\"%s\",\"%s\"\r\n", cid, ip, APN);
    write_AT(command);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_CGACT_D(int state, int cid) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CGACT=%d,%d\r\n", state, cid);
    write_AT(command);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_CGACT_Q(void) {
    char command[BUF_SIZE];
    write_AT("AT+CGACT?\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_START(void) {
    char command[BUF_SIZE];
    write_AT("AT+CMQTTSTART\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_ACCQ(int client_num, const char* client_id) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTACCQ=%d,\"%s\"\r\n", client_num, client_id);
    write_AT(command);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_WILLTOPIC(int client_num, const char* topic) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTWILLTOPIC=%d,%d\r\n", client_num, (int)strlen(topic));
    write_AT(command);
    write_AT(topic);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_WILLMSG(int client_num, int qos, const char* message) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTWILLMSG=%d,%d,%d\r\n", client_num, (int)strlen(message), qos);
    write_AT(command);
    write_AT(message);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_CONNECT(int client_num, const char* broker_ip, int keep_alive, int clean_session) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTCONNECT=%d,\"tcp://%s:1883\",%d,%d\r\n", client_num, broker_ip, keep_alive, clean_session);
    write_AT(command);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_SUBSCRIBE(int client_num, const char* topic, int qos) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTSUB=%d,%d,%d\r\n", client_num, (int)strlen(topic), qos);
    write_AT(command);
    write_AT(topic);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_DISCONNECT(int client_num, int timeout) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTDISC=%d,%d\r\n", client_num, timeout);
    write_AT(command);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_RELEASE(int client_num) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTREL=%d\r\n", client_num);
    write_AT(command);
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_STOP(void) {
    char command[BUF_SIZE];
    write_AT("AT+CMQTTSTOP\r\n");
    read_AT(command, BUF_SIZE);
	if (contieneOK(command)) return true;
	else return false;
}

bool AT_MQTT_TOPIC(int client_num, const char* topic) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTTOPIC=%d,%d\r\n", client_num, (int)strlen(topic));
    write_AT(command);
    write_AT(topic);
    read_AT(command, BUF_SIZE);
    return contieneOK(command);
}

bool AT_MQTT_PAYLOAD(int client_num, const char* payload) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTPAYLOAD=%d,%d\r\n", client_num, (int)strlen(payload));
    write_AT(command);
    write_AT(payload);
    read_AT(command, BUF_SIZE);
    return contieneOK(command);
}

bool AT_PUBLISH(int client_num, int qos, int retain) {
    char command[BUF_SIZE];
    sprintf(command, "AT+CMQTTPUB=%d,%d,%d\r\n", client_num, qos, retain);
    write_AT(command);
    read_AT(command, BUF_SIZE);
    return contieneOK(command);
}
