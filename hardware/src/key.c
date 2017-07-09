/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	key.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		����IO��ʼ�������������ж�
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//����ͷ�ļ�
#include "key.h"

//Ӳ������
#include "delay.h"




/*
************************************************************
*	�������ƣ�	Key_Init
*
*	�������ܣ�	����IO��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		SW2-PD2		SW3-PC11	SW4-PC12	SW5-PC13	
*				����Ϊ�͵�ƽ		�ͷ�Ϊ�ߵ�ƽ
************************************************************
*/
void Key_Init(void)
{

	GPIO_InitTypeDef gpioInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);	//��GPIOC��GPIOD��ʱ��
	
	gpioInitStructure.GPIO_Mode = GPIO_Mode_IPU;									//����Ϊ��������ģʽ
	gpioInitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12;			//��ʼ��Pin11��12��13
	gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;								//���ص����Ƶ��
	GPIO_Init(GPIOB, &gpioInitStructure);											//��ʼ��GPIOC
	
	gpioInitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &gpioInitStructure);											//��ʼ��GPIOD

}

/*
************************************************************
*	�������ƣ�	KeyScan
*
*	�������ܣ�	������ƽɨ��
*
*	��ڲ�����	GPIOX����Ҫɨ���GPIO��	NUM����GPIO���ڵı��
*
*	���ز�����	IO��ƽ״̬
*
*	˵����		
************************************************************
*/
_Bool KeyScan(GPIO_TypeDef* GPIOX, unsigned int NUM)
{
	
	if(GPIOX == GPIOB)
	{
		if(!GPIO_ReadInputDataBit(GPIOB, NUM))	//����  Ϊ��
		{
			return KEYDOWN;
		}
		else									//����  Ϊ��
		{
			return KEYUP;
		}
	}
	else if(GPIOX == GPIOA)
	{
		if(!GPIO_ReadInputDataBit(GPIOA, NUM))	//����  Ϊ��
		{
			return KEYDOWN;
		}
		else									//����  Ϊ��
		{
			return KEYUP;
		}
	}
	
	return KEYUP;								//Ĭ�Ϸ��ذ����ͷ�
	
}

/*
************************************************************
*	�������ƣ�	Keyboard
*
*	�������ܣ�	�������ܼ��
*
*	��ڲ�����	
*
*	���ز�����	��������
*
*	˵����		�ֵ�����˫��������
************************************************************
*/
unsigned char Keyboard(void)
{
	
	static unsigned char keyStatus = 0;
	
	if(KeyScan(GPIOB, KEY0) == KEYDOWN)
	{
		keyStatus |= 0x01;							//���KEY0����
		
		while(KeyScan(GPIOB, KEY0) != KEYUP)		//�ȴ��ͷ�
			return KEYNONE;
	}
	else
	{
		if(keyStatus & 0x01)
		{
			keyStatus &= 0xfe;						//���KEY0�ͷ�
			return KEY0DOWN;
		}
	}
	
	if(KeyScan(GPIOA, KEY1) == KEYDOWN)
	{
		keyStatus |= 0x02;							//���KEY1����
		
		while(KeyScan(GPIOA, KEY1) != KEYUP)		//�ȴ��ͷ�
			return KEYNONE;
	}
	else
	{
		if(keyStatus & 0x02)
		{
			keyStatus &= 0xfd;						//���KEY1�ͷ�
			return KEY1DOWN;
		}
	}
	
	if(KeyScan(GPIOB, KEY2) == KEYDOWN)
	{
		keyStatus |= 0x04;							//���KEY2����
		
		while(KeyScan(GPIOB, KEY2) != KEYUP)		//�ȴ��ͷ�
			return KEYNONE;
	}
	else
	{
		if(keyStatus & 0x04)
		{
			keyStatus &= 0xfb;						//���KEY2�ͷ�
			return KEY2DOWN;
		}
	}
	
	if(KeyScan(GPIOB, KEY3) == KEYDOWN)
	{
		keyStatus |= 0x08;							//���KEY3����
		
		while(KeyScan(GPIOB, KEY3) != KEYUP)		//�ȴ��ͷ�
			return KEYNONE;
	}
	else
	{
		if(keyStatus & 0x08)
		{
			keyStatus &= 0xf7;						//���KEY3�ͷ�
			return KEY3DOWN;
		}
	}
	
	return KEYNONE;
	
}
