#ifndef __LED_H
#define __LED_H


/* Macros used for configuring gpios for I/O s */
#define 	GPIOA_PIN_6			6
#define 	GPIOA_PIN_4			4
#define 	GPIOB_PIN_14		14
#define 	GPIOB_PIN_8			8
#define 	GPIOB_PIN_11		11
#define 	GPIOC_PIN_8			8
#define 	GPIOF_PIN_6			6
#define 	GPIOF_PIN_7			7
#define 	GPIOH_PIN_6			6
#define		GPIOI_PIN_3			3
#define 	GPIOJ_PIN_4			4
#define 	GPIOJ_PIN_0			0
#define 	GPIOJ_PIN_1			1
#define 	GPIOC_PIN_2			2

#define		V_REF_ADC_DOUT		GPIOB_PIN_14		
#define 	W1_CH4						GPIOB_PIN_8		
#define 	W1_CH5						GPIOB_PIN_11		
#define		W1_CH1						GPIOH_PIN_6		
#define		W1_CH0						GPIOJ_PIN_4		
#define		W1_CH2						GPIOI_PIN_3		
#define		W1_CH3						GPIOF_PIN_7		
#define		I_W1_ADC_OUT			GPIOC_PIN_8		
#define		ADC_CS						GPIOJ_PIN_0		
#define 	ADC_CLK						GPIOF_PIN_6		
#define		FILT_W1_ON_OFF		GPIOJ_PIN_1		
#define		AUX_ON_OFF				GPIOA_PIN_6		
#define		W1_ON_OFF					GPIOA_PIN_4		
#define 	BT_RESET_PIN				GPIOC_PIN_2				// Actualmente no se utiliza
/* --------------------------------------------------------------- */

/* Macros used for configuring gpios for LEDs */
#define GPIOJ_PIN_13  13
#define GPIOJ_PIN_5   5	


#define LED_RED			GPIOJ_PIN_13
#define LED_GREEN		GPIOJ_PIN_5
/* --------------------------------------------------------------- */

/* Macros used for configuring gpios for SPI functionality */
#define GPIOA_PIN_12	12
#define GPIOB_PIN_15	15
#define GPIOB_PIN_9		9

#define	SPI_CLK_PIN		GPIOA_PIN_12
#define SPI_MOSI_PIN	GPIOB_PIN_15
#define SPI_CS_PIN		GPIOB_PIN_9

//#define SPI_MISO_PIN


/* SPI alternate functionality value */
#define GPIO_PIN_AF5_SPI2		0x05

/* Definition for SPIx's NVIC */
#define SPIx_IRQn					SPI2_IRQn
#define SPIx_IRQHandler				SPI2_IRQHandler
/* --------------------------------------------------------------- */



/* Macros used for configuring gpios for UART functionality */
#define GPIO_AF8_USART6					((uint8_t)0x08)

#define GPIO_PIN_6_SEL					6
#define GPIO_PIN_7_SEL					7



#define USARTx_TX_PIN					GPIO_PIN_6_SEL
#define USARTx_TX_GPIO_PORT				GPIOC
#define USARTx_TX_AF					GPIO_AF8_USART6
#define USARTx_RX_PIN					GPIO_PIN_7_SEL
#define USARTx_RX_GPIO_PORT				GPIOC
#define USARTx_RX_AF					GPIO_AF8_USART6

/* Definition for USARTx's NVIC */
#define USARTx_IRQn						USART6_IRQn
#define USARTx_IRQHandler				USART6_IRQHandler
/* --------------------------------------------------------------- */



/* Macros used for GPIOs configuration of diferent funcionalities */
// Bluetooth reset

/* --------------------------------------------------------------- */












// -------------------------------------------------------------------

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




/* USART callback function */
void app_tx_cmp_callback(void *size);
void app_rx_cmp_callback(void *size);




void sendDFUART(void);

#endif 