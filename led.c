#include <stdint.h>
#include "hal_gpio_driver.h"
#include "hal_spi_driver.h"
#include "hal_usart_driver.h"
#include "hal_EQ_techniques.h "
#include "auxFunctions.h"
#include "led.h"




/* PRIVATE VARIABLES --------------------------------------------- */
/* Devices handlers */
spi_handle_t SpiHandle;
uart_handle_t uartHandle;


/* master read/write buffers */
// TEST DATA
uint16_t master_write_data[]={ 	0x8000,0xa78d,0xcb3c,0xe78d,0xf9bb,0xffff,0xf9bb,0xe78d,
																0xcb3c,0xa78d,0x8000,0x5872,0x34c3,0x1872,0x644,0x0,
																0x644,0x1872,0x34c3,0x5872,0x8000};
uint8_t addrcmd[2];

// UART messages
uint8_t message1[] = "DAT1234567891F";


/* UART buffers */
uint8_t UART_rxBuff[100];
uint8_t UART_txBuff[100];

																
/* LUTs for generating waveforms*/
float LUT1[10001];
float LUT2[10001];
float LUT3[10001];
float LUTcomplete[31000];																
uint16_t LUTdac[31000];				

uint16_t dataADC = 0x0000;

		
DF_CVTypeDef DF_CV;
DF_LSVTypeDef DF_LSV;
DF_SCVTypeDef DF_SCV;
DF_DPVTypeDef DF_DPV;			
DF_NPVTypeDef DF_NPV;
DF_DNPVTypeDef DF_DNPV;
DF_SWVTypeDef DF_SWV;
DF_ACTypeDef DF_ACV;
													
															
																
																
																
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
	
	/* configure GPIOx for CS functionality */
	spi_conf.pin = SPI_CS_PIN;
	spi_conf.mode = GPIO_PIN_OUTPUT_MODE;
	spi_conf.pull = GPIO_PIN_NO_PULL_PUSH;
	
	hal_gpio_init(GPIOB, &spi_conf);
	
	
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
	* @brief  Initialize I/O pins
	* @param  None
	* @retval None
	*/
