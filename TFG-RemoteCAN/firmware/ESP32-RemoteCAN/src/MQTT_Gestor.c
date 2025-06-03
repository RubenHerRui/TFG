#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "AT_Gestor.h"
#include "MQTT_Gestor.h"

//Datos para los comandos AT
#define PIN 5581
#define CID 1
#define IP "IP"
#define APN "jazzinternet"
#define STATE 1
#define CLIENT_NUM 0
#define CLIENT_ID "client test0"
#define TOPIC "car/test"
#define MESSAGE "26"
#define BROKER_IP "37.15.75.249"
#define KEEP_ALIVE 60
#define CLEAN_SESSION 1
#define QOS 1
#define RETAIN 60
#define TIMEOUT 120
#define SERVER_TYPE 0 //TLS
//#define BROKER_PORT "8883" //TLS
#define BROKER_PORT "1883" //NO TLS

//Datos varios
#define MQTT_BUF_SIZE 256  //Tamaño del buffer MQTT
#define UART_BUF_SIZE 1024  //Tamaño del buffer UART
#define MAX_TOPIC_LEN 32 //Tamaño maximo del topic
#define DEBUG false //Habilita los prints para el debug
#define TAG "MQTT_GESTOR" //Nombre del modulo

//Estados de la maquina de estados
typedef enum {
    POWERON,
    START_TX,
    START_RX,
    CPIN_TX,
    CPIN_RX,
    CSQ_TX,
    CSQ_RX,
    CREG_Q_TX,
    CREG_Q_RX,
    CGREG_Q_TX,
    CGREG_Q_RX,
    CPSI_Q_TX,
    CPSI_Q_RX,
    CGDCONT_D_TX,
    CGDCONT_D_RX,
    CGACT_D_TX,
    CGACT_D_RX,
    CGACT_Q_TX,
    CGACT_Q_RX,
    DISCONNECT_TX,
    DISCONNECT_RX,
    RELEASE_TX,
    RELEASE_RX,
    STOP_TX,
    STOP_RX,
    START_MQTT_TX,
    START_MQTT_RX,
    ACCQ_TX,
    ACCQ_RX,
    CONNECT_TX,
    CONNECT_RX,
    SUBSCRIBE_TX,
    SUBSCRIBE_RX,
    SUBSCRIBE_TX_2,
    SUBSCRIBE_RX_2,
    TXENABLED,
    TOPIC_TX,
    TOPIC_RX,
    TOPIC_TX_2,
    TOPIC_RX_2,
    PAYLOAD_TX,
    PAYLOAD_RX,
    PAYLOAD_TX_2,
    PAYLOAD_RX_2,
    PUBLISH_TX,
    PUBLISH_RX,
} sim_s;

//Variables static
static char buffer_mqtt[MQTT_BUF_SIZE]; // Conetenedor del ultimo mensaje MQTT recibido
static sim_s sim_state = POWERON; //Estado de la maquina de estados
static char write_mqtt_topic[MAX_TOPIC_LEN];  //Buffer para almacenar el topic tx
static char write_mqtt_message[MQTT_BUF_SIZE];  //Buffer para almacenar el mensaje tx
static bool OK_FLAG = false; //Indica que el ultimo mensaje contenia OK
static bool ERROR_FLAG = false; //Indica que el ultimo mensaje contenia ERROR
static bool CLOST_FLAG = false; //Indica que el ultimo mensaje contenia CONNECTION_LOST
static bool INPUT_FLAG = false; //Indica que el ultimo mensaje espera un input

//Funciones de procesamiento de la recepcion
bool contains_ERROR(const char *str) {
    return strstr(str, "ERROR") != NULL;
}

bool contains_OK(const char *str) {
    return strstr(str, "OK") != NULL;
}

bool contains_CLOST(const char *str){
    return strstr(str, "CMQTTCONNLOST") != NULL;
}

bool contains_INPUT(const char *str){
    return strstr(str, ">") != NULL;
}

void contains_MQTT(char *buffer_uart) {
    //Comprueba si se ha recibido un mensaje del mqtt y lo copia
    const char *start = strstr(buffer_uart, "+CMQTTRXPAYLOAD");
    if (start) {
        start = strchr(start, '\n'); // Avanzar al siguiente salto de línea
        start++; // Mover a la primera letra del mensaje
        const char *end = strstr(start, "\n+CMQTTRXEND");
        size_t len = end - start;
        strncpy(buffer_mqtt, start, len);
        //output[len] = '\0'; // Asegurar terminación
    }
}

