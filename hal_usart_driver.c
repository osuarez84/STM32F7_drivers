#include "hal_usart_driver.h"
#include "hal_gpio_driver.h"
#include "led.h"
#include "stm32f7xx.h"


/******************************************************************************/
/*                                                                            */
/*                      Helper functions                                      */
/*                                                                            */
/******************************************************************************/


/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 	wl : word length
* @retval None
*/
static void hal_uart_configure_word_length(USART_TypeDef *uartx, uint8_t wl){

	// clear M0 and M1
	uartx->CR1 &= ~USART_REG_CR1_M0;
	uartx->CR1 &= ~USART_REG_CR1_M1;
	
	switch(wl){
		
		case (USART_WL_7) :
			uartx->CR1 &= ~USART_REG_CR1_M0;
			uartx->CR1 |= USART_REG_CR1_M1;
			break;
		case (USART_WL_8) :
			uartx->CR1 &= ~USART_REG_CR1_M0;
			uartx->CR1 &= ~USART_REG_CR1_M1;
			break;
		case (USART_WL_9) :
			uartx->CR1 |= USART_REG_CR1_M0;
			uartx->CR1 &= ~USART_REG_CR1_M1;
			break;
		default :															// default : 8 bits
			uartx->CR1 &= ~USART_REG_CR1_M0;
			uartx->CR1 &= ~USART_REG_CR1_M1;
	}
}


/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 	
* @retval None
*/
static void hal_uart_configure_stop_bits(USART_TypeDef *uartx, uint32_t nstop){

	// clear 12th and 13th bits
	uartx->CR2 &= (0x03 << USART_REG_CR2_STOP_BITS);
	
	switch(nstop){
	
		case (USART_STOP_BITS_HALF) :
			uartx->CR2 |= (0x01 << USART_REG_CR2_STOP_BITS);
			break;
		case (USART_STOP_BITS_2) :
			uartx->CR2 |= (0x02 << USART_REG_CR2_STOP_BITS);
			break;
		case (USART_STOP_BITS_1NHALF) :
			uartx->CR2 |= (0x03 << USART_REG_CR2_STOP_BITS);
			break;
		case (USART_STOP_BITS_1) :
			uartx->CR2 |= (0x00 << USART_REG_CR2_STOP_BITS);
			break;
		default : 							// 1 stop bit
			uartx->CR2 |= (0x00 << USART_REG_CR2_STOP_BITS);
	}

}

/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 	
* @retval None
*/
static void hal_uart_configure_over_sampling(USART_TypeDef *uartx, uint32_t over8){

	if(over8){
		uartx->CR1 |= USART_REG_CR1_OVER8;
	}

}

/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 	
* @retval None
*/
static void hal_uart_set_baud_rate(USART_TypeDef *uartx, uint32_t baud){
	
	// OJO, estos valores están calculados para un clock de 25 MHz (HSE) y
	// OVER8 = 1.
	// TODO : si se utiliza otro reloj hay que ajustar estos valores de nuevo
	// VER README DEL PROYECTO.
	uint32_t val;
	
	if(baud == USART_BAUD_9600)
		val = 0xA2C;
	else if (baud == USART_BAUD_115200)
		val = 0xD9;
	else								// default : 9600 baudios
		val = 0xA2C;
	
	uartx->BRR = val;
}


/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 	
* @retval None
*/
static void hal_uart_enable_disable_tx(USART_TypeDef *uartx, uint32_t te){

	if(te & USART_REG_CR1_TE)
		uartx->CR1 |= USART_REG_CR1_TE;
	else
		uartx->CR1 &= ~USART_REG_CR1_TE;

}

static void hal_uart_enable_disable_rx(USART_TypeDef *uartx, uint32_t re){
	
	if(re & USART_REG_CR1_RE)
		uartx->CR1 |= USART_REG_CR1_RE;
	else
		uartx->CR1 &= ~USART_REG_CR1_RE;

}


/**
	* @brief  Enable the given USART peripheral 
	* @param  *uartx : base address of the USART or UART peripheral
	* @retval None
	*/
