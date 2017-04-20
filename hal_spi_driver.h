#ifndef __HAL_SPI_DRIVER_H
#define __HAL_SPI_DRIVER_H


/* MCU specific headerfile */
#include "stm32f769xx.h"


/******************************************************************************/
/*                                                                           	*/
/*                        1. SPI Register Bit Defininitions			              */
/*                                                     												*/
/******************************************************************************/


/**************************** bit definition for SPI_CR1 register *************/
#define SPI_REG_CR1_BIDIMODE					((uint32_t) 1 << 15)	// Bidirectional data mode enable
#define SPI_ENABLE_2_LINE_UNI_DIR			0
#define SPI_ENABLE_1_LINE_BIDI				1

#define SPI_REG_CR1_SSM								((uint32_t) 1 << 9)		// Software slave management
#define SPI_SSM_ENABLE								1
#define SPI_SSM_DISABLE								0

#define SPI_REG_CR1_SSI								((uint32_t) 1 << 8)		// Internal slave select


#define SPI_CR1_LSBFRIST							((uint32_t) 1 << 7)		// Frame format
#define SPI_TX_MSB_FIRST							0
#define SPI_TX_LSB_FIRST							1


#define SPI_REG_CR1_SPE								((uint32) 1 << 6)			// SPI Enable


#define SPI_REG_CR1_BR_PCLK_DIV_2			((uint32_t) 0 << 3)		// Baud rate control
#define SPI_REG_CR1_BR_PCLK_DIV_4			((uint32_t) 1 << 3)
#define SPI_REG_CR1_BR_PCLK_DIV_8			((uint32_t) 2 << 3)
#define SPI_REG_CR1_BR_PCLK_DIV_16		((uint32_t) 3 << 3)
#define SPI_REG_CR1_BR_PCLK_DIV_32		((uint32_t) 4 << 3)
#define SPI_REG_CR1_BR_PCLK_DIV_64		((uint32_t) 5 << 3)
#define SPI_REG_CR1_BR_PCLK_DIV_128		((uint32_t) 6 << 3)
#define SPI_REG_CR1_BR_PCLK_DIV_256		((uint32_t) 7 << 3)


#define SPI_REG_CR1_MSTR							((uint32_t) 1 << 2)		// Master selection
#define SPI_MASTER_MODE_SEL						1
#define SPI_SLAVE_MODE_SEL						0


#define SPI_REG_CR1_CPOL							((uint32_t) 1 << 1)		// Clock polarity
#define SPI_CPOL_LOW									0
#define SPI_CPOL_HIGH									1


#define SPI_REG_CR1_CPHA							((uint32_t) 1 << 0)		// Clock phase
#define SPI_FIRST_CLOCK_TRANS					0
#define SPI_SECOND_CLOCK_TRANS				1


/**************************** bit definition for SPI_CR2 register *************/
#define SPI_DATA_SIZE_4								((uint32_t) 3 << 8)		// Data size
#define SPI_DATA_SIZE_5								((uint32_t) 4 << 8)
#define SPI_DATA_SIZE_6								((uint32_t) 5 << 8)
#define SPI_DATA_SIZE_7								((uint32_t) 6 << 8)
#define SPI_DATA_SIZE_8								((uint32_t) 7 << 8)		
#define SPI_DATA_SIZE_9								((uint32_t) 8 << 8)
#define SPI_DATA_SIZE_10							((uint32_t) 9 << 8)
#define SPI_DATA_SIZE_11							((uint32_t) 10 << 8)
#define SPI_DATA_SIZE_12							((uint32_t) 11 << 8)
#define SPI_DATA_SIZE_13							((uint32_t) 12 << 8)		
#define SPI_DATA_SIZE_14							((uint32_t) 13 << 8)
#define SPI_DATA_SIZE_15						  ((uint32_t) 14 << 8)
#define SPI_DATA_SIZE_16							((uint32_t) 15 << 8)


#define SPI_REG_CR2_TXEIE_ENABLE			((uint32_t) 1 << 7)		// TX buffer empty int. enable
#define SPI_REG_CR2_RXNEIE_ENABLE			((uint32_t) 1 << 6)		// RX buffer not empty int. enable
#define SPI_REG_CR2_ERRIE_ENABLE			((uint32_t) 1 << 5)		// Error interrupt enable


#define SPI_REG_CR2_FRAME_FORMAT			((uint32_t) 1 << 4)		// Frame format
#define SPI_MOTOROLA_MODE							0
#define SPI_TI_MODE										1


#define SPI_REG_CR2_SSOE							((uint32_t) 1 << 2)		// SS output enable



/**************************** bit definition for SPI_SR register *************/
#define SPI_REG_SR_FRE_FLAG						((uint32_t) 1 << 8)
#define SPI_REG_SR_BUSY_FLAG					((uint32_t) 1 << 7)
#define SPI_REG_SR_TXE_FLAG						((uint32_t) 1 << 1)
#define SPI_REG_SR_RXNE_FLAG					((uint32_t) 1 << 0)





/* SPI device base address */
#define SPI_1 SPI1
#define SPI_2 SPI2
#define SPI_3 SPI3
#define SPI_4 SPI4
#define SPI_5 SPI5
#define SPI_6 SPI6

