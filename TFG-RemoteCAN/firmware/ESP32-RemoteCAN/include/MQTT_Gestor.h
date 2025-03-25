#ifndef MQTT_GESTOR_H
#define MQTT_GESTOR_H

// Declaración de funciones
void sim_state_machine(void);
void mqtt_state_machine(void);
void general_state_machine(void);
void tx_mqtt(const char* topic,const char* message);

#endif // MQTT_GESTOR_H
