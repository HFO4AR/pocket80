#include "ak80_8.h"
#include "main.h"
#include "can.h"
//#include "usart.h"
#include "gpio.h"
 uint8_t tx_data[8];
 // uint8_t rx_data[8];

	float motor_pos_2[2] = {0.0,0.0};
	float motor_spd_2[2] = {0.0,0.0};
	float motor_cur_2[2] = {0.0,0.0};
	int8_t motor_temp_2[2] = {0.0,0.0};
	int8_t motor_error_2[2] = {0.0,0.0};

	float motor_pos=0;
	float motor_spd=0;
	float motor_cur=0;
	int8_t motor_temp=0;
	int8_t motor_error=0;


void comm_can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len) {    //can发送
	uint32_t tx_mail =CAN_TX_MAILBOX0;//邮箱0发送
	uint8_t i=0;
	if (len > 8) {
		len = 8;
}
  CAN_TxHeaderTypeDef TxMessage;
 	TxMessage.StdId = 0;
	TxMessage.IDE = CAN_ID_EXT;
	TxMessage.ExtId = id;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = len;

  for(i=0;i<len;i++)tx_data[i]=data[i];
  HAL_CAN_AddTxMessage(&hcan2,&TxMessage, tx_data,&tx_mail);

}



uint8_t recesive_mesage_can(uint8_t *rx_data)      //can接受
{
	CAN_RxHeaderTypeDef   RxMessage;
	if(HAL_CAN_GetRxFifoFillLevel(&hcan2,CAN_RX_FIFO0)==0)return 0;
	HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&RxMessage,rx_data);
	return RxMessage.DLC;
}




void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t *index) { //用于将一个32位整数（int32_t）追加到字节数组（uint8_t* buffer）中
	buffer[(*index)++] = number >> 24;
	buffer[(*index)++] = number >> 16;
	buffer[(*index)++] = number >> 8;
	buffer[(*index)++] = number;
}

void buffer_append_int16(uint8_t* buffer, int16_t number, int16_t *index) { //用于将一个16位整数（int16_t）追加到字节数组（uint8_t* buffer）中
	buffer[(*index)++] = number >> 8;
	buffer[(*index)++] = number;
}


void comm_can_set_duty(uint8_t controller_id, float duty) {  //占空比
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(duty * 100000.0f), &send_index);
	comm_can_transmit_eid(controller_id |
((uint32_t)CAN_PACKET_SET_DUTY << 8), buffer, send_index);
}


void comm_can_set_current(uint8_t controller_id, float current) {  //电流环模式
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(current * 1000.0f), &send_index);
	comm_can_transmit_eid(controller_id |
((uint32_t)CAN_PACKET_SET_CURRENT << 8), buffer, send_index);
}


void comm_can_set_cb(uint8_t controller_id, float current) {    //电流刹车模式
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(current * 1000.0f), &send_index);
	comm_can_transmit_eid(controller_id |
			((uint32_t)CAN_PACKET_SET_CURRENT_BRAKE << 8), buffer, send_index);
}


void comm_can_set_rpm(uint8_t controller_id, float rpm) {   //速度环模式
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)rpm, &send_index);
	comm_can_transmit_eid(controller_id |
				((uint32_t)CAN_PACKET_SET_RPM << 8), buffer, send_index);
}


void comm_can_set_pos(uint8_t controller_id, float pos) {   //位置环模式
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(pos * 10000.0f), &send_index);
	comm_can_transmit_eid(controller_id |
			((uint32_t)CAN_PACKET_SET_POS << 8), buffer, send_index);
}


void comm_can_set_origin(uint8_t controller_id, uint8_t set_origin_mode) {   //设置原点模式
int32_t send_index = 1;
	uint8_t buffer;
  buffer=set_origin_mode;
	comm_can_transmit_eid(controller_id |
			((uint32_t) CAN_PACKET_SET_ORIGIN_HERE << 8), &buffer, send_index);
}