void gpio_init(void){
	
	/* Declaramos las estructuras para inicializar los pines */
	gpio_pin_conf_t bt_pin_conf;
	gpio_pin_conf_t W1_gpio_conf;
	gpio_pin_conf_t ADCs_gpio_conf;
	gpio_pin_conf_t ON_OFF_gpio_conf;
	
	
	/* Habilitamos los relojes necesarios */
	_HAL_RCC_GPIOB_CLK_ENABLE();
	_HAL_RCC_GPIOH_CLK_ENABLE();
	_HAL_RCC_GPIOI_CLK_ENABLE();
	_HAL_RCC_GPIOF_CLK_ENABLE();
	_HAL_RCC_GPIOJ_CLK_ENABLE();
	

	
	/* Working 1 Channels */
	// CH0
	W1_gpio_conf.pin = 		W1_CH0;
	W1_gpio_conf.mode = 		GPIO_PIN_OUTPUT_MODE;
	W1_gpio_conf.op_type = 	GPIO_PIN_OP_TYPE_PUSHPULL;
	W1_gpio_conf.speed =		GPIO_PIN_SPEED_HIGH;
	W1_gpio_conf.pull = 		GPIO_PIN_NO_PULL_PUSH;
	
	hal_gpio_init(GPIOJ, &W1_gpio_conf);
	
	// CH1
	W1_gpio_conf.pin = 		W1_CH1;

	hal_gpio_init(GPIOH, &W1_gpio_conf);
	
	// CH2
	W1_gpio_conf.pin =		W1_CH2;
	
	hal_gpio_init(GPIOI, &W1_gpio_conf);
	
	// CH3
	W1_gpio_conf.pin =		W1_CH3;
	
	hal_gpio_init(GPIOF, &W1_gpio_conf);
	
	// CH4
	W1_gpio_conf.pin = 		W1_CH4;
	
	hal_gpio_init(GPIOB, &W1_gpio_conf);
	
	// CH5
	W1_gpio_conf.pin = 		W1_CH5;
	
	hal_gpio_init(GPIOB, &W1_gpio_conf);
	
	
	
	/* ADCs : I W1 y V REF */
	// ADC V REF
	ADCs_gpio_conf.pin = 		V_REF_ADC_DOUT;
	ADCs_gpio_conf.mode = 	GPIO_PIN_INPUT_MODE;
	ADCs_gpio_conf.speed =	GPIO_PIN_SPEED_HIGH;
	ADCs_gpio_conf.pull = 	GPIO_PIN_NO_PULL_PUSH;
	
	hal_gpio_init(GPIOB, &ADCs_gpio_conf);
	
	// ADC I W1
	ADCs_gpio_conf.pin = 	I_W1_ADC_OUT;

	hal_gpio_init(GPIOC, &ADCs_gpio_conf);
	
	// ADCs CS y CLK
	ADCs_gpio_conf.pin = 		ADC_CS;
	ADCs_gpio_conf.mode =		GPIO_PIN_OUTPUT_MODE;
	ADCs_gpio_conf.op_type =	GPIO_PIN_OP_TYPE_PUSHPULL;
	
	hal_gpio_init(GPIOJ, &ADCs_gpio_conf);
	
	ADCs_gpio_conf.pin = ADC_CLK;
	
	hal_gpio_init(GPIOF, &ADCs_gpio_conf);
	
	
	
	/* ON/OFF */
	// Filter ON/OFF
	ON_OFF_gpio_conf.pin = 		FILT_W1_ON_OFF;
	ON_OFF_gpio_conf.mode =		GPIO_PIN_OUTPUT_MODE;
	ON_OFF_gpio_conf.op_type =	GPIO_PIN_OP_TYPE_PUSHPULL;
	ON_OFF_gpio_conf.speed =	GPIO_PIN_SPEED_HIGH;
	ON_OFF_gpio_conf.pull =		GPIO_PIN_NO_PULL_PUSH;
	
	hal_gpio_init(GPIOJ, &ON_OFF_gpio_conf);
	
	// CE ON/OFF 
	ON_OFF_gpio_conf.pin =	AUX_ON_OFF;

	hal_gpio_init(GPIOA, &ON_OFF_gpio_conf);
	
	// W1 ON/OFF
	ON_OFF_gpio_conf.pin =	W1_ON_OFF;
	
	hal_gpio_init(GPIOA, &ON_OFF_gpio_conf);
	
	/* Bluetooth */
	bt_pin_conf.pin = BT_RESET_PIN;
	bt_pin_conf.mode = GPIO_PIN_OUTPUT_MODE;
	bt_pin_conf.op_type = GPIO_PIN_OP_TYPE_PUSHPULL;
	bt_pin_conf.speed = GPIO_PIN_SPEED_HIGH;
	bt_pin_conf.pull = GPIO_PIN_NO_PULL_PUSH;
	
	hal_gpio_init(GPIOC, &bt_pin_conf);
	

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
	uint32_t c;
	
	// CONECT command 
	if (cmd[0] == 'C' && cmd[1] == 'O' && cmd[2] == 'N' && \
		cmd[3] == 'E' && cmd[4] == 'C' && cmd[5] == 'T'){
		
			led_turn_on(GPIOJ, LED_GREEN);
			
			// delay
			for (c = 0; c <= 2000; c++){}
			
			led_turn_off(GPIOJ, LED_GREEN);
				

			sendDFUART();
			
			while(uartHandle.rx_state != HAL_UART_STATE_READY);
			hal_uart_rx(&uartHandle, UART_rxBuff, 6);						// Dejamos la recepción prevista para empezar a recibir datos
				
		}
		
	// DAT command 
	else if(cmd[0] == 'D' && cmd[1] == 'A' && cmd[2] == 'T'){			// Vamos a recibir datos 
			
		// Hacemos la tarea que toque
			
			
		while(uartHandle.rx_state != HAL_UART_STATE_READY);		// Dejamos la recepción prevista para empezar a recibir datos 
		hal_uart_rx(&uartHandle, UART_rxBuff, 42);
		
	}
		
	
	
}



