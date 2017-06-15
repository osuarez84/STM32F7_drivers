#include "hal_spi_driver.h"


/******************************************************************************/
/*                                                                           	*/
/*                        			Helper functions											        */
/*                                                     												*/
/******************************************************************************/
	/* NOTE : helper functions are all static. Static means that are	
	encapsulated and only used inside this .c file. They are only
	available for the API functions. */

/**
* @brief	
* @param	
* @retval	
*/
static void hal_spi_enable(SPI_TypeDef *SPIx){

	// First check if peripheral is already enable
	if(!(SPIx->CR1 & SPI_REG_CR1_SPE))		// if not...
		SPIx->CR1 |= SPI_REG_CR1_SPE;				// enable it!


}


/**
* @brief	
* @param	
* @retval	
*/
static void hal_spi_disable(SPI_TypeDef *SPIx){

	SPIx->CR1 &= ~SPI_REG_CR1_SPE;


}

/**
* @brief	
* @param	
* @retval	
*/
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

/**
* @brief	
* @param	
* @retval	
*/
static void hal_spi_configure_device_mode(SPI_TypeDef *SPIx, uint32_t master){

	if(master){
		SPIx->CR1 |= SPI_REG_CR1_MSTR;
	}
	else{
		SPIx->CR1 &= ~SPI_REG_CR1_MSTR;
	}
		
}

/**
* @brief	
* @param	
* @retval	
*/
static void hal_spi_configure_datasize(SPI_TypeDef *SPIx, uint32_t datasize, uint32_t lsbfirst){

	switch(datasize){
		
		case (4) :
			SPIx->CR2 |= SPI_DATA_SIZE_4;		
			break;
		case (5) :
			SPIx->CR2 |= SPI_DATA_SIZE_5;
			break;
		case (6) :
			SPIx->CR2 |= SPI_DATA_SIZE_6;
			break;
		case (7) :
			SPIx->CR2 |= SPI_DATA_SIZE_7;
			break;
		case (8) :
			SPIx->CR2 |= SPI_DATA_SIZE_8;	
			break;
		case (9) :
			SPIx->CR2 |= SPI_DATA_SIZE_9;	
			break;
		case (10) :
			SPIx->CR2 |= SPI_DATA_SIZE_10;
			break;
		case (11) : 
			SPIx->CR2 |= SPI_DATA_SIZE_11;
			break;
		case (12) :
			SPIx->CR2 |= SPI_DATA_SIZE_12;
			break;
		case (13) :
			SPIx->CR2 |= SPI_DATA_SIZE_13;	
			break;
		case (14) :
			SPIx->CR2 |= SPI_DATA_SIZE_14;
			break;
		case (15) :
			SPIx->CR2 |= SPI_DATA_SIZE_15;
			break;
		case (16) :
			SPIx->CR2 |= SPI_DATA_SIZE_16;
			break;
		default :
			SPIx->CR2 |= SPI_DATA_SIZE_8;
	}
	
	if(lsbfirst){
		SPIx->CR1 |= SPI_CR1_LSBFRIST;
	}
	else{
		SPIx->CR1 &= ~SPI_CR1_LSBFRIST;
	}
	
}


/**
* @brief	
* @param	
* @retval	
*/
static void hal_spi_configure_nss_master(SPI_TypeDef *SPIx, uint32_t ssm_enable){

	if(ssm_enable){
		// When peripheral is in master mode, NSS pin is not used 
		// and must be held at HIGH
		SPIx->CR1 |= SPI_REG_CR1_SSM;
		SPIx->CR1 |= SPI_REG_CR1_SSI;
	}
	else{
		SPIx->CR1 &= ~SPI_REG_CR1_SSM;
	}

}

/**
* @brief	
* @param	
* @retval	
*/
static void hal_spi_configure_nss_slave(SPI_TypeDef *SPIx, uint32_t ssm_enable){

		if(ssm_enable){
			SPIx->CR1 |= SPI_REG_CR1_SSM;
		}
		else{
			SPIx->CR1 &= SPI_REG_CR1_SSM;
		}

}

/**
* @brief	
* @param	
* @retval	
*/
static void hal_spi_configure_direction(SPI_TypeDef *SPIx, uint32_t direction, uint32_t output){
	
	if(direction){
		SPIx->CR1 |= SPI_REG_CR1_BIDIMODE;
	}
	else{
		SPIx->CR1 &= ~SPI_REG_CR1_BIDIMODE;
	}
	
	if(output){
		SPIx->CR1 |= SPI_REG_CR1_OUTBIDIMODE;
	}
	else{
		SPIx->CR1 &= ~SPI_REG_CR1_OUTBIDIMODE;
	}

}

