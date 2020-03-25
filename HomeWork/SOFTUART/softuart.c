#include "stm8l15x.h"
#include "softuart.h"
#include "stm8l15x_it.h"
#include "stm8l15x_tim2.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_itc.h"
#include "stm8l15x_exti.h"

/* 
        使用的IO口 --- PA2  定时器2
		所需要的点文件 --- "stm8l15x_it.h"     "stm8l15x_tim2/3.h"
		  这个是各种中断函数的写法（详细见该文件）
       出现错误 --- 波特率出错（目前是1200）具体到时候要进行定时器值修改

*/
unsigned char uart_TX3_Buffer[COM_TX1_Lenth] = { 0x00 };
const unsigned char MSK_TAB[9] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0 }; // 这里是进行位操作一共8位（从最低位进行位取）
unsigned char Tx_bit = 0;
unsigned char Tx_data = 0;
COMx_Define COM3 = { 0x00 };


/* 对模拟串口的初始化 */
void p_softuart_init(void)
{
	/* 使能时钟 */
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);                       // 开启定时器2的时钟
			
	/* 模拟IO口的初始化  TX -- 推挽输出高 */
	//GPIO_Init(RXD_PORT, RXD_PIN, GPIO_Mode_In_PU_IT);
	GPIO_Init(TXD_PORT, TXD_PIN, GPIO_Mode_Out_PP_High_Fast);
	//EXTI_SetPinSensitivity(UART_EXTI, EXTI_Trigger_Falling);
	//ITC_SetSoftwarePriority(UART_IRQ, ITC_PriorityLevel_1);

	/* 定时器初始化 */
	TIM2_DeInit();
	//TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, 1250);             // 100US // 计算公式  16M/128/10000
          TIM2_TimeBaseInit(TIM2_Prescaler_8, TIM2_CounterMode_Up, 210);
        //  TIM2_SetCounter(0xcf);    
	TIM2_ARRPreloadConfig(ENABLE);
	TIM2_ClearFlag(TIM2_FLAG_Update);                                             // 清除溢出中断标志位                                     // 向上溢出中断
	ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_1);      // 设置优先级
	TIM2_ITConfig(TIM2_IT_Update, ENABLE);
	
	/* 开启定时器中断 */
	//TIM2_Cmd(DISABLE);                                                             // 使能
	//enableInterrupts();                                                           // 开全局中断
}



/* 模拟发送函数 */
//   将发送一个字节写成发送一个数组（字符串）
void UART3_ruan_DataBuf_Push(char dat)	                // 写入发送缓冲，指针+1
{
	if (COM3.TX_read != COM3.TX_write + 1)
	{
		uart_TX3_Buffer[COM3.TX_write] = dat;	    // 装发送缓冲
		++COM3.TX_write;
		if (COM3.TX_write >= COM_TX1_Lenth)
			COM3.TX_write = 0;
	}

	if (COM3.B_TX_busy == FALSE)		            // 标志空闲
	{
		COM3.B_TX_busy = TRUE;		                // 标志忙
		Tx_bit = 0xff;                              // 选择具体方式
		//TIM2_Cmd(ENABLE);                                 // 开启中断
		TIM_SEND_ENABLE();			                // TODO: 触发发送中断
	}
}

void p_swut_send_timing(void)
{
	if (COM3.B_TX_busy == TRUE)
	{
		/* 定时器中断进入检测busy标志位 */
		switch (Tx_bit)
		{
		case 0:
			clr_Tx;               // 发送起始位
			break;
		case DATA_LENGTH + 1:
		case DATA_LENGTH + 2:
		case DATA_LENGTH + 3:
		case DATA_LENGTH + 4:
			set_Tx;	             // 发送停止位
			break;

		case 0xff:
			break;              // 需要重新装载数据
		default:
		{
			if (Tx_data & MSK_TAB[Tx_bit - 1])
			{
				set_Tx;
			}
			else
			{
				clr_Tx;
			}
		}
		break;
		}
		if (Tx_bit > DATA_LENGTH + STOP_BITS)
		{
			if (COM3.TX_read != COM3.TX_write)
			{
				Tx_data = uart_TX3_Buffer[COM3.TX_read];
				if (++COM3.TX_read >= COM_TX1_Lenth)
				{
					COM3.TX_read = 0;
				}
			}
			else
			{
				COM3.B_TX_busy = FALSE;

			}

			Tx_bit = 0;
		}
		else
		{
			++Tx_bit;
		}

	}

	else
	{
		/* 关闭定时器 */
		//TIM2_Cmd(DISABLE);
		TIM_SEND_DISABLE();
	}
}

/* 定时器2 --- 用于模拟串口的发送 */
//INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
	/* 时间100US */
	p_swut_send_timing();                // 发送数据
	TIM2_ClearFlag(TIM2_FLAG_Update);    // 清空
}