/**
* @brief  This function initializes the SystemClock as HSE
* @param  None
* @retval None
*/
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





/* Testing functions *************************************************************/

void sendDFUART(){
		
	
	while(uartHandle.tx_state != HAL_UART_STATE_READY);
	hal_uart_tx(&uartHandle, message1, sizeof(message1)-1);
	

}


void sendSineSPI(){
	
	uint32_t count, c;
	
	
	for (count = 0; count <= 20; count++){

			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
			addrcmd[0] = (uint8_t) master_write_data[count];
			addrcmd[1] = (uint8_t) (master_write_data[count] >> 8);
			hal_spi_master_tx(&SpiHandle, addrcmd, 2);
			while(SpiHandle.State != HAL_SPI_STATE_READY);
			for(c = 0; c < 3000; c++){}													// MUST : Esperamos 0.1 ms entre cada envío para cumplir Timing del CLK (medido con Saleae)
			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH
			for(c = 0; c < 30; c++){}														// MUST : Generamos un CS HIGH de 4.5us para cumplir los requisitos de Timing del CS (medido con Saleae)

	}
	
		


}

void sendLUTSPI(uint32_t n){
	
	uint32_t count, c;
	
	
	for (count = 0; count <= n; count++){

			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
			addrcmd[0] = (uint8_t) LUTdac[count];
			addrcmd[1] = (uint8_t) (LUTdac[count] >> 8);
			hal_spi_master_tx(&SpiHandle, addrcmd, 2);
			while(SpiHandle.State != HAL_SPI_STATE_READY);
			for(c = 0; c < 3000; c++){}													// MUST : Esperamos 0.1 ms entre cada envío para cumplir Timing del CLK (medido con Saleae)
			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH 
			for(c = 0; c < 30; c++){}														// MUST : Generamos un CS HIGH de 4.5us para cumplir los requisitos de Timing del CS (medido con Saleae)
	}
	
	


}

/* Función de ejemplo para testear lectura ADC para una CV. Los tiempos de los bucles
están calculados de fomar empírica y aproximada. */
void sendLUTSPIandADC_CV(uint32_t n){
	
	uint32_t count, c;
	
	
	for (count = 0; count <= n; count++){
	
			uint32_t n = 0;
		
			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
			addrcmd[0] = (uint8_t) LUTdac[count];
			addrcmd[1] = (uint8_t) (LUTdac[count] >> 8);
			hal_spi_master_tx(&SpiHandle, addrcmd, 2);
			while(SpiHandle.State != HAL_SPI_STATE_READY);
			for(c = 0; c < 3000; c++){}													// MUST : Esperamos 0.1 ms entre cada envío para cumplir Timing del CLK (medido con Saleae)
			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH 
			for(c = 0; c < 30; c++){}														// MUST : Generamos un CS HIGH de 4.5us para cumplir los requisitos de Timing del CS (medido con Saleae)
			dataADC = read_ADC_W1();

			
			/* Send data using BT */
			UART_txBuff[0] = (uint8_t) dataADC;
			UART_txBuff[1] = (uint8_t) (dataADC >> 8);
			while(uartHandle.tx_state != HAL_UART_STATE_READY);
			hal_uart_tx(&uartHandle, UART_txBuff, 2);

			
			
	}
	
	


}


/* *************************************************************/



