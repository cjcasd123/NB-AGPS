#include "stm8l15x.h"
#include "softuart.h"
#include "stm8l15x_it.h"
#include "stm8l15x_tim2.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_itc.h"
#include "stm8l15x_exti.h"

/* 
        ʹ�õ�IO�� --- PA2  ��ʱ��2
		����Ҫ�ĵ��ļ� --- "stm8l15x_it.h"     "stm8l15x_tim2/3.h"
		  ����Ǹ����жϺ�����д������ϸ�����ļ���
       ���ִ��� --- �����ʳ���Ŀǰ��1200�����嵽ʱ��Ҫ���ж�ʱ��ֵ�޸�

*/
unsigned char uart_TX3_Buffer[COM_TX1_Lenth] = { 0x00 };
const unsigned char MSK_TAB[9] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0 }; // �����ǽ���λ����һ��8λ�������λ����λȡ��
unsigned char Tx_bit = 0;
unsigned char Tx_data = 0;
COMx_Define COM3 = { 0x00 };


/* ��ģ�⴮�ڵĳ�ʼ�� */
void p_softuart_init(void)
{
	/* ʹ��ʱ�� */
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);                       // ������ʱ��2��ʱ��
			
	/* ģ��IO�ڵĳ�ʼ��  TX -- ��������� */
	//GPIO_Init(RXD_PORT, RXD_PIN, GPIO_Mode_In_PU_IT);
	GPIO_Init(TXD_PORT, TXD_PIN, GPIO_Mode_Out_PP_High_Fast);
	//EXTI_SetPinSensitivity(UART_EXTI, EXTI_Trigger_Falling);
	//ITC_SetSoftwarePriority(UART_IRQ, ITC_PriorityLevel_1);

	/* ��ʱ����ʼ�� */
	TIM2_DeInit();
	//TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, 1250);             // 100US // ���㹫ʽ  16M/128/10000
          TIM2_TimeBaseInit(TIM2_Prescaler_8, TIM2_CounterMode_Up, 210);
        //  TIM2_SetCounter(0xcf);    
	TIM2_ARRPreloadConfig(ENABLE);
	TIM2_ClearFlag(TIM2_FLAG_Update);                                             // �������жϱ�־λ                                     // ��������ж�
	ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_1);      // �������ȼ�
	TIM2_ITConfig(TIM2_IT_Update, ENABLE);
	
	/* ������ʱ���ж� */
	//TIM2_Cmd(DISABLE);                                                             // ʹ��
	//enableInterrupts();                                                           // ��ȫ���ж�
}



/* ģ�ⷢ�ͺ��� */
//   ������һ���ֽ�д�ɷ���һ�����飨�ַ�����
void UART3_ruan_DataBuf_Push(char dat)	                // д�뷢�ͻ��壬ָ��+1
{
	if (COM3.TX_read != COM3.TX_write + 1)
	{
		uart_TX3_Buffer[COM3.TX_write] = dat;	    // װ���ͻ���
		++COM3.TX_write;
		if (COM3.TX_write >= COM_TX1_Lenth)
			COM3.TX_write = 0;
	}

	if (COM3.B_TX_busy == FALSE)		            // ��־����
	{
		COM3.B_TX_busy = TRUE;		                // ��־æ
		Tx_bit = 0xff;                              // ѡ����巽ʽ
		//TIM2_Cmd(ENABLE);                                 // �����ж�
		TIM_SEND_ENABLE();			                // TODO: ���������ж�
	}
}

void p_swut_send_timing(void)
{
	if (COM3.B_TX_busy == TRUE)
	{
		/* ��ʱ���жϽ�����busy��־λ */
		switch (Tx_bit)
		{
		case 0:
			clr_Tx;               // ������ʼλ
			break;
		case DATA_LENGTH + 1:
		case DATA_LENGTH + 2:
		case DATA_LENGTH + 3:
		case DATA_LENGTH + 4:
			set_Tx;	             // ����ֹͣλ
			break;

		case 0xff:
			break;              // ��Ҫ����װ������
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
		/* �رն�ʱ�� */
		//TIM2_Cmd(DISABLE);
		TIM_SEND_DISABLE();
	}
}

/* ��ʱ��2 --- ����ģ�⴮�ڵķ��� */
//INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
	/* ʱ��100US */
	p_swut_send_timing();                // ��������
	TIM2_ClearFlag(TIM2_FLAG_Update);    // ���
}






