#ifndef __SOFTUART_H__
#define __SOFTUART_H__
#include "stm8l15x.h"


typedef struct
{
	u8	TX_read;		//发送读指针
	u8	TX_write;		//发送写指针
	u8	B_TX_busy;		//忙标志

} COMx_Define;

#define	COM_TX1_Lenth			128
#define	DATA_LENGTH				8		
#define STOP_BITS				1
/* 模拟串口引脚 */
#define		TXD_PORT			GPIOA
#define		TXD_PIN			GPIO_Pin_2
//#define		RXD_PORT			GPIOA
//#define		RXD_PIN			GPIO_Pin_2

//#define UART_TxPORT	    (TXD_PORT)
//#define	UART_TxPIN	    (TXD_PIN)
//#define UART_RxPORT	    (RXD_PORT)
//#define	UART_RxPIN	    (RXD_PIN)

//#define UART_EXTI		(EXTI_Pin_2)
//#define UART_IRQ		(EXTI2_IRQn)

//#define set_Tx			(UART_TxPORT->ODR |= UART_TxPIN)
//#define clr_Tx			(UART_TxPORT->ODR &=~UART_TxPIN)

#define set_Tx  GPIOA->ODR |= GPIO_Pin_2;
#define clr_Tx  GPIOA->ODR &= (uint8_t)(~GPIO_Pin_2);
#define TIM_SEND_ENABLE()			do{	TIM2->CNTRH = 0; TIM2->CNTRL = 0 ;TIM2->CR1 |= (uint8_t)(TIM_CR1_CEN);  } while (0)
#define TIM_SEND_DISABLE()			do{	TIM2->CR1 &= (uint8_t)(~TIM_CR1_CEN);                               } while (0)


void UART3_ruan_DataBuf_Push(char dat);
//void p_swut_send_timing(void);
void p_softuart_init(void);

#endif  