void static hal_uart_enable(USART_TypeDef *uartx){
	
	uartx->CR1 |= USART_REG_CR1_USART_EN;

}


/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 	txe_en :
* @retval None
*/
static void hal_uart_configure_txe_interrupt(USART_TypeDef *uartx, uint32_t txe_en){

	if(txe_en)
		uartx->CR1 |= USART_REG_CR1_TXE_INT_EN;
	else
		uartx->CR1 &= ~USART_REG_CR1_TXE_INT_EN;
	
}

/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 	pe_en :
* @retval None
*/
static void hal_uart_configure_parity_error_interrupt(USART_TypeDef *uartx, uint32_t pe_en){
	
	if(pe_en)
		uartx->CR1 |= USART_REG_CR1_PEIE_INT_EN;
	else
		uartx->CR1 &= ~USART_REG_CR1_PEIE_INT_EN;
}

/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 	er_en :
* @retval None
*/
static void hal_uart_configure_error_interrupt(USART_TypeDef *uartx, uint32_t er_en){

	if(er_en)
		uartx->CR3 |= USART_REG_CR3_ERR_INT_EN;
	else
		uartx->CR3 &= ~USART_REG_CR3_ERR_INT_EN;
	
}


/**
* @brief  Enable the given USART peripheral 
* @param  *uartx : base address of the USART or UART peripheral
* @param 
* @retval None
*/
static void hal_uart_configure_rxne_interrupt(USART_TypeDef *uartx, uint32_t rxne_en){

	if(rxne_en)
		uartx->CR1 |= USART_REG_CR1_RXNE_INT_EN;
	else
		uartx->CR1 &= ~USART_REG_CR1_RXNE_INT_EN;

}


static void hal_uart_clear_error_flag(uart_handle_t *handle){
	
	uint32_t tmpreg = 0x00;
	tmpreg = handle->Instance->ISR;		// lee el ISR
	tmpreg = handle->Instance->RDR;		// lee el registro de recepción

}

/**
  * @brief  Handle the RXNE interrupt 
  * @param  huart: pointer to a uart_handle_t structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
static void hal_uart_handle_RXNE_interrupt(uart_handle_t *handle){

	uint32_t tmp1 = 0;
	
	tmp1 = handle->rx_state;
	
	if( tmp1 == HAL_UART_STATE_BUSY_RX){
		// is application usig parity??
		if(handle->Init.Parity == USART_PARITY_NONE){
			*handle->pRxBuffPtr++ = (uint8_t)(handle->Instance->RDR & (uint8_t)0x00FF);
		}
		else{
			// yes, don't read the MSB, because its a parity bit
			*handle->pRxBuffPtr++ = (uint8_t)(handle->Instance->RDR & (uint8_t)0x007F);
		}
		
		/* are we done with the reception ?? */
		if(--handle->RxXferCount == 0){
			// yes, disable the RXNE interrupt
			handle->Instance->CR1 &= ~USART_REG_CR1_RXNE_INT_EN;
			
			/* Disable the UART parity error interrupt */
			handle->Instance->CR1 &= ~USART_REG_CR1_PEIE_INT_EN;
			
			/* Disable the UART Error Interrupt (Frame error, noise error, overrun error) */
			handle->Instance->CR3 &= ~USART_REG_CR3_ERR_INT_EN;
			
			// make the state ready for this handle
			handle->rx_state = HAL_UART_STATE_READY;
			
			/* call the application callback */
			if(handle->rx_cmp_cb)
				handle->rx_cmp_cb(&handle->RxXferSize);
		
		}
	
	}
	

}


/**
  * @brief  handle the TXE interrupt 
  * @param  huart: Pointer to a uart_handle_t structure that contains
  *                the configuration information for the specified UART module.
  * @retval none
  */