/**
* @brief	
* @param	
* @retval	
*/
static void hal_spi_configure_baudrate(SPI_TypeDef *SPIx, uint32_t pre_scalar_value){

	SPIx->CR1 |= pre_scalar_value;
	
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
* @brief	close Tx transfer
* @param	*hspi : pointer to spi_handle_t structure that contains
*						the configuration information or SPI module.
* @retval	None
*/
static void hal_spi_tx_close_interrupt(spi_handle_t *hspi){
	
	/* Disable TXE interrupt */
	hal_spi_disable_txe_interrupt(hspi->Instance);
	
	/* if master and if driver state is not HAL_SPI_STATE_BUSY_RX then make state
	HAL_SPI_STATE_READEY */	
	if(hspi->Init.Mode && (hspi->State != HAL_SPI_STATE_BUSY_RX))
		hspi->State = HAL_SPI_STATE_READY;
}

/**
* @brief	
* @param	
* @retval	
*/
uint8_t hal_spi_is_bus_busy(SPI_TypeDef *SPIx){

	if(SPIx->SR & SPI_REG_SR_BUSY_FLAG){
		return SPI_IS_BUSY;
	}
	else{
		return SPI_IS_NOT_BUSY;
	}
}


/**
* @brief	
* @param	*hspi : pointer to spi_handle_t structure that contains
*						the configuration information or SPI module.
* @retval	None
*/
static void hal_spi_rx_close_interrupt(spi_handle_t *hspi){

	while(hal_spi_is_bus_busy(hspi->Instance));
	
	/* Disable RXNE interrupt */
	hal_spi_disable_rxne_interrupt(hspi->Instance);
	hspi->State = HAL_SPI_STATE_READY;

}






/******************************************************************************/
/*                                                                           	*/
/*                         Driver exposed APIs										            */
/*                                                     												*/
/******************************************************************************/

/**
* @brief	API used to do initilize the given SPI device
* @param	*SPIx : base adress of the SPI
* @retval	None
*/
void hal_spi_init(spi_handle_t *spi_handle){
	
	/* Configure the phase and polarity */
	hal_spi_configure_phase_and_polarity(spi_handle->Instance, \
				spi_handle->Init.CLKPhase, spi_handle->Init.CLKPolarity);
	
	/* Configure the spi device mode */
	hal_spi_configure_device_mode(spi_handle->Instance, spi_handle->Init.Mode);
	
	/* Configure the spi data size */
	hal_spi_configure_datasize(spi_handle->Instance, spi_handle->Init.DataSize, spi_handle->Init.FirstBit);
	
	/* configure the slave select line */
	if(spi_handle->Init.Mode == SPI_MASTER_MODE_SEL)
		hal_spi_configure_nss_master(spi_handle->Instance, spi_handle->Init.NSS);
	else
		hal_spi_configure_nss_slave(spi_handle->Instance, spi_handle->Init.NSS);
	
	/* Configure the SPI device speed */
	hal_spi_configure_baudrate(spi_handle->Instance, spi_handle->Init.BaudRatePrescaler);
	
	/* Configure the SPI device direction */
	hal_spi_configure_direction(spi_handle->Instance, spi_handle->Init.Direction, spi_handle->Init.OutputBidiMode);


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
* @retval
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
* @retval
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
* @brief	handles TXE interrupt
* @param	*hspi : pointer to spi_handle_t structure that contains
*						the configuration information or SPI module.
* @retval	None
*/
void hal_spi_handle_tx_interrupt(spi_handle_t *hspi){

	/* Transmit data in 8 bit mode */
	if(hspi->Init.DataSize == SPI_DATASIZE_8){
		hspi->Instance->DR = (*hspi->pTxBuffPtr++);
		hspi->TxXferCount--; // We sent 1 byte
	}
	else if (hspi->Init.DataSize == SPI_DATASIZE_16){
		hspi->Instance->DR = *((uint16_t*)hspi->pTxBuffPtr);
		hspi->pTxBuffPtr += 2;
		hspi->TxXferCount -= 2;		// we sent two bytes in one go
	}

	if(hspi->TxXferCount == 0){
		/* we reached end of transmission, so close the txe interrupt */
		hal_spi_tx_close_interrupt(hspi);
	
	}
	
}


/**
* @brief	handles RXNE interrupt
* @param	*hspi : pointer to spi_handle_t structure that contains
*						the configuration information or SPI module.
* @retval	None
*/
static void hal_spi_handle_rx_interrupt(spi_handle_t *hspi){

	/* Receive data in 8 bit mode */
	if(hspi->Init.DataSize == SPI_DATASIZE_8){
		// NULL check
		if(hspi->pRxBuffPtr++)													// Si no está vacío...
			(*hspi->pRxBuffPtr) = hspi->Instance->DR;			// Leeme el buffer RX
		hspi->RxXferCount--;
	}
	else if(hspi->Init.DataSize == SPI_DATASIZE_16){
		*((uint16_t*)hspi->pRxBuffPtr) = hspi->Instance->DR;
		hspi->pRxBuffPtr += 2;
		hspi->RxXferCount -= 2;
	}

	if(hspi->RxXferCount == 0){
		/* we are done with the Rxing of data, lets close the rxne interrupt */
		hal_spi_rx_close_interrupt(hspi);
	}
}



/**
* @brief	This function handles SPI interrupt request.
* @param	*hspi : pointer to spi_handle_t structure that contains
*						the configuration information or SPI module.
* @retval	None
*/
void hal_spi_irq_handler(spi_handle_t *hspi){

	uint32_t tmp1 = 0, tmp2 = 0;
	
	/* RXNE */
	/* check to see RXNE is set in the status register */
	tmp1 = (hspi->Instance->SR & SPI_REG_SR_RXNE_FLAG);
	
	/* check whether RXNEIE bit is enabled in the control register */
	tmp2 = (hspi->Instance->CR2 & SPI_REG_CR2_RXNEIE_ENABLE);
	
	if ((tmp1 != RESET_S) && (tmp2 != RESET_S) ){
		
		/* RXNE floag is set handle the RX of data bytes */
		hal_spi_handle_rx_interrupt(hspi);
		return;
	}
	
	
	/* TXE */
	/* check to see TXE is set in the status register */
	tmp1 = (hspi->Instance->SR & SPI_REG_SR_TXE_FLAG);
	tmp2 = (hspi->Instance->CR2 & SPI_REG_CR2_TXEIE_ENABLE);
	
	if ( (tmp1 != RESET_S) && (tmp2 != RESET_S) ){
		/* TXE flag is set handle the TX of data bytes */
		hal_spi_handle_tx_interrupt(hspi);
		return;
	}

};

