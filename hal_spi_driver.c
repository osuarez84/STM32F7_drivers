#include "hal_spi_driver.h"


/******************************************************************************/
/*                                                                           	*/
/*                        			Helper functions											        */
/*                                                     												*/
/******************************************************************************/

static void hal_spi_enable(SPI_TypeDef *SPIx){



}


static void hal_spi_disable(SPI_TypeDef *SPIx){



}

static void hal_spi_configure_phase_and_polarity(SPI_TypeDef *SPIx, uint32_t phase_value, uint32_t polarity){



}


static void hal_spi_configure_device_mode(SPI_TypeDef *SPIx, uint32_t master){




}


static void hal_spi_configure_datasize(SPI_TypeDef *SPIx, uint32_t datasize_16, uint32_t lsbfirst){




}

static void hal_spi_configure_nss_master(SPI_TypeDef *SPIx, uint32_t ssm_enable){



}


static void hal_spi_configure_nss_slave(SPI_TypeDef *SPIx, uint32_t ssm_enable){




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
