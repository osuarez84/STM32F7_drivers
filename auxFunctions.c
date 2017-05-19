
#include "hal_gpio_driver.h"
#include "led.h"

/**
	* @brief  Reads the W1 ADC value
	* @param  None
	* @retval None
	*/
uint16_t read_ADC_W1(void){

	uint32_t i;
	uint16_t data = 0;
	
	// Poner a 0 CS y CLK
	hal_gpio_write_to_pin(GPIOJ, ADC_CS, 0);
	hal_gpio_write_to_pin(GPIOF, ADC_CLK, 0);
	
	
	// Ir leyendo cada bit de los 16 que se deben recibir
	for(i = 0; i <= 15; i++){
		hal_gpio_write_to_pin(GPIOF, ADC_CLK, 1);             // CLK = 1
		data = hal_gpio_read_from_pin(GPIOC, I_W1_ADC_OUT);
		data = (data << 1);
		hal_gpio_write_to_pin(GPIOF, ADC_CLK, 0);             // CLK = 0
	}
	
	// Poner a 1 CS y CLK
	hal_gpio_write_to_pin(GPIOJ, ADC_CS, 1);
	hal_gpio_write_to_pin(GPIOF, ADC_CLK, 1);
	return data;

}