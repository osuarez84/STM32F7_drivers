#ifndef __LED_H
#define __LED_H

#include "hal_gpio_driver.h"

/* LEDs GPIO definitions */

#define GPIOJ_PIN_13  13
#define GPIOJ_PIN_5   5	


#define LED_GREEN     GPIOJ_PIN_13
#define LED_RED				GPIOJ_PIN_5

#define GPIO_BUTTON_PIN	0

/**
* @brief  Initialize the LEDs 
* @param  None
* @retval None
*/
void led_init(void);

/**
* @brief  Turns ON the led which is connected on the given pin  
* @param  *GPIOx : Base address of the GPIO Port
* @param  Pin : pin number of the LED
* @retval None
*/
void led_turn_on(GPIO_TypeDef *GPIOx, uint16_t pin);

/**
* @brief  Turns OFF the led which is connected on the given pin  
* @param  *GPIOx : Base address of the GPIO Port
* @param  Pin : pin number of the LED
* @retval None
*/
void led_turn_off(GPIO_TypeDef *GPIOx, uint16_t pin);

/**
* @brief  Toggels the led which is connected on the given pin  
* @param  *GPIOx : Base address of the GPIO Port
* @param  Pin : pin number of the LED
* @retval None
*/
void led_toggle(GPIO_TypeDef *GPIOx, uint16_t pin);

#endif 