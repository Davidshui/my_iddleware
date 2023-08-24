/**
 * @file IR.c
 * @author your name (you@domain.com)
 * @brief 1.�������ݸ�ʽ: ������ + ������ + ���ݷ���  + ������	
 * 			�����룺9ms�ز� + 4.5ms����  
 * 		  2.��������Կ��ƶ������ƣ�����Ʒ����ֻ��Ҫ����һ������ơ�
 * @version 0.1
 * @date 2023-06-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"
#include "dev_infrared_radia.h"


//uint8_t IR_data_array[20] = {0};


//�����������   ��Ӳ��M20���ţ��ߵ�ƽ���������
void bsp_IR_init(void)
{
	GPIO_Init(IR_PORT, IR_PIN, 1, 0, 0);         /* IR�˿�*//*pin��*//* ��� *//* ���� *//* ������ */
	GPIO_ClrBit(IR_PORT, IR_PIN);

	// while(0)
	// {	
		// Send_ENC_Message(IR_data_array,1,1);
	// }
}

//

/**
 * @brief send a frame buffer 
 * 
 * @param p_data addr of data's head 
 * @param data_len data length
 * @param ir_choose choose a ir_led from all ir_led.  In the process, it can be set to any value
 */
void Send_ENC_Message(uint8_t *p_data,uint8_t data_len,uint8_t ir_choose)
{
	//shut up the module of gesture_recognition
	bsp_gesture_init(0);
	bsp_gesture_init(0);


	//�ȷ���������:��9ms�ĸߵ�ƽ���壬�����4.5ms�ĵ͵�ƽ
	Send_IR(342,ir_choose);   //9ms�����ز��ź� ���� = 9000us/26.3us = 342
	NoSend_IR(171,ir_choose); //4.5ms�������ز��źţ����� = 4500us/26.3us = 171

	//��������
	GetByte_And_SendByte(p_data,data_len,ir_choose);   //ȡ��ÿһλ������
}


//ȡ�������е�ÿһλ������
void GetByte_And_SendByte(uint8_t *p_data, uint8_t data_len,uint8_t ir_choose)
{
	unsigned int temp,i,j; //�����м����
	uint8_t data_tmp;
	//���������루��ַ��͵�ַ��ķ��룩
	for(j = 0; j<data_len; j++)
	{
		data_tmp = p_data[j];
		for(i = 0;i<8;i++){  //ѭ��16λ�����е�ÿһλ
			temp = data_tmp&(0x01<<i); //ͨ��������ȡ���������λ
			if(temp){  //�����1ִ��
				Send_NEC_1(ir_choose);
			}else{     //��0ִ��
				Send_NEC_0(ir_choose);
			}
			//temp = temp>>1;   //����һλ��ȡ����һλ����
		}
		
		data_tmp = ~(p_data[j]);
		for(i = 0;i<8;i++){  //ѭ��8λ�����е�ÿһλ
			temp = data_tmp & (0x01<<i);
			if(temp){  //�����1ִ��
				Send_NEC_1(ir_choose);
			}else{     //��0ִ��
				Send_NEC_0(ir_choose);
			}
			//temp = temp>>1;   //����һλ��ȡ����һλ����
		}
		//���ݸ�ֵ���м����,ȡ�����λ
		//����������
	}
	
	//���ͽ�����
	Send_stop(ir_choose);
	Send_stop(ir_choose);
}


void Send_stop(uint8_t ir_choose)
{
	Send_NEC_0(ir_choose);

	//open the module of gesture_recognition
	bsp_gesture_init(1);
	bsp_gesture_init(1);
	
}




/*
���Ͷ��������� 0   �߼���1��Ϊ560us��+1680us���߼���0��Ϊ560us��+560us�ߡ�
*/
void Send_NEC_0(uint8_t ir_choose)
{
	Send_IR(21,ir_choose); 	   //�����ز��ź� 0.56ms, Ҳ���Ƿ��ͺ����21������
	NoSend_IR(21,ir_choose);   //�������ز��ź�0.56ms,Ҳ�Ǹ�����
}

/*
���Ͷ��������� 1	�߼���1��Ϊ560us��+1680us�ߣ��߼���0��Ϊ560us��+560us�ߡ�
*/
void Send_NEC_1(uint8_t ir_choose)
{
	Send_IR(21,ir_choose);    	  //�����ز��ź�Ҳ��21������

	/*debug_sh: ʵ�ʲ���ʱ��λ��1���ĺ�������λ1.759  ������1.68ms�����Խ�64��С*/
	NoSend_IR(60,ir_choose);      //64//�������ز��ź�Ϊ1.68ms �������� = 1680us/26.3us = 63.87
}
 
//���ͺ���⣬ 26.3us���������8.77us���ͺ���⣬17.53us�����ͺ����   ������Ϊ38khz�� 1/38 = 26.3��(ռ�ձ�Ϊ1:2)
void Send_IR(unsigned int i,uint8_t ir_choose)
{
	while(i--)      //����i�����ڵ��ź� ��һ��������26.3us
	{
		IR_EN(1);
		#if 0
		switch(ir_choose)
		{
			case 1:
			{
				IR_EN(1);
				break;
			}
		}
		#endif
		Delay9us();
		IR_EN(0);
		#if 0
		switch(ir_choose)
		{
			case 1:
			{
				IR_EN(0);
				break;
			}
		}
		#endif
		Delay18us();
	}

}

//�����ͺ��� 26.3us��������ڶ������ͺ����
void NoSend_IR(unsigned int i,uint8_t ir_choose)
{
	while(i--)
	{
		IR_EN(0);
		#if 0
		switch(ir_choose)
		{
			case 1:
			{
				IR_EN(0);
				break;
			}
		}
		#endif
		Delay26us(); 
	}
}

//152  8.8
//315 17.8
//450 26.8
static void Delay9us(void)		//@11.0592MHz
{
	#if 1
	swm_delay_us(9);
	#else
	volatile unsigned int jjjj=152;
	while(jjjj--)
		__nop();
	#endif
}

//��ʱ18us��������������͵�ƽʱ�䣬��׼Ϊ17.53us
static void Delay18us(void)		//@11.0592MHz
{
	#if 1
	swm_delay_us(18);
	#else
	volatile unsigned int jjjj=315;
	while(jjjj--)
		__nop();
	#endif
}

//26.3us�����ڿ��������ڲ���������
static void Delay26us(void)		//@11.0592MHz
{
	#if 1
	swm_delay_us(26);
	#else
	volatile unsigned int jjjj=450;
	while(jjjj--)
		__nop();
	#endif
}

