#include "hal_spi_driver.h"


/******************************************************************************/
/*                                                                           	*/
/*                        			Helper functions											        */
/*                                                     												*/
/******************************************************************************/

static void hal_spi_enable(SPI_TypeDef *SPIx){

	// First check if peripheral is already enable
	if(!(SPIx->CR1 & SPI_REG_CR1_SPE))		// if not...
		SPIx->CR1 |= SPI_REG_CR1_SPE;				// enable it!


}


static void hal_spi_disable(SPI_TypeDef *SPIx){

	SPIx->CR1 &= ~SPI_REG_CR1_SPE;


}

static void hal_spi_configure_phase_and_polarity(SPI_TypeDef *SPIx, uint32_t phase_value, uint32_t polarity){

	if(phase_value){
		SPIx->CR1 |= SPI_REG_CR1_CPHA;
	}
	else{
		SPIx->CR1 &= ~SPI_REG_CR1_CPHA;
	}

	if(polarity){
		SPIx->CR1 |= SPI_REG_CR1_CPOL;
	}
	else{
		SPIx->CR1 &= ~SPI_REG_CR1_CPOL;
	}
	

}


static void hal_spi_configure_device_mode(SPI_TypeDef *SPIx, uint32_t master){

	if(master){
		SPIx->CR1 |= SPI_REG_CR1_MSTR;
	}
	else{
		SPIx->CR1 &= ~SPI_REG_CR1_MSTR;
	}
		
}


static void hal_spi_configure_datasize_direction(SPI_TypeDef *SPIx, uint32_t datasize, uint32_t lsbfirst){

	switch(datasize){
		
		case (4) :
			SPIx->CR2 |= SPI_DATA_SIZE_4;		
		case (5) :
			SPIx->CR2 |= SPI_DATA_SIZE_5;
		case (6) :
			SPIx->CR2 |= SPI_DATA_SIZE_6;
		case (7) :
			SPIx->CR2 |= SPI_DATA_SIZE_7;
		case (8) :
			SPIx->CR2 |= SPI_DATA_SIZE_8;
		case (9) :
			SPIx->CR2 |= SPI_DATA_SIZE_9;
		case (10) :
			SPIx->CR2 |= SPI_DATA_SIZE_10;
		case (11) : 
			SPIx->CR2 |= SPI_DATA_SIZE_11;
		case (12) :
			SPIx->CR2 |= SPI_DATA_SIZE_12;
		case (13) :
			SPIx->CR2 |= SPI_DATA_SIZE_13;
		case (14) :
			SPIx->CR2 |= SPI_DATA_SIZE_14;
		case (15) :
			SPIx->CR2 |= SPI_DATA_SIZE_15;
		case (16) :
			SPIx->CR2 |= SPI_DATA_SIZE_16;
		default :
			SPIx->CR2 |= SPI_DATA_SIZE_8;
	}
	
	if(lsbfirst){
		SPIx->CR2 |= SPI_CR1_LSBFRIST;
	}
	else{
		SPIx->CR2 |= SPI_CR1_LSBFRIST;
	}
	
}

static void hal_spi_configure_nss_master(SPI_TypeDef *SPIx, uint32_t ssm_enable){

	if(ssm_enable){
		// When peripheral is in master mode, NSS pin is not used 
		// and must be held at HIGH
		SPIx->CR1 |= SPI_REG_CR1_SSM;
		SPIx->CR1 |= SPI_REG_CR1_SSI;
	}
	else{
		SPIx->CR1 |= SPI_REG_CR1_SSI;
	}

}


static void hal_spi_configure_nss_slave(SPI_TypeDef *SPIx, uint32_t ssm_enable){

		if(ssm_enable){
			SPIx->CR1 |= SPI_REG_CR1_SSM;
		}
		else{
			SPIx->CR1 &= SPI_REG_CR1_SSM;
		}



}







/**
* @brief	API used to do initilize the given SPI device
* @param	*SPIx : base adress of the SPI
* @retval	None
*/
void hal_spi_init(spi_handle_t *spi_handle){
	
	/* Configure the phase and polerity */
	
	/* Configure the spi device mode */
	
	/* Configure the spi data size */
	
	/* configure the slave select line */
	
	/* Configure the SPI device speed */
	
	/* Configure the SPI device direction */


}

/**
* @brief	API used to do master data transmission
* @param	*SPIx : base address of the SPI
* @param	*buffer : pointer to the tx buffer
* @param	len : len of tx data
* @retval	None
*/
void hal_spi_master_tx(spi_handle_t *spi_handle, uint8_t *buffer, uint32_t len){
		
	spi_handle->pTxBuffPtr = buffer;
	spi_handle->TxXferCount = len;
	spi_handle->TxXferSize = len;
	
	spi_handle->State = HAL_SPI_STATE_BUSY_TX;
	
	hal_spi_enable(spi_handle->Instance);
	
	hal_spi_enable_txe_interrupt(spi_handle->Instance);


}


