#ifndef __HAL_UART_DRIVER_H
#define __HAL_UART_DRIVER_H


#include "stm32f769xx.h"


typedef enum{
	
	HAL_UART_STATE_RESET				= 0x00,				/* Peripheral is not ye initialized */
	HAL_UART_STATE_READY				= 0x01,				/* Peripheral initialized and ready for use */
	HAL_UART_STATE_BUSY					= 0x02,				/* an internal process is ongoing */
	HAL_UART_STATE_BUSY_TX			= 0x12,				/* Data Transmission process is ongoing */
	HAL_UART_STATE_BUSY_RX			= 0x22,				/* Data Reception process is ongoing */
	HAL_UART_STATE_BUSY_TX_RX		= 0x32,				/* Data Transmission and Reception process is ongoing */
}hal_uart_state_t;


// TODO :  revisar estos códigos de error, no se si están sacados del manual o el datasheet
/* UART possible error codes */
#define	HAL_UART_ERROR_NONE						((uint32_t)0x00000000)		/* No error */
#define HAL_UART_ERROR_PE							((uint32_t)0x00000001)		/* Parity error */
#define HAL_UART_ERROR_NE							((uint32_t)0x00000002)		/* Noise error */
#define HAL_UART_ERROR_FE							((uint32_t)0x00000004)		/* Frame error */
#define	HAL_UART_ERROR_ORE						((uint32_t)0x00000008)		/* Overrun error */
#define HAL_UART_ERROR_DMA						((uint32_t)0x00000010)		/* DMA transfer error */


/* Different USART and UART peripheral base addresses */
#define USART_1	USART1
#define USART_2 USART2
#define USART_3 USART3
#define USART_4 UART4
#define	USART_5 UART5
#define USART_6 USART6
#define USART_7 UART7
#define USART_8 UART8


/* Macros to enable the clocks for various UART */
#define _HAL_RCC_USART1_CLK_ENABLE()						(RCC->APB2ENR |= (1 << 4))
#define _HAL_RCC_USART2_CLK_ENABLE()						(RCC->APB1ENR |= (1 << 17))		
#define _HAL_RCC_USART3_CLK_ENABLE()						(RCC->APB1ENR |= (1 << 18))
#define _HAL_RCC_USART4_CLK_ENABLE()						(RCC->APB1ENR |= (1 << 19))
#define _HAL_RCC_USART5_CLK_ENABLE()						(RCC->APB1ENR |= (1 << 20))
#define _HAL_RCC_USART6_CLK_ENABLE()						(RCC->APB2ENR |= (1 << 5))
#define _HAL_RCC_USART7_CLK_ENABLE()						(RCC->APB1ENR |= (1 << 30))
#define _HAL_RCC_USART8_CLK_ENABLE()						(RCC->APB1ENR |= (1 << 31))
	


/******************************************************************************/
/*                                                                            */
/*                      						UART			                                */
/*                          Register bit definitions                          */
/******************************************************************************/

/*********************** Bit definition for USART_SR register *****************/
#define USART_REG_SR_TXE_FLAG									((uint32_t)(1 << 7))
#define USART_REG_SR_TC_FLAG									((uint32_t)(1 << 6))
#define USART_REG_SR_RXNE_FLAG								((uint32_t)(1 << 5))
#define USART_REG_SR_IDLE_FLAG								((uint32_t)(1 << 4))
#define USART_REG_SR_ORE_FLAG									((uint32_t)(1 << 3))
#define USART_REG_SR_NE_FLAG									((uint32_t)(1 << 2))
#define USART_REG_SR_FE_FLAG									((uint32_t)(1 << 1))
#define USART_REG_SR_PE_FLAG									((uint32_t)(1 << 0))


/*********************** Bit definition for USART_BRR register ****************/



/*********************** Bit definition for USART_CR1 register ****************/
#define USART_REG_CR1_OVER8										((uint32_t)(1 << 15))
#define USART_OVER8_ENABLE										1
#define USART_OVER16_ENABLE										0

#define USART_REG_CR1_USART_EN								((uint32_t)(1 << 0))

/* M[1:0] word length */
#define USART_REG_CR1_M1											((uint32_t)(1 << 28))
#define USART_REG_CR1_M0											((uint32_t)(1 << 12))

#define USART_WL_7														0x00
#define USART_WL_8														0x01		
#define USART_WL_9														0x10


#define USART_REG_CR1_TXE_INT_EN							((uint32_t)(1 << 7))								
#define	USART_REG_CR1_TCIE_INT_EN							((uint32_t)(1 << 6))
#define USART_REG_CR1_RXNE_INT_EN							((uint32_t)(1 << 5))
#define USART_REG_CR1_PEIE_INT_EN							((uint32_t)(1 << 8))
#define USART_REG_CR1_TE											((uint32_t)(1 << 3))
#define USART_REG_CR1_RE											((uint32_t)(1 << 2))

