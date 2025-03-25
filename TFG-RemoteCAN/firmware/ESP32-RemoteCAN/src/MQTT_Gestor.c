#include <stdio.h>
#include <string.h>
#include "AT_Gestor.h"
#include "MQTT_Gestor.h"

#define MAX_TOPIC_LEN 100
#define MAX_MSG_LEN 256

#define PIN 5581
#define CID 1
#define IP "IP"
#define APN "jazzinternet"
#define STATE 1
#define CLIENT_NUM 0
#define CLIENT_ID "client test0"
#define TOPIC "test/topic"
#define MESSAGE "Hello MQTT!"
#define BROKER_IP "37.15.75.44"
#define KEEP_ALIVE 60
#define CLEAN_SESSION 1
#define QOS 1
#define BUF_SIZE 256  // Tamaño del buffer
#define RETAIN 60


// Definición de los enums
typedef enum {
    START,
    CPIN,
    CSQ,
    CREG_Q,
    CGREG_Q,
    CPSI_Q,
    CGDCONT_D,
    CGACT_D,
    CGACT_Q,
    FINISH
} sim_s;

typedef enum {
    START_MQTT,
    ACCQ,
    CONNECT,
    SUBSCRIBE,
    FINISH_MQTT
} mqtt_s;

typedef enum {
    START_COM,
    SMSIM,
    SMMQTT,
    RXMQTT,
    TXMQTT
} general_s;

// Variables de estado
static sim_s sim_state = START;
static mqtt_s mqtt_state = START_MQTT;
static general_s gen_state = START_COM;
char tx_topic[MAX_TOPIC_LEN];  // Buffer para almacenar el topic
char tx_message[MAX_MSG_LEN];  // Buffer para almacenar el mensaje

static char mqtt_buffer[BUF_SIZE];  // Variable global para almacenar la cadena

// Función para la máquina de estados SIM
void sim_state_machine() {
    switch (sim_state) {
        case START:
            if (AT()) sim_state = CPIN;
            break;
        case CPIN:
            AT_CPIN_D(PIN);
            if (AT_CPIN_Q()) sim_state = CSQ;
            break;
        case CSQ:
            if (AT_CSQ()) sim_state = CREG_Q;
            break;
        case CREG_Q:
            if (AT_CREG_Q()) sim_state = CGREG_Q;
            break;
        case CGREG_Q:
            if (AT_CGREG_Q()) sim_state = CPSI_Q;
                break;
        case CPSI_Q:
            if (AT_CPSI_Q()) sim_state = CGDCONT_D;
            break;
        case CGDCONT_D:
            if (AT_CGDCONT_D(CID, IP, APN)) sim_state = CGACT_D;
            break;
        case CGACT_D:
            if (AT_CGACT_D(STATE, CID)) sim_state = CGACT_Q;
            break;
        case CGACT_Q:
            if (AT_CGACT_Q()) sim_state = FINISH;
            break;
        case FINISH:
            break;
    }
}

// Función para la máquina de estados MQTT
void mqtt_state_machine() {
    switch (mqtt_state) {
        case START_MQTT:
            if (AT_MQTT_START()) mqtt_state = ACCQ;
            break;
        case ACCQ:
            if (AT_MQTT_ACCQ(CLIENT_NUM, CLIENT_ID)) mqtt_state = CONNECT;
            break;
        case CONNECT:
            if (AT_MQTT_CONNECT(CLIENT_NUM, BROKER_IP, KEEP_ALIVE, CLEAN_SESSION)) mqtt_state = SUBSCRIBE;
            break;
        case SUBSCRIBE:
            if (AT_MQTT_SUBSCRIBE(CLIENT_NUM, TOPIC, QOS)) mqtt_state = FINISH_MQTT;
            break;
        case FINISH_MQTT:
            break;
    }
}

void general_state_machine(){
    switch (gen_state) {
        case START_COM:
            init_sim();
            gen_state = SMSIM;
            break;
        case SMSIM:
            if (sim_state != FINISH) {
                sim_state_machine();
            } 
            else {
                gen_state = SMMQTT;
            }
            break;
        case SMMQTT:
            if (mqtt_state != FINISH_MQTT) {
                mqtt_state_machine();
            } 
            else {
                gen_state = RXMQTT;
            }
            break;
        case RXMQTT:
            if (!read_MQTT(mqtt_buffer)){
                gen_state = SMMQTT;
                mqtt_state = START;
            }
            break;
        case TXMQTT:
            AT_MQTT_TOPIC(CLIENT_NUM,tx_topic);
            AT_MQTT_PAYLOAD(CLIENT_NUM,tx_message);
            AT_PUBLISH(CLIENT_NUM,QOS,RETAIN);
            gen_state = RXMQTT;
            break;
    }
}

void tx_mqtt(const char* topic, const char* message) {
    if (gen_state == RXMQTT) {
        gen_state = TXMQTT;
        strncpy(tx_topic, topic, MAX_TOPIC_LEN - 1);
        tx_topic[MAX_TOPIC_LEN - 1] = '\0';  // Asegurar terminación NULL
        
        strncpy(tx_message, message, MAX_MSG_LEN - 1);
        tx_message[MAX_MSG_LEN - 1] = '\0';
    }
}