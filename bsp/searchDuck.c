//
// Created by nyuki on 2025/10/15.
//

#include "searchDuck.h"
#include "ak80_8.h"

extern float motor_pos;
ak80_data_t ak80_pitch_data;
ak80_data_t ak80_yaw_data;
void get_ak80_data(ak80_data_t *ak80_data) {
    motor_receive(&ak80_data->pos, &ak80_data->spd, &ak80_data->cur, &ak80_data->temp, &ak80_data->error);
}

void search_duck_init(ak80_data_t *ak80_data,int id) {
    ak80_data->id=id;
    get_ak80_data(ak80_data);
    ak80_data->init_pos=ak80_data->pos;

}

void search_duck(double yaw_err,double pitch_err) {
    get_ak80_data(&ak80_pitch_data);
    comm_can_set_pos_spd(ak80_pitch_data.id,ak80_pitch_data.init_pos+pitch_err,3000,200);
    // comm_can_set_pos(ak80_pitch_data.id,ak80_pitch_data.pos+pitch_err);
}