#include <stdint.h>
#include "hal_gpio_driver.h"
#include "hal_spi_driver.h"
#include "hal_usart_driver.h"
#include "led.h"




/* PRIVATE VARIABLES */
/* Devices handlers */
spi_handle_t SpiHandle;
uart_handle_t uartHandle;


/* master read/write buffers */
// TEST DATA
uint16_t master_write_data[]={ 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD };
uint8_t addrcmd[2];

uint8_t message1[] = "STM32F7xx Hello world!!";

/* UART buffers */
uint8_t UART_rxBuff[39];

/**
	* @brief  Initialize the SPI2 pin
	* @param  None
	* @retval None
	*/
/* Configure GPIO for SPI functionality */
void spi_gpio_init(void){

	gpio_pin_conf_t spi_conf;
	
	_HAL_RCC_GPIOA_CLK_ENABLE();
	_HAL_RCC_GPIOB_CLK_ENABLE();

	
	/* configure GPIOA_PIN_12 for SPI CLK functionality */
	spi_conf.pin = SPI_CLK_PIN;
	spi_conf.mode = GPIO_PIN_ALT_FUN_MODE;
	spi_conf.op_type = GPIO_PIN_OP_TYPE_PUSHPULL;
	spi_conf.pull = GPIO_PIN_PULL_DOWN;
	spi_conf.speed = GPIO_PIN_SPEED_MEDIUM;
	
	hal_gpio_set_alt_function(GPIOA, SPI_CLK_PIN, GPIO_PIN_AF5_SPI2);
	hal_gpio_init(GPIOA, &spi_conf);
	
	/* configure GPIOB_PIN_15 for MOSI functionality */
	spi_conf.pin = SPI_MOSI_PIN;
	spi_conf.pull = GPIO_PIN_PULL_UP;
	hal_gpio_set_alt_function(GPIOB, SPI_MOSI_PIN, GPIO_PIN_AF5_SPI2);
	hal_gpio_init(GPIOB, &spi_conf);
	
	/* configure GPIOx for MISO functionality */
	// TODO : para la comunicación con el DAC 
	// no vamos a necesitar la línea MISO.
	
	// TODO : no necesitamos el GPIO para el CS ya que el control
	// lo vamos a configurar por software mediante la NSS
	
	
}


/**
	* @brief  Initialize the USART6 pin
	* @param  None
	* @retval None
	*/
void uart_gpio_init(void){

	gpio_pin_conf_t uart_pin_conf;
	
	/* enable the clock for the GPIO port C */
	_HAL_RCC_GPIOC_CLK_ENABLE();
	
	/* configure the GPIO_PORT_C_PIN_6 as TX */
	uart_pin_conf.pin = USARTx_TX_PIN;
	uart_pin_conf.mode = GPIO_PIN_ALT_FUN_MODE;
	uart_pin_conf.op_type = GPIO_PIN_OP_TYPE_PUSHPULL;
	uart_pin_conf.speed = GPIO_PIN_SPEED_HIGH;
	uart_pin_conf.pull = GPIO_PIN_NO_PULL_PUSH;
	hal_gpio_set_alt_function(GPIOC, USARTx_TX_PIN, USARTx_TX_AF);
	hal_gpio_init(GPIOC, &uart_pin_conf);
	
	
	/* configure the GPIO PORT_C_PIN_7 as RX */
	uart_pin_conf.pin = USARTx_RX_PIN;
	hal_gpio_set_alt_function(GPIOC, USARTx_RX_PIN, USARTx_RX_AF);
	hal_gpio_init(GPIOC, &uart_pin_conf);
}



/**
	* @brief  Initialize the LEDs 
	* @param  None
	* @retval None
	*/
void led_init(void)
{
	
	gpio_pin_conf_t led_pin_conf;
	
	/* Enable the clock for the GPIOJ port */
	_HAL_RCC_GPIOJ_CLK_ENABLE();
	
	led_pin_conf.pin = LED_GREEN;
	led_pin_conf.mode = GPIO_PIN_OUTPUT_MODE;
	led_pin_conf.op_type = GPIO_PIN_OP_TYPE_PUSHPULL;
	led_pin_conf.speed = GPIO_PIN_SPEED_MEDIUM;
	led_pin_conf.pull = GPIO_PIN_NO_PULL_PUSH;
	hal_gpio_init(GPIOJ, &led_pin_conf);
	
	led_pin_conf.pin = LED_RED;
	hal_gpio_init(GPIOJ, &led_pin_conf);
}


