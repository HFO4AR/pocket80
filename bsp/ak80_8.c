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


void comm_can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len) {    //can����
	uint32_t tx_mail =CAN_TX_MAILBOX0;//����0����
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



uint8_t recesive_mesage_can(uint8_t *rx_data)      //can����
{
	CAN_RxHeaderTypeDef   RxMessage;
	if(HAL_CAN_GetRxFifoFillLevel(&hcan2,CAN_RX_FIFO0)==0)return 0;
	HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&RxMessage,rx_data);
	return RxMessage.DLC;
}




void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t *index) { //���ڽ�һ��32λ������int32_t��׷�ӵ��ֽ����飨uint8_t* buffer����
	buffer[(*index)++] = number >> 24;
	buffer[(*index)++] = number >> 16;
	buffer[(*index)++] = number >> 8;
	buffer[(*index)++] = number;
}

void buffer_append_int16(uint8_t* buffer, int16_t number, int16_t *index) { //���ڽ�һ��16λ������int16_t��׷�ӵ��ֽ����飨uint8_t* buffer����
	buffer[(*index)++] = number >> 8;
	buffer[(*index)++] = number;
}


void comm_can_set_duty(uint8_t controller_id, float duty) {  //ռ�ձ�
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(duty * 100000.0f), &send_index);
	comm_can_transmit_eid(controller_id |
((uint32_t)CAN_PACKET_SET_DUTY << 8), buffer, send_index);
}


void comm_can_set_current(uint8_t controller_id, float current) {  //������ģʽ
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(current * 1000.0f), &send_index);
	comm_can_transmit_eid(controller_id |
((uint32_t)CAN_PACKET_SET_CURRENT << 8), buffer, send_index);
}


void comm_can_set_cb(uint8_t controller_id, float current) {    //����ɲ��ģʽ
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(current * 1000.0f), &send_index);
	comm_can_transmit_eid(controller_id |
			((uint32_t)CAN_PACKET_SET_CURRENT_BRAKE << 8), buffer, send_index);
}


void comm_can_set_rpm(uint8_t controller_id, float rpm) {   //�ٶȻ�ģʽ
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)rpm, &send_index);
	comm_can_transmit_eid(controller_id |
				((uint32_t)CAN_PACKET_SET_RPM << 8), buffer, send_index);
}


void comm_can_set_pos(uint8_t controller_id, float pos) {   //λ�û�ģʽ
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(pos * 10000.0f), &send_index);
	comm_can_transmit_eid(controller_id |
			((uint32_t)CAN_PACKET_SET_POS << 8), buffer, send_index);
}


void comm_can_set_origin(uint8_t controller_id, uint8_t set_origin_mode) {   //����ԭ��ģʽ
int32_t send_index = 1;
	uint8_t buffer;
  buffer=set_origin_mode;
	comm_can_transmit_eid(controller_id |
			((uint32_t) CAN_PACKET_SET_ORIGIN_HERE << 8), &buffer, send_index);
}

void comm_can_set_pos_spd(uint8_t controller_id, float pos, int16_t spd, int16_t RPA) {
	//λ���ٶȻ�ģʽ
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
//     *motor_pos= (float)( pos_int * 0.1f); //���λ��
//     *motor_spd= (float)( spd_int * 10.0f);//����ٶ�
//     *motor_cur= (float) ( cur_int * 0.01f);//�������
//     *motor_temp= rx_data[6] ;//����¶�
//     *motor_error= rx_data[7] ;//���������
//   }


	
//	uint8_t recesive_mesage_can(uint8_t *rx_data)      //can����
//{
//	CAN_RxHeaderTypeDef   RxMessage;
//	if(HAL_CAN_GetRxFifoFillLevel(&hcan2,CAN_RX_FIFO0)==0)return 0;
//	HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&RxMessage,rx_data);
//	return RxMessage.DLC;
//}
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//	CAN_RxHeaderTypeDef RX_Header;																					//��������֡��֡ͷ
//																											//���մ������֡���ݵ�����
//	
//	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RX_Header,rx_data);						//��can���յ�������֡����ֲ�����
//	
//	static uint8_t i = 0;
//	if(hcan==&hcan1){																												//can1
//     int16_t pos_int = (rx_data[0]<<8)|rx_data[1];
//	   int16_t spd_int = (rx_data[2]<<8)|rx_data[3];
//     int16_t cur_int = (rx_data[4]<<8)|rx_data[5];                                                                              
//    motor_pos= (float)( pos_int * 0.1f); //���λ��                                                          
//    motor_spd= (float)( spd_int * 10.0f);//����ٶ�
//    motor_cur= (float) ( cur_int * 0.01f);//�������
//    motor_temp= rx_data[6] ;//����¶�
//    motor_error= rx_data[7] ;//��������� 
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
//    motor_pos= (float)( pos_int * 0.1f); //���λ��                                                          
//    motor_spd= (float)( spd_int * 10.0f);//����ٶ�
//    motor_cur= (float) ( cur_int * 0.01f);//�������
//    motor_temp= rx_data[6] ;//����¶�
//    motor_error= rx_data[7] ;//��������� 
//		
//	}	//���ú��������ݴ���ṹ������
//	
//}



