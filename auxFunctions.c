
#include "hal_gpio_driver.h"
#include "led.h"
#include "hal_usart_driver.h"
#include "hal_spi_driver.h"
#include "hal_EQ_techniques.h"




/**
* @brief  This function parses the command and takes action
* @param  *cmd :
* @retval None
*/
void 	parse_cmd(uint8_t *cmd)
{

	/* TESTING COMMAND RECEPTION */
	uint32_t c;
	uint8_t ack[] = "ACK";
	
	// CONECT command 
	if (cmd[0] == 'C' && cmd[1] == 'O' && cmd[2] == 'N' && cmd[3] == 'E' && cmd[4] == 'C' && cmd[5] == 'T'){

	
			led_turn_on(GPIOJ, LED_GREEN);
			
			// delay
			//for (c = 0; c <= 2000; c++){}
			
			//led_turn_off(GPIOJ, LED_GREEN);
				
			// Disparamos el evento de la FSM	
			communication_mode = C_BT;

			// Envio del ACK
			while(uartHandle.tx_state != HAL_UART_STATE_READY);
			hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
			
			while(uartHandle.rx_state != HAL_UART_STATE_READY);
			hal_uart_rx(&uartHandle, UART_rxBuff, 9);						// Dejamos la recepción prevista para empezar a recibir datos tipo DAT
				
		}
		
	// DAT command 
	else if(cmd[0] == 'D' && cmd[1] == 'A' && cmd[2] == 'T'){			// Vamos a recibir datos 
			
		
			
		// TODO
		// Seleccionar el modo de funcionamiento
		df_mode = M_POT;
		
		// Miramos que experimento 
			
		
		
		// Envio del ACK
		while(uartHandle.tx_state != HAL_UART_STATE_READY);
		hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
		
		while(uartHandle.rx_state != HAL_UART_STATE_READY);		// Dejamos la recepción prevista por si es necesario cancelar desde PC
		hal_uart_rx(&uartHandle, UART_rxBuff, 6);
		
	}
		
	
	
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
