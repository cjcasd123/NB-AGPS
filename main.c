#include "stm8l15x.h"
#include "softuart.h"
#include "mystdio.h"
//#include "stm8l15x_clk.h"
#include "stm8l15x_gpio.h"
/*
    建立模拟串口测试的初级版本
	第二个分支建立！！！！
	第三次进行修改！！！！！
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
	//启动LSI时钟源
	CLK_LSICmd(ENABLE);
	//使能时钟切换
	CLK_SYSCLKSourceSwitchCmd(ENABLE);
	//选择内部低速时钟作为时钟源
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);
	//设置系统时钟分频
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
	//等待时钟稳定
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
	enableInterrupts();                                                           // 开全局中断
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
