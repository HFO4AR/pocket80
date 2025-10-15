#ifndef __AK80_8_H
#define __AK80_8_H

	
	
#include "stm32f4xx_hal.h"


typedef enum {
	CAN_PACKET_SET_DUTY = 0,         //占空比模式
	CAN_PACKET_SET_CURRENT,         //电流环模式
	CAN_PACKET_SET_CURRENT_BRAKE,   // 电流刹车模式
	CAN_PACKET_SET_RPM,             // 转速模式
	CAN_PACKET_SET_POS,              // 位置模式
	CAN_PACKET_SET_ORIGIN_HERE,      //设置原点模式
	CAN_PACKET_SET_POS_SPD,          //位置速度环模式
} CAN_PACKET_ID;




void comm_can_set_duty(uint8_t controller_id, float duty);
void comm_can_set_current(uint8_t controller_id, float current);
void comm_can_set_cb(uint8_t controller_id, float current);
void comm_can_set_rpm(uint8_t controller_id, float rpm);
void comm_can_set_pos(uint8_t controller_id, float pos);
void comm_can_set_origin(uint8_t controller_id, uint8_t set_origin_mode);
void comm_can_set_pos_spd(uint8_t controller_id, float pos,int16_t spd, int16_t RPA );
void motor_receive(float* motor_pos,float* motor_spd,float* motor_cur,int8_t *motor_temp,int8_t *motor_error);
uint8_t recesive_mesage_can(uint8_t *rx_data);
//uint8_t recesive_mesage_can(uint8_t *rx_data);
//void comm_can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len);






#endif