/**
	* @brief  Turns ON the led which is connected on the given pin  
	* @param  *GPIOx : Base address of the GPIO Port
	* @param  Pin : pin number of the LED
	* @retval None
	*/
void led_turn_on(GPIO_TypeDef *GPIOx, uint16_t pin)
{
	hal_gpio_write_to_pin(GPIOx,pin, 1);
	
}

/**
	* @brief  Turns OFF the led which is connected on the given pin  
	* @param  *GPIOx : Base address of the GPIO Port
	* @param  Pin : pin number of the LED
	* @retval None
	*/
void led_turn_off(GPIO_TypeDef *GPIOx, uint16_t pin)
{
	hal_gpio_write_to_pin(GPIOx,pin, 0);
	
}

/**
	* @brief  Toggels the led which is connected on the given pin  
	* @param  *GPIOx : Base address of the GPIO Port
	* @param  Pin : pin number of the LED
	* @retval None
	*/
void led_toggle(GPIO_TypeDef *GPIOx, uint16_t pin)
{
	if(hal_gpio_read_from_pin(GPIOx,pin))		// read value from pin...
	{
		 hal_gpio_write_to_pin(GPIOx,pin, 0);
	}else
	{
		 hal_gpio_write_to_pin(GPIOx,pin, 1);
		
	}
	
#if 0
	//Logic 2
	hal_gpio_write_to_pin(GPIOx,pin, ~(hal_gpio_read_from_pin(GPIOx,pin)));		// Compact way to use the hal_gpio_write_to_pin
#endif 
}


/**
* @brief  This function parses the command and takes action
* @param  *cmd :
* @retval None
*/
void 	parse_cmd(uint8_t *cmd)
{
	/*
	if( cmd[0] == 'L' && cmd[1] == 'E' && cmd[2] == 'D' )
	{
		if(cmd[3] == 'O' )
		{
			handle_cmd(cmd[4],LED_ORANGE);
			
		}else if(cmd[3] == 'B' )
		{
			handle_cmd(cmd[4],LED_BLUE);
			
		}else if(cmd[3] == 'G' )
		{
				handle_cmd(cmd[4],LED_GREEN);
			
		}else if(cmd[3] == 'R' )
		{
	    	handle_cmd(cmd[4],LED_RED);
		}else if (cmd[3] == 'A' )
		{
			handle_cmd(cmd[4],0xff);
		}
		else 
		{
			;
		}
		
		
		
	}else
	{
		error_handler();
		
	}
	*/
	
	/* TESTING COMMAND RECEPTION */
	if (cmd[0] == 'C' && cmd[1] == 'O' && cmd[2] == 'N' && \
		cmd[3] == 'E' && cmd[4] == 'C' && cmd[5] == 'T'){
		
			led_turn_on(GPIOJ, LED_GREEN);
		}
	
}




void initClock(){
	/* Selección de HSE como reloj del sistema */
	/* Enable HSE */
	RCC->CR |= (1 << 16);
	
	
	/* Wait until HSE is stable */
	while( !(RCC->CR & (1 << 17)) );
	
	/* change the system clock as HSE */
	RCC->CFGR &= ~(0x03);   // reseteamos los dos bits del registro 
	RCC->CFGR |= 0x01;			// ponemos a 1 el bit 0
	
	/* Disable HSI */
	RCC->CR &= ~(0x01);
	
	SystemCoreClockUpdate();

}


