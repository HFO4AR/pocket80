//
// Created by nyuki on 2025/10/15.
//

#include "searchDuck.h"

#include <stdlib.h>
#include <string.h>

#include "ak80_8.h"
#include "can.h"

#define MOTOR_ENABLE 1
#define MOTOR_DEADBAND 0.01
#define MOTOR_MAX_YAW_ANGLE 90
#define MOTOR_MAX_PITCH_ANGLE 90
extern float motor_pos;
ak80_data_t ak80_pitch_data;
ak80_data_t ak80_yaw_data;


// void get_ak80_data(ak80_data_t *ak80_data) {
//     motor_receive(&ak80_data->pos, &ak80_data->spd, &ak80_data->cur, &ak80_data->temp, &ak80_data->error);
// }

union data {
    uint8_t input[8];

    struct {
        int8_t error;
        int8_t temp;
        int16_t cur;
        int16_t spd;
        int16_t pos;
    } read;
};
void get_ak80_data(ak80_data_t *ak80_data) {
    union data rx_data;
    CAN_RxHeaderTypeDef   RxMessage;
    HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&RxMessage,rx_data.input);
    uint8_t stack[8];
    for (int i = 0; i < 8; i++) {
        stack[i]=rx_data.input[i];
    }
    for (int i=0;i<8;i++) {
        rx_data.input[i]=stack[7-i];
    }
    if (RxMessage.ExtId==(0x2900+ak80_data->id)) {
        ak80_data->pos=(float)(rx_data.read.pos)*0.1;
        ak80_data->spd=(float)(rx_data.read.spd)*10.0;
        ak80_data->cur=(float)(rx_data.read.cur)*0.01;
        ak80_data->temp=rx_data.read.temp;
        ak80_data->error=rx_data.read.error;
    }
}

void search_duck_init(ak80_data_t *ak80_data,int id) {
    ak80_data->id=id;
    ak80_data->current_pos=10;
    // HAL_Delay(5000);
    comm_can_set_pos_spd(ak80_data->id,0,0,0);
    get_ak80_data(ak80_data);
    while (ak80_data->temp==0) {
        get_ak80_data(ak80_data);
        HAL_Delay(10);
    };
    ak80_data->init_pos=ak80_data->pos;
    ak80_data->current_pos=ak80_data->pos;
    if (MOTOR_ENABLE) {
        comm_can_set_pos_spd(ak80_data->id,ak80_data->current_pos+1,3000,200);
    }
    // comm_can_set_pos_spd(ak80_data->id,ak80_data->current_pos+1,3000,200);
    HAL_Delay(100);
}

void search_duck(double yaw_err,double pitch_err) {
    yaw_err=yaw_err*0.1;
    pitch_err=pitch_err*0.1;

    //死区检测
    if (abs(yaw_err)<MOTOR_DEADBAND) {
        yaw_err=0;
    }
    if (abs(pitch_err)<MOTOR_DEADBAND) {
        pitch_err=0;
    }

    //限位
    if (ak80_yaw_data.current_pos + yaw_err > ak80_yaw_data.init_pos + MOTOR_MAX_YAW_ANGLE) {
        yaw_err = ak80_yaw_data.init_pos+MOTOR_MAX_YAW_ANGLE - ak80_yaw_data.current_pos;
    }else if (ak80_yaw_data.current_pos
        + yaw_err < ak80_yaw_data.init_pos - MOTOR_MAX_YAW_ANGLE) {
        yaw_err = ak80_yaw_data.init_pos-MOTOR_MAX_YAW_ANGLE - ak80_yaw_data.current_pos;
    }
    if (ak80_pitch_data.current_pos + pitch_err > ak80_pitch_data.init_pos + MOTOR_MAX_PITCH_ANGLE) {
        pitch_err = ak80_yaw_data.init_pos+MOTOR_MAX_PITCH_ANGLE - ak80_pitch_data.current_pos;
    }else if (ak80_pitch_data.
        current_pos + pitch_err < ak80_pitch_data.init_pos - MOTOR_MAX_PITCH_ANGLE) {
        pitch_err = ak80_yaw_data.init_pos-MOTOR_MAX_PITCH_ANGLE - ak80_pitch_data.current_pos;
    }


    if (MOTOR_ENABLE) {
        comm_can_set_pos_spd(ak80_pitch_data.id,ak80_pitch_data.current_pos+pitch_err,3000,200);
    }
    // comm_can_set_pos_spd(ak80_pitch_data.id,ak80_pitch_data.current_pos+pitch_err,6000,3000);
    get_ak80_data(&ak80_pitch_data);
    if (MOTOR_ENABLE) {
        comm_can_set_pos_spd(ak80_yaw_data.id,ak80_yaw_data.current_pos+yaw_err,3000,200);
    }
    // comm_can_set_pos_spd(ak80_yaw_data.id,ak80_yaw_data.current_pos+yaw_err,3000,200);
    get_ak80_data(&ak80_yaw_data);
    // comm_can_set_pos(ak80_pitch_data.id,ak80_pitch_data.pos+pitch_err);
    // HAL_Delay(100);
}