void read_AT_processor(void) {
    //Se comprueba el buffer de recepcion uart y se actualizan las variables de estado segun lo que contenga
    char buffer_uart[UART_BUF_SIZE];
    read_AT(buffer_uart);
    contains_MQTT(buffer_uart);
    OK_FLAG = contains_OK(buffer_uart);
    ERROR_FLAG = contains_ERROR(buffer_uart);
    CLOST_FLAG = contains_CLOST(buffer_uart);
    INPUT_FLAG = contains_INPUT(buffer_uart);
}

//Maquina de estados
void sim_state_machine(){
    if (DEBUG){
        ESP_LOGI(TAG, "Estado actual: %d\n", sim_state);
    }
    switch (sim_state) {
        case POWERON:
            init_sim();
            sim_state = START_TX;
            break;

        case START_TX:
            AT();
            sim_state = START_RX;
            break;

        case START_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CPIN_TX;
            else if (ERROR_FLAG) sim_state = START_TX;
            break;

        case CPIN_TX:
            AT_CPIN_D(PIN);
            AT_CPIN_Q();
            sim_state = CPIN_RX;
            break;

        case CPIN_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CSQ_TX;
            else if (ERROR_FLAG) sim_state = CPIN_TX;
            break;

        case CSQ_TX:
            AT_CSQ();
            sim_state = CSQ_RX;
            break;

        case CSQ_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CREG_Q_TX;
            else if (ERROR_FLAG) sim_state = CSQ_TX;
            break;

        case CREG_Q_TX:
            AT_CREG_Q();
            sim_state = CREG_Q_RX;
            break;

        case CREG_Q_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CGREG_Q_TX;
            else if (ERROR_FLAG) sim_state = CREG_Q_TX;
            break;

        case CGREG_Q_TX:
            AT_CGREG_Q();
            sim_state = CGREG_Q_RX;
            break;

        case CGREG_Q_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CPSI_Q_TX;
            else if (ERROR_FLAG) sim_state = CGREG_Q_TX;
            break;

        case CPSI_Q_TX:
            AT_CPSI_Q();
            sim_state = CPSI_Q_RX;
            break;

        case CPSI_Q_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CGDCONT_D_TX;
            else if (ERROR_FLAG) sim_state = CPSI_Q_TX;
            break;

        case CGDCONT_D_TX:
            AT_CGDCONT_D(CID, IP, APN);
            sim_state = CGDCONT_D_RX;
            break;

        case CGDCONT_D_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CGACT_D_TX;
            else if (ERROR_FLAG) sim_state = CGDCONT_D_TX;
            break;

        case CGACT_D_TX:
            AT_CGACT_D(STATE, CID);
            sim_state = CGACT_D_RX;
            break;

        case CGACT_D_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CGACT_Q_TX;
            else if (ERROR_FLAG) sim_state = CGACT_D_TX;
            break;

        case CGACT_Q_TX:
            AT_CGACT_Q();
            sim_state = CGACT_Q_RX;
            break;

        case CGACT_Q_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = START_MQTT_TX;
            else if (ERROR_FLAG) sim_state = CGACT_Q_TX;
            break;

        case DISCONNECT_TX:
            AT_MQTT_DISCONNECT(CLIENT_NUM, TIMEOUT);
            sim_state = DISCONNECT_RX;
            break;

        case DISCONNECT_RX:
            read_AT_processor();
            if (OK_FLAG || ERROR_FLAG) {
                sim_state = RELEASE_TX;
            }
            break;

        case RELEASE_TX:
            AT_MQTT_RELEASE(CLIENT_NUM);
            sim_state = RELEASE_RX;
            break;

        case RELEASE_RX:
            read_AT_processor();
            if (OK_FLAG || ERROR_FLAG) {
                sim_state = STOP_TX;
            }
            break;

        case STOP_TX:
            AT_MQTT_STOP();
            sim_state = STOP_RX;
            break;

        case STOP_RX:
            read_AT_processor();
            if (OK_FLAG || ERROR_FLAG) {
                sim_state = START_MQTT_TX;
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            break;

        case START_MQTT_TX:
            AT_MQTT_START();
            sim_state = START_MQTT_RX;
            break;

        case START_MQTT_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = ACCQ_TX;
            else if (ERROR_FLAG) sim_state = DISCONNECT_TX;
            break;

        case ACCQ_TX:
            AT_MQTT_ACCQ(CLIENT_NUM, CLIENT_ID, SERVER_TYPE);
            sim_state = ACCQ_RX;
            break;

        case ACCQ_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = CONNECT_TX;
            else if (ERROR_FLAG) sim_state = DISCONNECT_TX;
            break;

        case CONNECT_TX:
            AT_MQTT_CONNECT(CLIENT_NUM, BROKER_IP, BROKER_PORT,KEEP_ALIVE, CLEAN_SESSION);
            sim_state = CONNECT_RX;
            break;

        case CONNECT_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = SUBSCRIBE_TX;
            else if (ERROR_FLAG) sim_state = CONNECT_TX;
            break;
        
        case SUBSCRIBE_TX:
            AT_MQTT_SUBSCRIBE(CLIENT_NUM, TOPIC, QOS);
            sim_state = SUBSCRIBE_RX;
            break;
    
        case SUBSCRIBE_RX:
            read_AT_processor();
            if (INPUT_FLAG) sim_state = SUBSCRIBE_TX_2;
            else if (ERROR_FLAG) sim_state = SUBSCRIBE_TX;
            break;

        case SUBSCRIBE_TX_2:
            AT_MQTT_SUBSCRIBE_2(TOPIC);
            sim_state = SUBSCRIBE_RX_2;
            break;
    
        case SUBSCRIBE_RX_2:
            read_AT_processor();
            if (OK_FLAG){ 
                sim_state = TXENABLED;
                if (DEBUG){
                    ESP_LOGI(TAG, "Comunicacion Completada");
                }
            }
            else if (ERROR_FLAG) sim_state = SUBSCRIBE_TX;
            break;

        case TXENABLED:
            read_AT_processor();
            if (CLOST_FLAG) sim_state = START_MQTT_TX;
            break;
        
        case TOPIC_TX:
            AT_MQTT_TOPIC(CLIENT_NUM,write_mqtt_topic);
            sim_state = TOPIC_RX;
            break;
    
        case TOPIC_RX:
            read_AT_processor();
            if (INPUT_FLAG) sim_state = TOPIC_TX_2;
            else if (ERROR_FLAG) sim_state = TOPIC_TX;
            break;

        case TOPIC_TX_2:
            AT_MQTT_TOPIC_2(write_mqtt_topic);
            sim_state = TOPIC_RX_2;
            break;
    
        case TOPIC_RX_2:
            read_AT_processor();
            if (OK_FLAG) sim_state = PAYLOAD_TX;
            else if (ERROR_FLAG) sim_state = TOPIC_TX;
            break;

        case PAYLOAD_TX:
            AT_MQTT_PAYLOAD(CLIENT_NUM,write_mqtt_message);  
            sim_state = PAYLOAD_RX;
            break;

        case PAYLOAD_RX:
            read_AT_processor();
            if (INPUT_FLAG) sim_state = PAYLOAD_TX_2;
            else if (ERROR_FLAG) sim_state = TOPIC_TX;  
            break;

        case PAYLOAD_TX_2:
            AT_MQTT_PAYLOAD_2(write_mqtt_message);  
            sim_state = PAYLOAD_RX_2;
            break;

        case PAYLOAD_RX_2:
            read_AT_processor();
            if (OK_FLAG) sim_state = PUBLISH_TX;
            else if (ERROR_FLAG) sim_state = TOPIC_TX; 
            break;
    
        case PUBLISH_TX:
            AT_PUBLISH(CLIENT_NUM,QOS,RETAIN); 
            sim_state = PUBLISH_RX;
            break;
    
        case PUBLISH_RX:
            read_AT_processor();
            if (OK_FLAG) sim_state = TXENABLED;
            else if (ERROR_FLAG) sim_state = PUBLISH_TX;
            break;
    }
}

//Funciones para que otros modulos puedan realizar las comunicaciones mqtt
bool read_MQTT_available(void){
    //Funcion para revisar si hay algo para leer
    return (strlen(buffer_mqtt) > 0);
}

void read_MQTT(char* message){
    //printf("rx_mqtt antes: %s", buffer_mqtt);
    strncpy(message, buffer_mqtt, MAX_TOPIC_LEN - 1);
    message[MAX_TOPIC_LEN - 1] = '\0';
    buffer_mqtt[0] = '\0';
    //printf("rx_mqtt despues: %s", buffer_mqtt);
}

void write_MQTT(const char* topic, const char* message) {
    //Funcion bloqueante, espera a que sea possible enviar y que prepara el topic y el payload tx para
    while (sim_state != TXENABLED) {}
    sim_state = TOPIC_TX;
    strncpy(write_mqtt_topic, topic, MAX_TOPIC_LEN - 1);
    strncpy(write_mqtt_message, message, MQTT_BUF_SIZE - 1);
}