static void hal_uart_handle_TXE_interrupt(uart_handle_t *handle){

	uint32_t tmp1 = 0;
	uint8_t val;
	
	tmp1 = handle->tx_state;
	if(tmp1 == HAL_UART_STATE_BUSY_TX){
		val = (uint8_t)(*handle->pTxBuffPtr++ & (uint8_t)0x00FF);
		handle->Instance->TDR = val;
		
		if(--handle->TxXferCount == 0){
			/* Disable the UART TXE Interrupt */
			handle->Instance->CR1 &= ~USART_REG_CR1_TXE_INT_EN;
			
			/* Enable the UART Transmit Complete Interrupt */
			handle->Instance->CR1 |= USART_REG_CR1_TCIE_INT_EN;
		}
	}

}

/**
  * @brief Handle the Transmission Complete (TC) interrupt 
  * @param  huart: pointer to a uart_handle_t structure that contains
  *                the configuration information for the specified UART module.
  * @retval void
  */
static void hal_uart_handle_TC_interrupt(uart_handle_t *handle){
	
	/* Disable the UART Transmit Complete Interrupt */
	handle->Instance->CR1 &= ~USART_REG_CR1_TCIE_INT_EN;
	handle->tx_state = HAL_UART_STATE_READY;
	
	/* call the application callback */
	if(handle->tx_cmp_cb)
		handle->tx_cmp_cb(&handle->TxXferSize);
}


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
void hal_uart_init(uart_handle_t *handle){

	/* Configure the word length */
	hal_uart_configure_word_length(handle->Instance, handle->Init.WordLength);
	
	/* Configure the number of STOP bits */
	hal_uart_configure_stop_bits(handle->Instance, handle->Init.StopBits);
	
	
	/* Configure the oversampling rate for the receive block */
	hal_uart_configure_over_sampling(handle->Instance, handle->Init.OverSampling);
	
	
	/* Set the baudrate */
	hal_uart_set_baud_rate(handle->Instance, handle->Init.BaudRate);
	
	
	/* Enable the transmit block of the UART peripheral */
	hal_uart_enable_disable_tx(handle->Instance, handle->Init.Mode);
	
	
	/* Enable the receive block of the UART peripheral */
	hal_uart_enable_disable_rx(handle->Instance, handle->Init.Mode);
	
	
	/* Enable the UART peripheral */
	hal_uart_enable(handle->Instance);
	
	// Inicializamos los estados 
	handle->tx_state = HAL_UART_STATE_READY;
	handle->rx_state = HAL_UART_STATE_READY;
	handle->ErrorCode = HAL_UART_ERROR_NONE;
	
	

}



/**
	* @brief  API to do UART data Transmission
	* @param  *uart_handle : pointer to the handle structure of the UART Peripheral 
  * @param  *buffer : holds the pointer to the TX buffer 
  * @param  len : len of the data to be TXed
	* @retval None
	*/
void hal_uart_tx(uart_handle_t *handle, uint8_t *buffer, uint32_t len){

	/* Populate the application given info in to the UART handle structure */
	handle->pTxBuffPtr = buffer;
	handle->TxXferCount = len;
	handle->TxXferSize = len;
	
	/* This handle is busy in doing the TX */
	handle->tx_state = HAL_UART_STATE_BUSY_TX;
	
	/* Enable the UART peripheral */
	hal_uart_enable(handle->Instance);
	
	
	/* Enable the TXE interrupt */
	hal_uart_configure_txe_interrupt(handle->Instance, 1);
}



/**
	* @brief  API to do UART data Reception  
	* @param  *handle : pointer to the handle structure of the UART peripheral  
  * @param  *buffer : holds the pointer to the RX buffer 
  * @param  len : len of the data to be RXed
	* @retval None
	*/
