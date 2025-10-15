//
// Created by nyuki on 2025/10/15.
//

#include "uart_bsp.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "usart.h"

#include "main.h"
#include "ak80_8.h"
#include "searchDuck.h"

int parseTwoFloats(const char *input, double *num1, double *num2) {
    // 检查输入参数有效性
    if (input == NULL || num1 == NULL || num2 == NULL) {
        return 1; // 参数无效
    }

    // 复制原字符串（避免修改输入，因strtok会修改原字符串）
    char *str = strdup(input);
    if (str == NULL) {
        return 1; // 内存分配失败
    }

    const char *delim = " "; // 分隔符：空格
    char *token;
    int count = 0; // 记录分割出的数字个数

    // 分割第一个数字
    token = strtok(str, delim);
    if (token == NULL) {
        free(str);
        return 2; // 未找到第一个数字（格式错误）
    }

    // 转换第一个数字为浮点数
    char *endptr;
    errno = 0; // 重置错误码
    *num1 = strtod(token, &endptr);
    if (endptr == token || errno != 0) { // 转换失败（无有效数字或溢出）
        free(str);
        return 3;
    }
    count++;

    // 分割第二个数字
    token = strtok(NULL, delim);
    if (token == NULL) {
        free(str);
        return 2; // 未找到第二个数字（格式错误）
    }

    // 转换第二个数字为浮点数
    errno = 0;
    *num2 = strtod(token, &endptr);
    if (endptr == token || errno != 0) {
        free(str);
        return 3;
    }
    count++;

    // 检查是否有多余的内容（超过两个数字）
    token = strtok(NULL, delim);
    if (token != NULL) {
        free(str);
        return 2; // 存在多余内容（格式错误）
    }

    free(str);
    return 0; // 成功
}



extern char uart_rx[1];
char data[50];
int data_i=0;
double yaw_err=0;
double pitch_err=0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
        if (uart_rx[0]=='\n') {
            data[data_i] = '\0';
            HAL_UART_Transmit_IT(&huart1, (uint8_t*)data, strlen(data));
            parseTwoFloats(data, &yaw_err, &pitch_err);

            //重置接收
            memset(data, 0, sizeof(data));
            data_i=0;

        }else {
            data[data_i]=uart_rx[0];
            data_i++;
        }

        HAL_UART_Receive_IT(&huart1,uart_rx,sizeof(uart_rx));
    }
}

