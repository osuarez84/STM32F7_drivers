#include <stdint.h>
#include "hal_gpio_driver.h"
#include "led.h"


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
	
	/* Inicializamos el HSE como clock rel sistema (25 MHz) */
	initClock();

	/* Initializes the LEDs */
	led_init();
	
	
	/* Enable the clock for the GPIOA Port */
	_HAL_RCC_GPIOA_CLK_ENABLE();
	
		//set the mode as input
	//GPIOA->MODER &= ~0x03;
	//GPIOA->PUPDR  &= ~0x03;
	
	GPIOA->MODER |= ( 0 << (2 * GPIO_BUTTON_PIN));
	GPIOA->PUPDR |= (0 << (2 * GPIO_BUTTON_PIN));
	
	//enable clock for RCC
	RCC->APB2ENR |= 0x00004000;
	
	/*Configure the button interrupt as falling edge */
	hal_gpio_configure_interrupt(GPIO_BUTTON_PIN, INT_RISING_FALLING_EDGE);
	
	/*Enable the interrupt on EXTI0 line */
	hal_gpio_enable_interrupt(GPIO_BUTTON_PIN,EXTI0_IRQn);

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
void EXTI0_IRQHandler(void)
{
  hal_gpio_clear_interrupt(GPIO_BUTTON_PIN);
	
	/* Do youR TASK here */
	led_toggle(GPIOJ,LED_RED);
	led_toggle(GPIOJ,LED_GREEN);
}
//================================================