int main(void)
{
	uint32_t i;
	int32_t c;
	
	/* Inicializamos el HSE como clock del sistema (25 MHz) */
	initClock();

	/* Initializes the LEDs */
	led_init();
	
	//enable clock for RCC
	RCC->APB2ENR |= 0x00004000;

	
	/* SPI SECTION */
	/* configure GPIO for SPI2 */
	spi_gpio_init();
	
	/* enable the clock for the SPI2 */
	_HAL_RCC_SPI2_CLK_ENABLE();
	
	/* fill up the handle structure */
	SpiHandle.Instance										= SPI_2;
	SpiHandle.Init.BaudRatePrescaler						= SPI_REG_CR1_BR_PCLK_DIV_32;
	SpiHandle.Init.Direction								= SPI_ENABLE_2_LINE_UNI_DIR;
	SpiHandle.Init.OutputBidiMode						= SPI_ENABLE_RX_ONLY;
	SpiHandle.Init.CLKPhase									= SPI_SECOND_CLOCK_TRANS;
	SpiHandle.Init.CLKPolarity								= SPI_CPOL_LOW;
	SpiHandle.Init.DataSize									= SPI_DATASIZE_16;
	SpiHandle.Init.FirstBit									= SPI_TX_MSB_FIRST;
	SpiHandle.Init.NSS										= SPI_SSM_ENABLE;
	SpiHandle.Init.Mode										= SPI_MASTER_MODE_SEL;
	
	SpiHandle.State											= HAL_SPI_STATE_READY;
	
	/* Call driver API to initialize devices */
	hal_spi_init(&SpiHandle);
	
	/* Enable the IRQs in the NVIC */
	NVIC_EnableIRQ(SPI2_IRQn);
	
	
	/* USART SECTION */
	/* Configure GPIO for UART6 */
	uart_gpio_init();
	
	/* enable clock for USART6 */
	_HAL_RCC_USART6_CLK_ENABLE();
	
	uartHandle.Instance = USART_6;
	
	uartHandle.Init.BaudRate 				= USART_BAUD_115200;
	uartHandle.Init.WordLength 				= USART_WL_8;
	uartHandle.Init.StopBits				= USART_STOP_BITS_1;
	uartHandle.Init.Parity					= USART_PARITY_NONE;
	uartHandle.Init.Mode 					= USART_MODE_TX_RX;
	uartHandle.Init.OverSampling			= USART_OVER16_ENABLE;
	
	/* fill out the application callbacks */
	uartHandle.tx_cmp_cb = app_tx_cmp_callback;
	uartHandle.rx_cmp_cb = app_rx_cmp_callback;
	
	hal_uart_init(&uartHandle);
	
	/* enable the IRQ of USART peripheral */
	NVIC_EnableIRQ(USART6_IRQn);
	
	
	
	
	//Delay para esperar a que arranque el BT
	for(c = 0; c < 5000000; c++){}
		
	
	
	/* TEST SENDING */
	//while(uartHandle.tx_state != HAL_UART_STATE_READY);
	//hal_uart_tx(&uartHandle, message1, sizeof(message1)-1);
		
	
	/* TIM6 SECTION */
	// TODO

#if 1
	while(1)
	{
		/* TEST RECEIVING */
		//while(uartHandle.rx_state != HAL_UART_STATE_READY);
		//hal_uart_rx(&uartHandle, UART_rxBuff, 6);

		/* TEST SPI MASTER */
		while(SpiHandle.State != HAL_SPI_STATE_READY);
		
		addrcmd[0] = (uint8_t) master_write_data[0];
		addrcmd[1] = (uint8_t) (master_write_data[0] >> 8);
		
		/* first send the master write cmd to slave */
		hal_spi_master_tx(&SpiHandle, addrcmd, 2);
		
		// Delay entre envíos
		for(c = 0; c < 5000000; c++){}
		
		/*
		led_turn_on(GPIOJ,LED_GREEN);
		led_turn_on(GPIOJ,LED_RED);

		for(i=0;i<500000;i++);

		led_turn_off(GPIOJ,LED_GREEN);
		led_turn_off(GPIOJ,LED_RED);

		for(i=0;i<500000;i++);
		*/

	}

#endif 
	
}





//================================================
/* Handlers para la gestión de interrupciones */
// -----------------------------------------------
/**
	* @brief  ISR for the configured EXTI0 interrupt  
	* @retval None
	*/
void EXTI0_IRQHandler(void){

	
}



/**
  * @brief  This function handles SPI2 interrupt request.
  * @param  None
  * @retval None
  */
void SPI2_IRQHandler(void){
	/* call the driver api to process this interrupt */
  hal_spi_irq_handler(&SpiHandle);
}


void USART6_IRQHandler(void)
{
  hal_uart_handle_interrupt(&uartHandle);
}

//================================================

//================================================
/* Application callbacks */
// -----------------------------------------------

/* UART Callbacks */
/*This callback will be called by the driver when driver finishes the transmission of data */
void app_tx_cmp_callback(void *size)
{
 led_turn_on(GPIOJ,LED_RED);
	
}

/*This callback will be called by the driver when the application receives the command */
void app_rx_cmp_callback(void *size)
{
	//we got a command,  parse it 
	parse_cmd(UART_rxBuff);
	
	// TODO: aquí podemos volver a habilitar la interrupción RXNE, para
	// que vuelva a estár disponible la recepcion de datos.
	
}







//================================================