void hal_uart_rx(uart_handle_t *handle, uint8_t *buffer, uint32_t len){

	uint32_t val;
	
	/* Populate the application given info in to the UART handle structure */
	handle->pRxBuffPtr = buffer;
	handle->RxXferCount = len;
	handle->RxXferSize = len;
	
	/* This enable is busy in doing the RX*/
	handle->rx_state = HAL_UART_STATE_BUSY_RX;
	
	/* En el caso de la recepción habilitamos las interrupciones por errores */
	/* Enable the UART parity error interrupt */
	hal_uart_configure_parity_error_interrupt(handle->Instance, 1);
	
	/* Enable the UART error interrupt (frame error, noise error, overrun error) */
	hal_uart_configure_error_interrupt(handle->Instance, 1);
	
	// Hacemos una primera lectura del buffer rx, para que no nos salte la
	// interrupción por buffer no vacío al configurarlo por primera vez
	val = handle->Instance->RDR;
	
	/* Enable the UART RXNE interrupt */
	hal_uart_configure_rxne_interrupt(handle->Instance, 1);
	

}



/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: pointer to a uart_handle_t structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void hal_uart_handle_interrupt(uart_handle_t *handle){

	uint32_t tmp1 = 0, tmp2 = 0;
	
	tmp1 = handle->Instance->ISR & USART_REG_SR_PE_FLAG;
	tmp2 = handle->Instance->CR1 & USART_REG_CR1_PEIE_INT_EN;
	/* UART parity error interrupt ------------------------------*/
	if((tmp1) && (tmp2)){
		hal_uart_clear_error_flag(handle);
		handle->ErrorCode |= HAL_UART_ERROR_PE;
	}


	tmp1 = handle->Instance->ISR & USART_REG_SR_FE_FLAG;
	tmp2 = handle->Instance->CR3 & USART_REG_CR3_ERR_INT_EN;
	/* UART frame error interrupt ----------------------------- */
	if((tmp1) && (tmp2)){
		hal_uart_clear_error_flag(handle);
		handle->ErrorCode |= HAL_UART_ERROR_FE;
	}
	
	
	tmp1 = handle->Instance->ISR & USART_REG_SR_NE_FLAG;
	tmp2 = handle->Instance->CR3 & USART_REG_CR3_ERR_INT_EN;
	/* UART noise error interrupt ----------------------------- */
	if((tmp1) && (tmp2)){
		hal_uart_clear_error_flag(handle);
		handle->ErrorCode |= HAL_UART_ERROR_NE;
	}	
	
	tmp1 = handle->Instance->ISR & USART_REG_SR_ORE_FLAG;
	tmp2 = handle->Instance->CR3 & USART_REG_CR3_ERR_INT_EN;
	/* UART overrun interrupt --------------------------------- */
	if((tmp1) && (tmp2)){
		hal_uart_clear_error_flag(handle);
		handle->ErrorCode |= HAL_UART_ERROR_ORE;
	}	
	
	tmp1 = handle->Instance->ISR & USART_REG_SR_RXNE_FLAG;
	tmp2 = handle->Instance->CR1 & USART_REG_CR1_RXNE_INT_EN;
	/* UART in mode receiver ---------------------------------- */
	if((tmp1) && (tmp2)){
		hal_uart_handle_RXNE_interrupt(handle);
	}	
	
	tmp1 = handle->Instance->ISR & USART_REG_SR_TXE_FLAG;
	tmp2 = handle->Instance->CR1 & USART_REG_CR1_TXE_INT_EN;
	/* UART in mode transmitter ------------------------------- */
	if((tmp1) && (tmp2)){
		hal_uart_handle_TXE_interrupt(handle);
	}	
	
	tmp1 = handle->Instance->ISR & USART_REG_SR_TC_FLAG;
	tmp2 = handle->Instance->CR1 & USART_REG_CR1_TCIE_INT_EN;
	/* UART in mode transmitter end --------------------------- */
	if((tmp1) && (tmp2)){
		hal_uart_handle_TC_interrupt(handle);
	}

}



/******************************************************************************/
/*                                                                            */
/*                      Callback functions                                    */
/*                                                                            */
/******************************************************************************/

/**
  * @brief  UART error callbacks.
  * @param  huart: pointer to a uart_handle_t structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void hal_uart_error_cb(uart_handle_t *handle){
	
	while(1)
		led_turn_on(GPIOJ,LED_RED);

}





