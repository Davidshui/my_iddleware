/**
 * @file IR.c
 * @author your name (you@domain.com)
 * @brief 1.红外数据格式: 引导码 + 数据码 + 数据反码  + 结束码	
 * 			引导码：9ms载波 + 4.5ms空闲  
 * 		  2.本代码可以控制多个红外灯，但产品功能只需要控制一个红外灯。
 * @version 0.1
 * @date 2023-06-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"
#include "dev_infrared_radia.h"


//uint8_t IR_data_array[20] = {0};


//红外控制引脚   （硬件M20引脚，高电平红外灯亮）
void bsp_IR_init(void)
{
	GPIO_Init(IR_PORT, IR_PIN, 1, 0, 0);         /* IR端口*//*pin脚*//* 输出 *//* 上拉 *//* 无下拉 */
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


	//先发送引导码:先9ms的高电平脉冲，其后是4.5ms的低电平
	Send_IR(342,ir_choose);   //9ms发送载波信号 周期 = 9000us/26.3us = 342
	NoSend_IR(171,ir_choose); //4.5ms不发送载波信号，周期 = 4500us/26.3us = 171

	//发送数据
	GetByte_And_SendByte(p_data,data_len,ir_choose);   //取出每一位并发送
}


//取出数据中的每一位并发送
void GetByte_And_SendByte(uint8_t *p_data, uint8_t data_len,uint8_t ir_choose)
{
	unsigned int temp,i,j; //定义中间变量
	uint8_t data_tmp;
	//发送数据码（地址码和地址码的反码）
	for(j = 0; j<data_len; j++)
	{
		data_tmp = p_data[j];
		for(i = 0;i<8;i++){  //循环16位数据中的每一位
			temp = data_tmp&(0x01<<i); //通过与运算取出数据最低位
			if(temp){  //如果是1执行
				Send_NEC_1(ir_choose);
			}else{     //是0执行
				Send_NEC_0(ir_choose);
			}
			//temp = temp>>1;   //左移一位，取出下一位数据
		}
		
		data_tmp = ~(p_data[j]);
		for(i = 0;i<8;i++){  //循环8位命令中的每一位
			temp = data_tmp & (0x01<<i);
			if(temp){  //如果是1执行
				Send_NEC_1(ir_choose);
			}else{     //是0执行
				Send_NEC_0(ir_choose);
			}
			//temp = temp>>1;   //左移一位，取出下一位命令
		}
		//数据赋值给中间变量,取出最低位
		//发送命令码
	}
	
	//发送结束码
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
发送二进制数据 0   逻辑“1”为560us低+1680us，逻辑“0”为560us低+560us高。
*/
void Send_NEC_0(uint8_t ir_choose)
{
	Send_IR(21,ir_choose); 	   //发送载波信号 0.56ms, 也就是发送红外光21个周期
	NoSend_IR(21,ir_choose);   //不发送载波信号0.56ms,也是个周期
}

/*
发送二进制数据 1	逻辑“1”为560us低+1680us高，逻辑“0”为560us低+560us高。
*/
void Send_NEC_1(uint8_t ir_choose)
{
	Send_IR(21,ir_choose);    	  //发送载波信号也是21个周期

	/*debug_sh: 实际测试时，位“1”的后半个周期位1.759  ，超过1.68ms，所以将64改小*/
	NoSend_IR(60,ir_choose);      //64//不发送载波信号为1.68ms 发送周期 = 1680us/26.3us = 63.87
}
 
//发送红外光， 26.3us这个周期内8.77us发送红外光，17.53us不发送红外光   （周期为38khz， 1/38 = 26.3）(占空比为1:2)
void Send_IR(unsigned int i,uint8_t ir_choose)
{
	while(i--)      //产生i个周期的信号 ，一个周期是26.3us
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

//不发送红外 26.3us这个周期内都不发送红外光
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

//延时18us函数，控制输出低电平时间，标准为17.53us
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

//26.3us，用于控制周期内不发射红外光
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