int main(void)
{
	uint32_t n;
	int32_t c;
	uint32_t count = 0;
	
	/* Inicializamos el HSE como clock del sistema (25 MHz) */
	initClock();

	/* Initializes the LEDs */
	led_init();
	
	//enable clock for RCC
	RCC->APB2ENR |= 0x00004000;

	
	/* SPI SECTION ---------------------------------------- */
	/* configure GPIO for SPI2 */
	spi_gpio_init();
	
	/* enable the clock for the SPI2 */
	_HAL_RCC_SPI2_CLK_ENABLE();
	
	/* fill up the handle structure */
	SpiHandle.Instance										= SPI_2;
	SpiHandle.Init.BaudRatePrescaler						= SPI_REG_CR1_BR_PCLK_DIV_256;
	SpiHandle.Init.Direction								= SPI_ENABLE_2_LINE_UNI_DIR;
	SpiHandle.Init.OutputBidiMode						= SPI_ENABLE_TX_ONLY;
	SpiHandle.Init.CLKPhase									= SPI_SECOND_CLOCK_TRANS;
	SpiHandle.Init.CLKPolarity								= SPI_CPOL_HIGH;
	SpiHandle.Init.DataSize									= SPI_DATASIZE_16;
	SpiHandle.Init.FirstBit									= SPI_TX_MSB_FIRST;
	SpiHandle.Init.NSS										= SPI_SSM_ENABLE;
	SpiHandle.Init.Mode										= SPI_MASTER_MODE_SEL;
	
	SpiHandle.State											= HAL_SPI_STATE_READY;
	
	/* Call driver API to initialize devices */
	hal_spi_init(&SpiHandle);
	
	// Ponemos SPI2 CS a HIGH
	hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);
	
	/* Enable the IRQs in the NVIC */
	NVIC_EnableIRQ(SPI2_IRQn);
	
	
	/* USART SECTION ---------------------------------------- */
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
	
		
	/* I/Os SECTION ---------------------------------------- */
	/* Configure GPIO for I/Os */
	gpio_init();
	
	/* Init the I/Os */
	/* Working 1 Channels */
	hal_gpio_write_to_pin(GPIOJ, W1_CH0, 0);
	hal_gpio_write_to_pin(GPIOH, W1_CH1, 0);
	hal_gpio_write_to_pin(GPIOI, W1_CH2, 0);
	hal_gpio_write_to_pin(GPIOF, W1_CH3, 0);
	hal_gpio_write_to_pin(GPIOB, W1_CH4, 0);
	hal_gpio_write_to_pin(GPIOA, W1_CH5, 1);
	
	/* ADCs : I W1 y V REF */
	hal_gpio_write_to_pin(GPIOJ, ADC_CS, 1);
	hal_gpio_write_to_pin(GPIOF, ADC_CLK, 1);
	
	
	/* ON/OFFs */
	// Filter ON/OFF
	hal_gpio_write_to_pin(GPIOJ, FILT_W1_ON_OFF, 1);

	
	// CE ON/OFF 
	hal_gpio_write_to_pin(GPIOA, AUX_ON_OFF, 0);
	
	// W1 ON/OFF
	hal_gpio_write_to_pin(GPIOA, W1_ON_OFF, 0);
	
	/* Bluetooth */
	// No utilizamos el reset aquí
	
	
	
	
	
	
	//Delay para esperar a que arranque el BT
	for(c = 0; c < 5000000; c++){}
		
		
		
	/* TEST UART SENDING */
	//while(uartHandle.tx_state != HAL_UART_STATE_READY);
	//hal_uart_tx(&uartHandle, message1, sizeof(message1)-1);
		
	/* TEST UART RECEIVING */
	//while(uartHandle.rx_state != HAL_UART_STATE_READY);
	hal_uart_rx(&uartHandle, UART_rxBuff, 6);		// Recibimos la ADDR
		

	/* Test SPI Master sending */
	/* Load CV data */
	DF_CV.Measurement.start = 1.4;
	DF_CV.Measurement.vtx1 = 2;
	DF_CV.Measurement.vtx2 = 1.1;
	DF_CV.Measurement.step = 0.1;
	DF_CV.Measurement.sr = 1;
	DF_CV.Measurement.scans = 1;
		
	/* Load LSV data */
	DF_LSV.Measurement.start = 0.3;
	DF_LSV.Measurement.stop = 4.3;
	DF_LSV.Measurement.step = 0.21;
	
	/* Load SCV data */
	DF_SCV.Measurement.start = 0.3;
	DF_SCV.Measurement.stop = 4;
	DF_SCV.Measurement.step = 0.46;
		
	/* Load DPV data */
	DF_DPV.Measurement.start = 0.38;
	DF_DPV.Measurement.stop = 1;
	DF_DPV.Measurement.step = 0.04;
	DF_DPV.Measurement.ePulse = 0.1;
	DF_DPV.Measurement.tPulse = 0.0012;
	DF_DPV.Measurement.sr = 8;
	
	/* Load NPV data */
	DF_NPV.Measurement.start = 1.96;
	DF_NPV.Measurement.start = 4.66;
	DF_NPV.Measurement.step = 0.1;
	DF_NPV.Measurement.tPulse = 0.002;
	DF_NPV.Measurement.sr = 6;
	
	/* Load DNPV data */
	DF_DNPV.Measurement.start = 1.26;
	DF_DNPV.Measurement.stop = 2.88;
	DF_DNPV.Measurement.step = 0.24;
	DF_DNPV.Measurement.ePulse = 0.13;
	DF_DNPV.Measurement.tPulse1 = 0.002;
	DF_DNPV.Measurement.tPulse2 = 0.002;
	DF_DNPV.Measurement.sr = 25;
	
	/* Load SWV data */
	DF_SWV.Measurement.start = 3.33;
	DF_SWV.Measurement.stop = 4.60;
	DF_SWV.Measurement.step = 0.13;
	DF_SWV.Measurement.amplitude = 0.12;
	DF_SWV.Measurement.freq = 13;
	
	/* Load ACV data */
	DF_ACV.Measurement.start = 1.21;
	DF_ACV.Measurement.stop = 2;
	DF_ACV.Measurement.step = 0.17;
	DF_ACV.Measurement.ACamplitude = 0.04;
	DF_ACV.Measurement.sr = 13;
	DF_ACV.Measurement.freq = 2000;
	
	
	
	/* Generamos LUT */
	/* Descomentar aquella que quiera probarse */
	/* CV */
	//n = generateCVsignal(&DF_CV, LUT1, LUT2, LUT3, LUTcomplete);
	
	/* LSV */
	//n = generateLSVsignal(&DF_LSV, LUTcomplete);
	
	/* SCV */
	//n = generateSCVsignal(&DF_SCV, LUT1, LUT2, LUT3, LUTcomplete);

	
	/* DPV */
	n = generateDPVsignal(&DF_DPV, LUTcomplete);

	
	/* NPV */
	//n = generateNPVsignal(&DF_NPV, LUTcomplete);

	
	/* DNPV */
	//n = generateDNPVsignal(&DF_DNPV, LUTcomplete);

	
	/* SWV */
	//n = generateSWVsignal(&DF_SWV, LUT1, LUT2, LUTcomplete);

	
	/* ACV */
	//n = generateACVsignal(&DF_ACV, LUT1, LUTcomplete);
	
	
	
	
	/* Generamos valores para el DAC */
	generateDACValues(LUTcomplete, LUTdac, n);
		
		

		
