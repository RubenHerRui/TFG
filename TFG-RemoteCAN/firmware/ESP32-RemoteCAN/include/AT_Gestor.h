#ifndef AT_GESTOR_H
#define AT_GESTOR_H

#include <stdbool.h>

void init_sim(void);
void write_AT(const char *comand);
void read_AT(char* rx_buffer, size_t buffer_size);

bool contieneOK(const char *str);
bool contieneCLOST(const char *str);

bool read_MQTT(char *output);
bool AT(void);
void AT_CPIN_D(int pin);
bool AT_CPIN_Q(void);
bool AT_CSQ(void);
bool AT_CREG_Q(void);
bool AT_CGREG_Q(void);
bool AT_CPSI_Q(void);
bool AT_CGDCONT_D(int cid, const char* ip, const char* APN);
bool AT_CGACT_D(int state, int cid);
bool AT_CGACT_Q(void);
bool AT_MQTT_START(void);
bool AT_MQTT_ACCQ(int client_num, const char* client_id);
bool AT_MQTT_WILLTOPIC(int client_num, const char* topic);
bool AT_MQTT_WILLMSG(int client_num, int qos, const char* message);
bool AT_MQTT_CONNECT(int client_num, const char* broker_ip, int keep_alive, int clean_session);
bool AT_MQTT_SUBSCRIBE(int client_num, const char* topic, int qos);
bool AT_MQTT_DISCONNECT(int client_num, int timeout);
bool AT_MQTT_RELEASE(int client_num);
bool AT_MQTT_STOP(void);
bool AT_MQTT_TOPIC(int client_num, const char* topic); 
bool AT_MQTT_PAYLOAD(int client_num, const char* payload); 
bool AT_PUBLISH(int client_num, int qos, int retain);

#endif // AT_GESTOR_H
