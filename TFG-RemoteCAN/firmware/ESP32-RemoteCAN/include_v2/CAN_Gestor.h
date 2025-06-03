#ifndef CAN_GESTOR_H
#define CAN_GESTOR_H

void init_can(void);

void stop_can(void);

void transmit_can(int ID, int mode, int rtr, int payload_length, uint8_t* payload);

bool receive_can(twai_message_t *message);

void tx_speed(void);

void tx_rpm(void);

#endif // CAN_GESTOR_H