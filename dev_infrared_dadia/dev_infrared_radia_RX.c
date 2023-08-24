#include "IR_RX.h"
//���ݽ��ջ�����
unsigned char ircode[10];
char my_ir_read(unsigned char * readBuff);

//��־λ�������ж��Ƿ�ɹ���������
char ir_flag =0;

//�ȴ�10us
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



//���ճ�ʼ��
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

//�жϷ�����
//������յĺ�����Ϣ�����н���
/*void EX0_ISR() 
{
	ir_flag = my_ir_read(ircode);   //�Խ��ձ�־λ�����ж�,�����ܵ������ݱ��浽������
	if(!ir_flag)
	{
		return;  //����ֵΪ��ֱ���˳�
	}
}*/

//���պ������ݽ���
char my_ir_read(unsigned char * readBuff)
{
	unsigned int count ,i,j,temp,ZZZ = 0;
	//�ж��ǲ��ǵ͵�ƽ��������������Ϊ�͵�ƽ
	ZZZ=IR_INPUT;
	if(!IR_INPUT){
		//���͵�ƽ����Ч�ԣ��Ƿ�Ϊ������ �͵�ƽ��ʱ��ֻ��9ms
		count = 0;
		//�ȴ��͵�ƽ(������)����,����Ϊ�͵�ƽʱһֱ�ȴ�
		while(!IR_INPUT){
			count++;   //Ϊ�͵�ƽ�ǼӼӼ���
			//ʵʱ����Ƿ�Ϊ�͵�ƽ
			Delay10us(); 
			ZZZ=IR_INPUT;
			if(ZZZ==1){
				ZZZ=1;
			}
			if(count>1000){   //9ms/10us = 900�� �������9ms���ǵ͵�ƽ���͵�ƽ���Ϸ�(��������΢��һ�㣬1000)
				return 0;    //������Ч�����룬�˳�
			}
		}
		//IR_INPUT = 1��������͵�ƽ����������4.5ms�ĸߵ�ƽ
		count = 0; //����
		//���ߵ�ƽ����Ч��
		while(IR_INPUT){
			count++;
			Delay10us();    //�ߵ�ƽ����4.5ms,��ʱ�ж�		
			if(count>500){  //4500us/10us = 450 ,����������Ϊ 500
				return 0;  
			}
		}
		//�ߵ�ƽ�������������������������
		//�����ĸ��ֽڵ����� �ú��룬�û��뷴�� �������룬�����뷴��
		for(i = 0;i<8;i++){
			//����ÿ���ֽڵı���λByte
			for(j=0;j<8;j++){
				count = 0;
				while(!IR_INPUT){ //�ȴ���һ��λ�ĵ͵�ƽ����0.56ms��Ҳ�����ز��к���ʱ��
					count++;
					Delay10us();
					if(count>60){  //0.54ms/10us = 56 ,�������
						return 0; //��ʱ�˳��������Զ���whileѭ���жϾ��˳���
					}
				}
				//�ж���0����1���ߵ�ƽʱ��Ϊ560us,��0������ߵ�ƽʱ��Ϊ1.685ms,��1
				//��ʱ600us,����0��ʱ�䣬�ж�IR_INPUT��0����1�������1�����ʾǰ��״̬��δ������˵����1
				Delay600us();
				if(IR_INPUT){  //�ߵ�ƽ����ʾ����Ϊ����λ 1
					temp |= 1<<j; //��ǰλ��1
					count = 0;
					while(IR_INPUT){  //�ȴ��ߵ�ƽ����
						count++;
						Delay10us();
						if(count>100){
							return 0;
						}
					}
				} //���� 1,Ϊ0ʱ��ʼ�Ѿ���ֵΪ0��
			}
			readBuff[i] = temp;  //��������
			temp = 0;
		}
		Delay600us();
		//ͨ�������ж������Ƿ���ȷ >>��Ϊ������ӵ���255
		
			if((readBuff[2]+readBuff[3]) == 255 ){
				if((readBuff[4]+readBuff[5]) == 255 ){
					if((readBuff[6]+readBuff[7]) == 255 ){
						return 1;  //������ȷ������1
					}
				}
			}
	}
	return 0;
}	



