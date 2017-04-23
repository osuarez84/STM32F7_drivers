#include <stdint.h>
#include "hal_gpio_driver.h"
#include "hal_spi_driver.h"
#include "led.h"




/* PRIVATE VARIABLES */
/* SPI handle for our SPI device */
spi_handle_t SpiHandle;


/* master read/write buffers */
// TEST DATA
uint16_t master_write_data[]={ 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD };





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
	
	// TODO : no necesitamos el GPIO paca el CS ya que el control
	// lo vamos a configurar por software mediante la NSS
	
	
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
	
	/* Inicializamos el HSE como clock del sistema (25 MHz) */
	initClock();

	/* Initializes the LEDs */
	led_init();
	
	//enable clock for RCC
	RCC->APB2ENR |= 0x00004000;

	
	/* SPI SECTION */
	/* enable the clock for the SPI2 */
	_HAL_RCC_SPI2_CLK_ENABLE();
	
	/* fill up the handle structure */
	SpiHandle.Instance													= SPI_2;
	SpiHandle.Init.BaudRatePrescaler						= SPI_REG_CR1_BR_PCLK_DIV_32;
	SpiHandle.Init.Direction										= SPI_ENABLE_2_LINE_UNI_DIR;
	SpiHandle.Init.CLKPhase											= SPI_SECOND_CLOCK_TRANS;
	SpiHandle.Init.CLKPolarity									= SPI_CPOL_LOW;
	SpiHandle.Init.DataSize											= SPI_DATASIZE_16;
	SpiHandle.Init.FirstBit											= SPI_TX_MSB_FIRST;
	SpiHandle.Init.NSS													= SPI_SSM_ENABLE;
	SpiHandle.Init.Mode													= SPI_MASTER_MODE_SEL;
	
	SpiHandle.State															= HAL_SPI_STATE_READY;
	
	/* Call driver API to initialize the SPI device */
	hal_spi_init(&SpiHandle);
	
	/* Enable the IRQs in the NVIC */
	NVIC_EnableIRQ(SPI2_IRQn);
	
	
	/* USART SECTION */
	// TODO
	
	
	/* TIM6 SECTION */
	// TODO

#if 1
	while(1)
	{
		led_turn_on(GPIOJ,LED_GREEN);
		led_turn_on(GPIOJ,LED_RED);

		for(i=0;i<500000;i++);

		led_turn_off(GPIOJ,LED_GREEN);
		led_turn_off(GPIOJ,LED_RED);

		for(i=0;i<500000;i++);

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


//================================================