/* Parity control */
#define USART_REG_CR1_PCE											((uint32_t)(1 << 10))			/* Parity control enable */
#define USART_REG_CR1_PS											((uint32_t)(1 << 9))			/* Parity selection */
#define USART_PARITY_NONE											(uint32_t)0x00
#define USART_PARITY_EVEN											(uint32_t)0x00


#define USART_MODE_TX_RX												((uint32_t)(USART_REG_CR1_TE | USART_REG_CR1_RE))
#define USART_MODE_TX													((uint32_t)(USART_REG_CR1_TE))

/*********************** Bit definition for USART_CR2 register ****************/
#define USART_REG_CR2_STOP_BITS								12
#define USART_STOP_BITS_1											(uint32_t)0x00
#define USART_STOP_BITS_HALF									(uint32_t)0x01
#define USART_STOP_BITS_2											(uint32_t)0x02
#define USART_STOP_BITS_1NHALF								(uint32_t)0x03




/*********************** Bit definition for USART_CR3 register ****************/

#define USART_REG_CR3_ERR_INT_EN							((uint32_t)(1 << 0))






#define UART_HWCONTROL_NONE										((uint32_t)0x00)

#define USART_BAUD_9600												(uint32_t)9600
#define USART_BAUD_115200											(uint32_t)115200
#define USART_BAUD_2000000										(uint32_t)2000000



/******************************************************************************/
/*                                                                            */
/*                      Data Structures used by UART Driver                   */
/*                                                                            */
/******************************************************************************/

/**
* @brief	UART Init structure definition
*/
typedef struct{
	
	uint32_t BaudRate;								/* This member configures the UART communication baud rate */
	
	uint8_t WordLength;							/* Specifies number of data bits transmitted or received in a frame */
	
	uint32_t StopBits;								/* Specifies number of stop bits transmitted */
	
	uint32_t Parity;									/* Specifies the parity mode */
	
	uint32_t Mode;										/* Specifies whether the receive or transmit mode is enable or disabled */

	uint32_t OverSampling;						/* Specifies whether the over sampling 8 is enabled or disabled */

}uart_init_t;

/* Application callbacks typedef */
typedef void (TX_COMP_CB_t) (void *ptr);
typedef void (RX_COMP_CB_t) (void *ptr);


/**
* @brief	UART handle structure definition
*/
typedef struct{

	USART_TypeDef								*Instance;				/* UART registers base address */
	
	uart_init_t									Init;							/* UART communication parameters */
	
	uint8_t											*pTxBuffPtr;			/* Pointer to UART Tx transfer buffer */
	
	uint16_t										TxXferSize;				/* UART Tx Transfer size */
	
	uint16_t										TxXferCount;			/* UART Rx Transfer counter */
	
	uint8_t											*pRxBuffPtr;			/* Pointer to UART Rx transfer buffer */
	
	uint16_t										RxXferSize;				/* UART Rx Transfer size */
	
	uint16_t										RxXferCount;			/* UART Rx Transfer counter */
	
	hal_uart_state_t						rx_state;					/* UART communication state */
	
	hal_uart_state_t						tx_state;					/* UART communication state*/
	
	uint32_t										ErrorCode;				/* UART Error code */
	
	TX_COMP_CB_t								*tx_cmp_cb;				/* Application callback when tx completed */
	
	RX_COMP_CB_t								*rx_cmp_cb;				/* Application callback when rx completed */

}uart_handle_t;


/******************************************************************************/
/*                                                                            */
/*                      Driver exposed APIs                                   */
/*                                                                            */
/******************************************************************************/

/**
	* @brief  API to do UART Peripheral initialization   
	* @param  *handle : pointer to the handle structure of the UART peripheral  
	* @retval None
	*/
void hal_uart_init(uart_handle_t *handle);


/**
	* @brief  API to do UART data Transmission
	* @param  *uart_handle : pointer to the handle structure of the UART Peripheral 
  * @param  *buffer : holds the pointer to the TX buffer 
  * @param  len : len of the data to be TXed
	* @retval None
	*/
void hal_uart_tx(uart_handle_t *handle, uint8_t *buffer, uint32_t len);



/**
	* @brief  API to do UART data Reception  
	* @param  *handle : pointer to the handle structure of the UART peripheral  
  * @param  *buffer : holds the pointer to the RX buffer 
  * @param  len : len of the data to be RXed
	* @retval None
	*/
void hal_uart_rx(uart_handle_t *handle, uint8_t *buffer, uint32_t len);



/**
  * @brief  This API handles UART interrupt request.
  * @param  huart: pointer to a uart_handle_t structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void hal_uart_handle_interrupt(uart_handle_t *handle);









#endif