void comm_can_set_pos_spd(uint8_t controller_id, float pos, int16_t spd, int16_t RPA) {
	//位置速度环模式
	int32_t send_index = 0;
	int16_t send_index1 = 4;
	uint8_t buffer[8];
	buffer_append_int32(buffer, (int32_t) (pos * 10000.0f), &send_index);
	buffer_append_int16(buffer, spd / 10.0, &send_index1);

	buffer_append_int16(buffer, RPA / 10.0, &send_index1);
	comm_can_transmit_eid(controller_id |
	                      ((uint32_t) CAN_PACKET_SET_POS_SPD << 8), buffer, send_index1);
}

// void motor_receive(float* motor_pos,float* motor_spd,float* motor_cur,int8_t *motor_temp,int8_t *motor_error)
//   {
//     recesive_mesage_can(rx_data);
//    int16_t pos_int = (rx_data[0]<<8)|rx_data[1];
// 	 int16_t spd_int = (rx_data[2]<<8)|rx_data[3];
//    int16_t cur_int = (rx_data[4]<<8)|rx_data[5];
//     *motor_pos= (float)( pos_int * 0.1f); //电机位置
//     *motor_spd= (float)( spd_int * 10.0f);//电机速度
//     *motor_cur= (float) ( cur_int * 0.01f);//电机电流
//     *motor_temp= rx_data[6] ;//电机温度
//     *motor_error= rx_data[7] ;//电机故障码
//   }


	
//	uint8_t recesive_mesage_can(uint8_t *rx_data)      //can接受
//{
//	CAN_RxHeaderTypeDef   RxMessage;
//	if(HAL_CAN_GetRxFifoFillLevel(&hcan2,CAN_RX_FIFO0)==0)return 0;
//	HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&RxMessage,rx_data);
//	return RxMessage.DLC;
//}
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//	CAN_RxHeaderTypeDef RX_Header;																					//定义数据帧的帧头
//																											//接收存放数据帧数据的数组
//	
//	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RX_Header,rx_data);						//把can接收到的数据帧传入局部变量
//	
//	static uint8_t i = 0;
//	if(hcan==&hcan1){																												//can1
//     int16_t pos_int = (rx_data[0]<<8)|rx_data[1];
//	   int16_t spd_int = (rx_data[2]<<8)|rx_data[3];
//     int16_t cur_int = (rx_data[4]<<8)|rx_data[5];                                                                              
//    motor_pos= (float)( pos_int * 0.1f); //电机位置                                                          
//    motor_spd= (float)( spd_int * 10.0f);//电机速度
//    motor_cur= (float) ( cur_int * 0.01f);//电机电流
//    motor_temp= rx_data[6] ;//电机温度
//    motor_error= rx_data[7] ;//电机故障码 
//		if (motor_pos <=90)
//		{
//		 motor_pos_2[0] = motor_pos;
//     motor_spd_2[0] = motor_spd;
//     motor_cur_2[0] = motor_cur;
//     motor_temp_2[0] = motor_temp;
//     motor_error_2[0] = motor_error;
//		}
//		else 
//		{
//		 motor_pos_2[1] = motor_pos;
//     motor_spd_2[1] = motor_spd;
//     motor_cur_2[1] = motor_cur;
//     motor_temp_2[1] = motor_temp;
//     motor_error_2[1] = motor_error;
//			
//		}
//		
//		
//	 }
//	else{
//     int16_t pos_int = (rx_data[0]<<8)|rx_data[1];
//	   int16_t spd_int = (rx_data[2]<<8)|rx_data[3];
//     int16_t cur_int = (rx_data[4]<<8)|rx_data[5];                                                                              
//    motor_pos= (float)( pos_int * 0.1f); //电机位置                                                          
//    motor_spd= (float)( spd_int * 10.0f);//电机速度
//    motor_cur= (float) ( cur_int * 0.01f);//电机电流
//    motor_temp= rx_data[6] ;//电机温度
//    motor_error= rx_data[7] ;//电机故障码 
//		
//	}	//调用函数把数据存入结构体数组
//	
//}



