#ifndef CAN_GESTOR_H
#define CAN_GESTOR_H

void init_CAN(void);

void stop_CAN(void);

void write_CAN(int ID, int mode, int rtr, int payload_length, uint8_t* payload);

bool read_CAN(twai_message_t *message);

void write_CAN_speed(void);

void write_CAN_rpm(void);

#endif // CAN_GESTOR_H