#define SPI_IS_BUSY										1
#define	SPI_IS_NOT_BUSY								0


/* Macros to enable clock for different SPI devices */
#define _HAL_RCC_SPI1_CLK_ENABLE()		(RCC->APB2ENR |= 1 << 12)
#define _HAL_RCC_SPI2_CLK_ENABLE()		(RCC->APB1ENR |= 1 << 14)
#define _HAL_RCC_SPI3_CLK_ENABLE()		(RCC->APB1ENR |= 1 << 15)
#define _HAL_RCC_SPI4_CLK_ENABLE()		(RCC->APB2ENR |= 1 << 13)
#define _HAL_RCC_SPI5_CLK_ENABLE()		(RCC->APB2ENR |= 1 << 20)
#define _HAL_RCC_SPI6_CLK_ENABLE()		(RCC->APB2ENR |= 1 << 21)



#define RESET				0
#define	SET					!RESET



/******************************************************************************/
/*                                                                           	*/
/*                        2. Data structures used by SPI driver	              */
/*                                                     												*/
/******************************************************************************/


/**
* @brief	HAL SPI State structure definition
*/
typedef enum{
	HAL_SPI_STATE_RESET					= 0X00,					// SPI not yet initialized or disabled
	HAL_SPI_STATE_READY					= 0X01,					// SPI initialized and ready for use
	HAL_SPI_STATE_BUSY					= 0X02,					// SPI process is ongoing
	HAL_SPI_STATE_BUSY_TX				= 0X12,					// Data transmission process is ongoing
	HAL_SPI_STATE_BUSY_RX				= 0X22,					// Data reception process is ongoing
	HAL_SPI_STATE_BUSY_TX_RX		= 0X32,					//  Data transmission and reception process is ongoing
	HAL_SPI_STATE_ERROR					= 0X03					// SPI error state
}hal_spi_state_t;



/**
* @brief	SPI configuration structure definition
*/
typedef struct{
	uint32_t Mode;									// Specifies the SPI operating mode
	
	uint32_t Direction;							// Specifies the SPI directional mode state
	
	uint32_t DataSize;							// Specifies the SPI data size
	
	uint32_t CLKPolarity;						// Specifies the serial clock steady state
	
	uint32_t CLKPhase;							// Specifies the clock active adge for bit capture
	
	uint32_t NSS;										// Specifies whether the NSS signal is managed by hardware
																	// (NSS pin) or by software using the SSI bit
	
	uint32_t BaudRatePrescaler;			// Specifies the baud rate prescaler value which will be used 
																	// to configure the trnsmit and receive SCK clock
	
	uint32_t FirstBit;							// Specifies whether data transfers start from MSB or LSB bit.

}spi_init_t;


/**
* @brief	SPI handle structure definition
*/
typedef struct __spi_handle_t{
	SPI_TypeDef			*Instance;			// SPI registers base address
	
	spi_init_t			Init;						// SPI communication parameters
	
	uint8_t					*pTxBuffPtr;		// Pointer to SPI Tx transfer Buffer
	
	uint16_t				TxXferSize;			// SPI Tx transfer size
	
	uint16_t				TxXferCount;			// SPI Tx transfer counter
	
	uint8_t					*pRxBuffPtr;			// SPI Rx transfer size
	
	uint16_t				RxXferSize;			// SPI Rx transfer size
	
	uint16_t				RxXferCount;		// SPI Rx transfer counter
	
	hal_spi_state_t	State;					// SPI communication state
	
}spi_handle_t;



/******************************************************************************/
/*                                                                           	*/
/*                        3. Driver exposed APIs									            */
/*                                                     												*/
/******************************************************************************/

/**
* @brief	API used to do initilize the given SPI device
* @param	*SPIx : base adress of the SPI
* @retval	None
*/
void hal_spi_init(spi_handle_t *spi_handle);

/**
* @brief	API used to do master data transmission
* @param	*SPIx : base address of the SPI
* @param	*buffer : pointer to the tx buffer
* @param	len : len of tx data
* @retval	None
*/
void hal_spi_master_tx(spi_handle_t *spi_handle, uint8_t *buffer, uint32_t len);

/**
* @brief	API used to do slave data transmission
* @param	*SPIx : Base address of the SPI
* @param	*rcv_buffer : pointer to the tx buffer
* @param	len : len of tx data
* @retval	None
*/
void hal_spi_slave_tx(spi_handle_t *spi_handle, uint8_t *buffer, uint32_t len);

/**
* @brief	API used to do master data reception
* @param	*SPIx : Base address of the SPI
* @param	*buffer : pointer to the rx buffer
* @param	len : len of rx data
*/
void hal_spi_master_rx(spi_handle_t *spi_handle, uint8_t *rcv_buffer, uint32_t len);


/**
* @brief	API used to do slave data reception
* @param	*SPIx : Base addres of the SPI
* @param	*buffer : pointer to the rx buffer
* @param	len : len of rx data
*/
void hal_spi_slave_rx(spi_handle_t *spi_handle, uint8_t *rcv_buffer, uint32_t len);

/**
* @brief	This function handles SPI interrupt request.
* @param	*hspi : pointer to spi_handle_t structure that contains
*						the configuration information or SPI module.
* @reval	None
*/
void hal_spi_irq_handler(spi_handle_t *hspi);


#endif





