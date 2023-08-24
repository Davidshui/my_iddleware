#ifndef __BSP_INFRARED_RADIA_H__
#define	__BSP_INFRARED_RADIA_H__
#include "SWM320.h"

#define IR_PORT GPIOM
#define IR_PIN PIN20



#define IR_EN(x)                   do{ x ?                                             \
                                                    GPIO_SetBit(IR_PORT, IR_PIN) :     \
                                                    GPIO_ClrBit(IR_PORT, IR_PIN);      \
                                                }while(0)

void bsp_IR_init(void);
void Send_ENC_Message(uint8_t *p_data,uint8_t data_len,uint8_t ir_choose);

void GetByte_And_SendByte(uint8_t *p_data, uint8_t data_len,uint8_t ir_choose);
void Send_NEC_0(uint8_t ir_choose);
void Send_NEC_1(uint8_t ir_choose);
void Send_IR(unsigned int i,uint8_t ir_choose);
void NoSend_IR(unsigned int i,uint8_t ir_choose);
void Send_stop(uint8_t ir_choose);

static void Delay9us(void);
static void Delay18us(void);		
static void Delay26us(void);	


#if 0
//红外控制引脚   （硬件M20引脚，高电平红外灯亮） 
#define IR_EN1	   PBout(9) 
#define IR_EN2	   PBout(9) 
#define IR_EN3	   PBout(9) 
#define IR_EN4	   PBout(9) 
#define IR_EN(x)	IR_EN##x
#endif

#endif /*__BSP_INFRARED_RADIA_H__*/
