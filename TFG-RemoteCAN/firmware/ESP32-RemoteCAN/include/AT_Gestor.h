#ifndef AT_GESTOR_H
#define AT_GESTOR_H

#include <stdbool.h>

void init_sim(void);
void write_AT(const char *comand);
void read_AT(char* rx_buffer);

void AT(void);
void AT_CPIN_D(int pin);
void AT_CPIN_Q(void);
void AT_CSQ(void);
void AT_CREG_Q(void);
void AT_CGREG_Q(void);
void AT_CPSI_Q(void);
void AT_CGDCONT_D(int cid, const char* ip, const char* APN);
void AT_CGACT_D(int state, int cid);
void AT_CGACT_Q(void);
void AT_MQTT_START(void);
void AT_MQTT_ACCQ(int client_num, const char* client_id, int server_type);
void AT_MQTT_WILLTOPIC(int client_num, const char* topic);
void AT_MQTT_WILLMSG(int client_num, int qos, const char* message);
void AT_MQTT_CONNECT(int client_num, const char* broker_ip, const char* broker_port,int keep_alive, int clean_session);
void AT_MQTT_SUBSCRIBE(int client_num, const char* topic, int qos);
void AT_MQTT_SUBSCRIBE_2(const char* topic);
void AT_MQTT_DISCONNECT(int client_num, int timeout);
void AT_MQTT_RELEASE(int client_num);
void AT_MQTT_STOP(void);
void AT_MQTT_TOPIC(int client_num, const char* topic); 
void AT_MQTT_TOPIC_2(const char* topic); 
void AT_MQTT_PAYLOAD(int client_num, const char* payload); 
void AT_MQTT_PAYLOAD_2(const char* payload); 
void AT_PUBLISH(int client_num, int qos, int retain);

#endif // AT_GESTOR_H
