#include "stm8l15x.h"
#include "softuart.h"
#include "mystdio.h"
//#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
/*
    ����ģ�⴮�ڲ��Եĳ����汾
	�ڶ�����֧������������
	�����ν����޸ģ���������
*/

void delay_ms(unsigned int ms)
{
	unsigned int a;
	while (ms)
	{
		a = 1800;
		while (a--);
		ms--;
	}
	return;
}

/*
void CLK_Config(void)
{
	CLK_DeInit();
	CLK_HSEConfig(CLK_HSE_OFF);
	//����LSIʱ��Դ
	CLK_LSICmd(ENABLE);
	//ʹ��ʱ���л�
	CLK_SYSCLKSourceSwitchCmd(ENABLE);
	//ѡ���ڲ�����ʱ����Ϊʱ��Դ
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);
	//����ϵͳʱ�ӷ�Ƶ
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
	//�ȴ�ʱ���ȶ�
	while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_LSI);
}
*/

#define LED_GPIO_PORT  GPIOA
#define LED_GPIO_PINS  GPIO_Pin_3

void p_all_init()
{
	disableInterrupts();
//	CLK_Config();
	p_softuart_init();
	enableInterrupts();                                                           // ��ȫ���ж�
}

int main( void )
{
	p_all_init();
	int date = 89;
	GPIO_Init(LED_GPIO_PORT, LED_GPIO_PINS, GPIO_Mode_Out_PP_Low_Slow);
	while (1)
	{
		GPIO_ToggleBits(LED_GPIO_PORT, LED_GPIO_PINS);
		softuart_printf("shu ju shi; %d\r\n", date);
		delay_ms(500);
        softuart_string("shu ju shi\r\n");
        delay_ms(500);
	}
}
