#include "IR_RX.h"
//数据接收缓冲区
unsigned char ircode[10];
char my_ir_read(unsigned char * readBuff);

//标志位，用于判断是否成功接收数据
char ir_flag =0;

//等待10us
void Delay10us()		//@11.0592MHz
{
	volatile unsigned int jjjj=33;
	while(jjjj--)
		__nop();
	//i = 1;
	//while (--i);
}

void Delay600us()		//@11.0592MHz
{
	volatile unsigned int jjjj=1850;
	while(jjjj--)
		__nop();
}



//接收初始化
void IR_Init(GPIO_Module* GPIOx, uint16_t Pin)
{
	GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else
    {
        return;
    }

    /*Configure the GPIO pin as input floating*/
    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin          = Pin;
        GPIO_InitStructure.GPIO_Pull    = GPIO_PULL_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }

    /*Configure key EXTI Line to key input Pin*/
    GPIO_ConfigEXTILine(IR_PORT_SOURCE, IR_PIN_SOURCE);

    /*Configure key EXTI line*/
    EXTI_InitStructure.EXTI_Line    = IR_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /*Set key input interrupt priority*/
    NVIC_InitStructure.NVIC_IRQChannel                   = IR_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority           = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//中断服务函数
//处理接收的红外信息，进行解码
/*void EX0_ISR() 
{
	ir_flag = my_ir_read(ircode);   //对接收标志位进行判断,将接受到的数据保存到数组中
	if(!ir_flag)
	{
		return;  //返回值为零直接退出
	}
}*/

//接收红外数据解码
char my_ir_read(unsigned char * readBuff)
{
	unsigned int count ,i,j,temp,ZZZ = 0;
	//判断是不是低电平，即接收引导码为低电平
	ZZZ=IR_INPUT;
	if(!IR_INPUT){
		//检测低电平的有效性，是否为引导码 低电平的时间只有9ms
		count = 0;
		//等待低电平(引导码)结束,接收为低电平时一直等待
		while(!IR_INPUT){
			count++;   //为低电平是加加技术
			//实时检测是否为低电平
			Delay10us(); 
			ZZZ=IR_INPUT;
			if(ZZZ==1){
				ZZZ=1;
			}
			if(count>1000){   //9ms/10us = 900次 如果超过9ms还是低电平，低电平不合法(允许误差，稍微大一点，1000)
				return 0;    //不是有效引导码，退出
			}
		}
		//IR_INPUT = 1，引导码低电平结束，进入4.5ms的高电平
		count = 0; //清零
		//检测高电平的有效性
		while(IR_INPUT){
			count++;
			Delay10us();    //高电平超过4.5ms,超时判断		
			if(count>500){  //4500us/10us = 450 ,允许误差，设置为 500
				return 0;  
			}
		}
		//高电平结束，引导码结束，接收数据
		//接收四个字节的数据 用号码，用户码反码 ，命令码，命令码反码
		for(i = 0;i<8;i++){
			//接收每个字节的比特位Byte
			for(j=0;j<8;j++){
				count = 0;
				while(!IR_INPUT){ //等待第一个位的低电平结束0.56ms，也就是载波有红外时。
					count++;
					Delay10us();
					if(count>60){  //0.54ms/10us = 56 ,允许误差
						return 0; //超时退出，正常自动在while循环判断就退出了
					}
				}
				//判断是0还是1，高电平时间为560us,是0，如果高电平时间为1.685ms,是1
				//延时600us,超过0的时间，判断IR_INPUT是0还是1，如果是1，则表示前面状态还未结束，说明是1
				Delay600us();
				if(IR_INPUT){  //高电平，表示数据为比特位 1
					temp |= 1<<j; //当前位置1
					count = 0;
					while(IR_INPUT){  //等待高电平结束
						count++;
						Delay10us();
						if(count>100){
							return 0;
						}
					}
				} //不是 1,为0时开始已经赋值为0了
			}
			readBuff[i] = temp;  //保存数据
			temp = 0;
		}
		Delay600us();
		//通过反码判断数据是否正确 >>互为反码相加等于255
		
			if((readBuff[2]+readBuff[3]) == 255 ){
				if((readBuff[4]+readBuff[5]) == 255 ){
					if((readBuff[6]+readBuff[7]) == 255 ){
						return 1;  //数据正确，返回1
					}
				}
			}
	}
	return 0;
}	