/**
* @brief	
* @param	*SPIx : base address of the SPI
* @retval	None
*/
static void hal_spi_enable_txe_interrupt(SPI_TypeDef *SPIx){
	
	SPIx->CR2 |= SPI_REG_CR2_TXEIE_ENABLE;
	
}

/**
* @brief	
* @param	*SPIx : base address of the SPI
* @retval	None
*/
static void hal_spi_disable_txe_interrupt(SPI_TypeDef *SPIx){

	SPIx->CR2 &= ~SPI_REG_CR2_TXEIE_ENABLE; 

}




/**
* @brief	
* @param	*SPIx : base address of the SPI
* @retval	None
*/
static void hal_spi_enable_rxne_interrupt(SPI_TypeDef *SPIx){

	SPIx->CR2 |= SPI_REG_CR2_RXNEIE_ENABLE;
	
}

/**
* @brief	
* @param	*SPIx : base address of the SPI
* @retval	None
*/
static void hal_spi_disable_rxne_interrupt(SPI_TypeDef *SPIx){
	
	SPIx->CR2 &= ~SPI_REG_CR2_RXNEIE_ENABLE;
	
}




/**
* @brief	API used to do slave data transmission
* @param	*SPIx : Base address of the SPI
* @param	*rcv_buffer : pointer to the tx buffer
* @param	len : len of tx data
* @retval	None
*/
void hal_spi_slave_tx(spi_handle_t *spi_handle, uint8_t *buffer, uint32_t len){
	
	// WARNING: whenever you are in slave mode and want send data,
	// then slave must be ready with the data in th tx buffer of 
	// the SPI engine, before master produces the clock.
	
	/* populate the pointers and length info to TX the data */
	spi_handle->pTxBuffPtr = buffer;
	spi_handle->TxXferSize = len;
	spi_handle->TxXferCount = len;
	
	/* pointers to handle dummy rx, you can reuse the same pointer */
	spi_handle->pRxBuffPtr = buffer;
	spi_handle->RxXferSize = len;
	spi_handle->RxXferCount = len;
	
	/* Driver is busy in doing TX */
	spi_handle->State = HAL_SPI_STATE_BUSY_TX;
	
	hal_spi_enable(spi_handle->Instance);
	
	/* Now enable both TXE and RXNE Interrupt */
	hal_spi_enable_rxne_interrupt(spi_handle->Instance);
	hal_spi_enable_txe_interrupt(spi_handle->Instance);			// First enable the tx interrupt


}

/**
* @brief	API used to do master data reception
* @param	*SPIx : Base address of the SPI
* @param	*buffer : pointer to the rx buffer
* @param	len : len of rx data
*/
void hal_spi_master_rx(spi_handle_t *spi_handle, uint8_t *rx_buffer, uint32_t len){
	
	uint32_t i = 0, val;
	
	/* this is a dummy tx */
	// para la recepción debemos de generar la señal de reloj,
	// el SPI genera la señal cuando se escriben datos al txBuffer,
	// utilizamos entonces datos dummies para generar esa señal.
	spi_handle->pTxBuffPtr = rx_buffer;
	spi_handle->TxXferSize = len;
	spi_handle->TxXferCount = len;
	
	/* data will be rxed to rx_buffer */
	spi_handle->pRxBuffPtr = rx_buffer;
	spi_handle->RxXferSize = len;
	spi_handle->RxXferCount = len;
	
	/* Driver is busy in RX */
	spi_handle->State = HAL_SPI_STATE_BUSY_RX;
	
	hal_spi_enable(spi_handle->Instance);
	
	/* read data register once before enabling 
	the RXNE interrupt to make sure DR is
	empty */
	// Lo leemos para vaciarlo antes de habilitar
	// las interrupciones
	val = spi_handle->Instance->DR;
	
	/* Now enable both TXE and RXNE Interrupt */
	hal_spi_enable_rxne_interrupt(spi_handle->Instance);
	hal_spi_enable_txe_interrupt(spi_handle->Instance);



}


/**
* @brief	API used to do slave data reception
* @param	*SPIx : Base addres of the SPI
* @param	*buffer : pointer to the rx buffer
* @param	len : len of rx data
*/
void hal_spi_slave_rx(spi_handle_t *spi_handle, uint8_t *rx_buffer, uint32_t len){

	/* Populate the rx_buffer pointer addres along with size in the handle */
	spi_handle->pRxBuffPtr = rx_buffer;
	spi_handle->RxXferSize = len;
	spi_handle->RxXferCount = len;
	
	/* Driver is busy in RX */
	spi_handle->State = HAL_SPI_STATE_BUSY_RX;
	
	/* enable the peripheral, if its not enabled */
	hal_spi_enable(spi_handle->Instance);
	
	/* slave need to rx, so enable the RXNE interrupt */
	/* Byte reception will be taken care in the RXNE Interrupt handling code */
	hal_spi_enable_rxne_interrupt(spi_handle->Instance);
	
}

/**
* @brief	This function handles SPI interrupt request.
* @param	*hspi : pointer to spi_handle_t structure that contains
*						the configuration information or SPI module.
* @reval	None
*/
void hal_spi_irq_handler(spi_handle_t *hspi);