#if 1
	while(1)
	{
		sendLUTSPIandADC_CV(n);
		//sendSineSPI();
		//sendLUTSPI(n);

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

/**
  * @brief  This function handles SPI2 interrupt request.
  * @param  None
  * @retval None
  */
void USART6_IRQHandler(void)
{
  hal_uart_handle_interrupt(&uartHandle);
}

/**
  * @brief  This function handles TIM6 interrupt request.
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler(){

}

/**
  * @brief  This function handles TIM7 interrupt request.
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler(){
	
	
	
}


//================================================

//================================================
/* Application callbacks */
// -----------------------------------------------

/* UART Callbacks */
/*This callback will be called by the driver when driver finishes the transmission of data */
void app_tx_cmp_callback(void *size)
{
	uint32_t c;
	led_turn_on(GPIOJ,LED_RED);
	for (c = 0; c <= 2000; c++){}
	led_turn_off(GPIOJ, LED_RED);
	
}

/*This callback will be called by the driver when the application receives the command */
void app_rx_cmp_callback(void *size)
{
	//we got a command,  parse it 
	parse_cmd(UART_rxBuff);
	
	// TODO: aquí podemos volver a habilitar la interrupción RXNE, para
	// que vuelva a estár disponible la recepcion de datos.
	while(uartHandle.rx_state != HAL_UART_STATE_READY);
	hal_uart_rx(&uartHandle, UART_rxBuff, 6);
}







//================================================