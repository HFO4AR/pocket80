//
// Created by nyuki on 2025/10/15.
//

#ifndef POCKET80_SEARCHDUCK_H
#define POCKET80_SEARCHDUCK_H
#include <stdint.h>

typedef struct {
    int id;
    float pos;
    float spd;
    float cur;
    float init_pos;
    float current_pos;
    int8_t temp;
    int8_t error;
}ak80_data_t;
void search_duck_init(ak80_data_t *ak80_data,int id);
void get_ak80_data(ak80_data_t *ak80_data);
void search_duck(double yaw_err,double pitch_err);
#endif //POCKET80_SEARCHDUCK_H