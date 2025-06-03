#ifndef MQTT_GESTOR_H
#define MQTT_GESTOR_H

bool contieneERROR(const char *str);
bool contieneOK(const char *str);
bool contieneCLOST(const char *str);
void read_MQTT(char *buffer_uart);
void read_AT_processor(void);

void general_state_machine(void);
bool rx_mqtt_available(void);
void rx_mqtt(char* message);
void tx_mqtt(const char* topic,const char* message);

#endif // MQTT_GESTOR_H
