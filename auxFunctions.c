
#include "hal_gpio_driver.h"
#include "led.h"
#include "hal_usart_driver.h"
#include "hal_spi_driver.h"
#include "hal_EQ_techniques.h"
#include "auxFunctions.h"





/* PRIVATE FUNCTIONS ----------------------------------------------------------------------- */




/* PUBLIC FUNCTIONS ----------------------------------------------------------------------- */

/**
* @brief  This function parses the command and takes action
* @param  *cmd :
* @retval None
*/
void 	parse_cmd(uint8_t* cmd){

	/* TESTING COMMAND RECEPTION */
	uint8_t ack[] = "ACK";						// Mensaje ACK
	
	/* CONECT command */
	if (cmd[0] == 'C' && cmd[1] == 'O' && cmd[2] == 'N' && cmd[3] == 'E' && cmd[4] == 'C' && cmd[5] == 'T'){

	
			led_turn_on(GPIOJ, LED_GREEN);
						
			/* Disparamos evento FSM */
			// for testing pourposes using bluetooth
			communication_mode = C_BT;

			/* Enviamos ACK */
			//while(uartHandle.tx_state != HAL_UART_STATE_READY);
			//hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
			
			/* Salimos dejando a la espera los siguientes mensajes de CON */
			//while(uartHandle.rx_state != HAL_UART_STATE_READY);
			//hal_uart_rx(&uartHandle, UART_rxBuff, 39);						// Dejamos la recepción prevista para empezar a recibir datos tipo CON o DISCONECT
				
	}
		
	/* CON command */
	else if(cmd[0] == 'C' && cmd[1] == 'O' && cmd[2] == 'N'){			// Vamos a recibir datos 
		
		uint32_t i;
		
		// ¿Modo de funcionamiento...?
		switch(cmd[3]){
			case 0 : 							// pot
				df_mode = M_POT;
				break;
			case 1 :							// bipot
				df_mode = M_BIPOT;
				break;
			case 3:								// galv
				df_mode = M_GALV;
				break;
			case 4:								// EIS
				df_mode = M_EIS;
				break;
		}
		
		
		// Envio del ACK
		//while(uartHandle.tx_state != HAL_UART_STATE_READY);
		//hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
		
		
		// Dejamos la recepción prevista desde PC para siguiente ACK
		//while(uartHandle.rx_state != HAL_UART_STATE_READY);		
		//hal_uart_rx(&uartHandle, UART_rxBuff, 3);

		
		
	}
		
	
	/* DISCONN command */
	else if (cmd[0] == 'D' && cmd[1] == 'I' && cmd[2] == 'S' && cmd[3] == 'C' && cmd[4] == 'O' && cmd[5] == 'N' && cmd[6] == 'N'){

	
		if (next_state == IDLE){
			/* Pasamos a estado Conect */
			communication_mode = C_NONE;
		
			/* Recepción de CONECT por BT */
			//while(uartHandle.rx_state != HAL_UART_STATE_READY);		// Dejamos la recepción prevista por si es necesario cancelar desde PC
			//hal_uart_rx(&uartHandle, UART_rxBuff, 6);
			
			led_turn_off(GPIOJ, LED_GREEN);					
		}
		
		/* Enviamos ACK */
		//while(uartHandle.tx_state != HAL_UART_STATE_READY);
		//hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
				

	

	}

	/* ACK, PC ready to receive data! */
	else if(cmd[0] == 'A' && cmd[1] == 'C' && cmd[2] == 'K'){

		pc_ready_to_receive = READY;
		//while(uartHandle.rx_state != HAL_UART_STATE_READY);		
		//hal_uart_rx(&uartHandle, UART_rxBuff, 39);

	}
	
	
	
	/* STOP, the experiment is cancelled */
	else if(cmd[0] == 'S' && cmd[1] == 'T' && cmd[2] == 'O' && cmd[3] == 'P'){
		
		uint16_t i;
		
		// TODO : pretreatment = P_CANCELLED;
		experiment = E_CANCELLED;
	
		led_turn_off(GPIOJ, LED_GREEN);
		for (i = 0; i < 20000; i++){}
		led_turn_on(GPIOJ, LED_GREEN);
			
		// Dejamos la recepción de mensajes habilitada
		//while(uartHandle.rx_state != HAL_UART_STATE_READY);		
		//hal_uart_rx(&uartHandle, UART_rxBuff, 39);
			

	}
	
	
	
	
	/* Garbage handler */
	// TODO 
	// función para descartar basura o mensajes que no me sirvan por el UART6
	else{}
	
}




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


/* Functions to enable/disable the heartbeat ---------------------------------------------- */
/**
	* @brief  Configure and generate the heartbeat
	* @param  *handle : pointer to an tim67_handle_t structure
	* @retval None
	*/
//void heartbeat_enable(tim67_handle_t* handle){

	
	
	

//}

//void hearbeat_disable(tim67_handle_t* handle){




//}

/* Functions to control user LEDs ---------------------------------------------------------- */


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




