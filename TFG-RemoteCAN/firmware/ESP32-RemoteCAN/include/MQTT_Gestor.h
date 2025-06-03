#ifndef MQTT_GESTOR_H
#define MQTT_GESTOR_H

//bool contains_ERROR(const char *str);
//bool contains_OK(const char *str);
//bool contains_CLOST(const char *str);
//void contains_MQTT(char *buffer_uart);
//void read_AT_processor(void);

void sim_state_machine(void);
bool read_MQTT_available(void);
void read_MQTT(char* message);
void write_MQTT(const char* topic,const char* message);

#endif // MQTT_GESTOR_H
