#include <stdint.h>
#include "hal_gpio_driver.h"
#include "hal_spi_driver.h"
#include "hal_usart_driver.h"
#include "hal_EQ_techniques.h "
#include "hal_timer6_driver.h"
#include "auxFunctions.h"
#include "led.h"


/* PRIVATE VARIABLES --------------------------------------------- */
/* Devices handlers */
spi_handle_t SpiHandle;
uart_handle_t uartHandle;
tim67_handle_t tim6Handle;
tim67_handle_t tim7Handle;


uint32_t contPretreatment_we1 = 0;
uint32_t contPretreatment_we2 = 0;
uint8_t DACvalue[2];

uint16_t dataADCwe1[1000];				// Array para almacenar datos leídos por el DAC
uint16_t dataADCwe2[1000];

uint32_t cont = 5;
uint16_t contAux_we1 = 0;
uint16_t contAux_we2 = 0;
uint16_t contPtos_we1 = 0;	// Contadores para controlar ptos enviados por USART6 a PC
uint16_t contPtos_we2 = 0;

// Contadores para probar cuántas veces se refrescan las LUTs. Para testeos del funcionamiento.
uint16_t contRefrescosLUTWE1A = 0;
uint16_t contRefrescosLUTWE1B = 0;
uint16_t contRefrescosLUTWE2A = 0;
uint16_t contRefrescosLUTWE2B = 0; 
													
uint8_t data_out[14] = {0};
uint8_t ack[] = "ACK";
uint8_t end[] = "END";


/* UART buffers */
uint8_t UART_rxBuff[100];
uint8_t UART_txBuff[100];

																
/* LUTs periódicas para el refresco continuo */												
uint16_t LUTWE1A[NSAMPLESLUT] = {0};
uint16_t LUTWE1B[NSAMPLESLUT] = {0};
uint16_t LUTWE2A[NSAMPLESLUT] = {0};
uint16_t LUTWE2B[NSAMPLESLUT] = {0};


uint8_t cont_bipot = 0;			// Contador para controlar la recepción de datos en modo bipot

/* Estructuras para guardar datos experimentos */
exp_param_values_t experimentWE1, experimentWE2;
pretreat_param_t pretreat_we1, pretreat_we2;
exp_config_t exp_config_we1, exp_config_we2;



/* Variables para guardar el número de samples tanto de
cada LUT/período como del experimento completo. Esto
nos permite llevar el control de si debemos de 
refrescar alguna LUT o si hemos llegado al final
del experimento */
uint16_t contMedidasADC_we1 = 0;
uint16_t contMedidasADC_we2 = 0;

												
/* Variables para el seguimiento de los estados
y eventos que disparan las transiciones */
stateType next_state;
state_exp_t next_state_exp_WE1, next_state_exp_WE2;
status_I_measure status_I_we1, status_I_we2;
mode_com communication_mode;
status_experiment eWE1, eWE2;
lut_status lutwe1A_state, lutwe1B_state, lutwe2A_state, lutwe2B_state;
mode df_mode;
mode mode_working;
status_pretreatment pWE1, pWE2;
state_pc pc_ready_to_receive;
pret_value_t real_pret_we1, real_pret_we2;
															
flag leerLUTAWE1;
flag leerLUTBWE1;
flag leerLUTAWE2;
flag leerLUTBWE2;
flag medidasADC_we1, medidasADC_we2;
flag enviar_dato_DAC_we1, enviar_dato_DAC_we2;
flag envio_end_we1, envio_end_we2;



/* INITIALIZING FUNCTIONS -------------------------------------------------------------- */
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







/* *************************************************************/

/* FSM functions -------------------------------------------- */
// Array que apunta a las funciones que corren en cada estado
void(*state_table[])(void) = {
	connection,
	Idle,
	calibration,
	bipot,
	pot,
	galv,
	eis,
	PrepE,
	Experiment,
	Ending,
	Error
};

// Pointer to functions array para subFSM de WE1
void (*state_table_we1[])(void) = {
	pretreatment_we1,
	measuring_we1,
	FS_change_we1,
};

// Pointer to funtions array para subFSM WE2
void (*state_table_we2[]) (void) = {
	pretreatment_we2,
	measuring_we2,
	FS_change_we2,
};



void start() {

	// inicializamos todo el equipo paso a paso
	mode_working = M_NONE;
	df_mode = M_NONE;
	communication_mode = C_NONE;
	
	status_I_we1 = I_DEFAULT;
	status_I_we2 = I_DEFAULT;

	lutwe1A_state = L_EMPTY;
	lutwe1B_state = L_EMPTY;
	lutwe2A_state = L_EMPTY;
	lutwe2B_state = L_EMPTY;
	eWE1 = E_NONE;
	eWE2 = E_NONE;
	pWE1 = P_NONE;
	pWE2 = P_NONE;
	
	pc_ready_to_receive = NOT_READY;
	
	cont_bipot = 2;
	
	next_state = CONECT;

}

/* STATE: Connection --------------------------------------------------- */
void connection() {

	if (communication_mode == C_BT) {
		// Recibimos los datos de conexión
		// Deshabilitamos el USB
		

		/* Salimos dejando a la espera los siguientes mensajes de CON */
		while(uartHandle.rx_state != HAL_UART_STATE_READY){};
		hal_uart_rx(&uartHandle, UART_rxBuff, 39);						// Dejamos la recepción prevista para empezar a recibir datos tipo CON o DISCONECT
		
		/* Enviamos ACK */
		while(uartHandle.tx_state != HAL_UART_STATE_READY){};
		hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
		
		
		
		// Configuramos y lanzamos el heartbeat
		
		next_state = IDLE;

	}
	else if (communication_mode == C_USB) {
		// Recibimos los datos de conexión
		// Deshabilitamos el BT
		// Enviamos el ACK al PC
		// Configuramos y lanzamos el hearbeat
		next_state = IDLE;
	}


	
}

/* STATE: Idle ----------------------------------------- */
void Idle() {

	if (df_mode == M_BIPOT) {
		next_state = BIPOT;
	}
	else if (df_mode == M_POT) {
				
		next_state = POT;
	}
	else if (df_mode == M_GALV) {
		next_state = GALV;
	}
	else if (df_mode == M_EIS) {
		next_state = EIS;
	}
	else if(communication_mode == C_NONE){
		
				
		/* Dejamos recepción prevista desde PC para trama CONECT de nuevo */
		while(uartHandle.rx_state != HAL_UART_STATE_READY){};		
		hal_uart_rx(&uartHandle, UART_rxBuff, 6);
		
		/* Enviamos ACK */
		while(uartHandle.tx_state != HAL_UART_STATE_READY){};
		hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);

		
		next_state = CONECT;
	}



}

void bipot() {
	
	cont_bipot--;
	
	// ¿Hemos recibido las dos tramas de datos del bipot...?
	if(cont_bipot == 0){							// Si			
		
		// Cargamos la estructura para el electródo WE2...		
		load_data(UART_rxBuff, &experimentWE2, &pretreat_we2, &exp_config_we2);
	
		// Generamos LUTWE2A y LUTWE2B
	
		// Dejamos la recepción prevista para recoger el ACK de READY
		while(uartHandle.rx_state != HAL_UART_STATE_READY){};		
		hal_uart_rx(&uartHandle, UART_rxBuff, 3);
		
		// Envio del ACK
		while(uartHandle.tx_state != HAL_UART_STATE_READY){};
		hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
		
			
		cont_bipot = 2;									// Reseteamos contador para la siguiente vez...
		mode_working = M_BIPOT;
		next_state = PREP_E;
	}
	else{															// No
		// Cargamos la estructura para el electródo WE1...
		load_data(UART_rxBuff, &experimentWE1, &pretreat_we1, &exp_config_we1);
	
		// Generamos LUTWE1A y LUTWE1B

		
		/* Salimos dejando a la espera los siguientes mensajes de CON */
		while(uartHandle.rx_state != HAL_UART_STATE_READY){};
		hal_uart_rx(&uartHandle, UART_rxBuff, 39);		
			
		// Envio del ACK
		while(uartHandle.tx_state != HAL_UART_STATE_READY){};
		hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
			
		// Volvemos al estado de espera de nuevo...
		df_mode = M_NONE;				
		next_state = IDLE;
	}

}

void pot() {
	

		mode_working = M_POT;
		
		// Cargamos los datos del experimento
		load_data(UART_rxBuff, &experimentWE1, &pretreat_we1, &exp_config_we1);
		
		// Dejamos la recepción prevista para recoger el ACK de READY
		while(uartHandle.rx_state != HAL_UART_STATE_READY){};		
		hal_uart_rx(&uartHandle, UART_rxBuff, 3);
		
		// Envio del ACK
		while(uartHandle.tx_state != HAL_UART_STATE_READY){};
		hal_uart_tx(&uartHandle, ack, sizeof(ack)-1);
		
		next_state = PREP_E;

}

void galv() {

	mode_working = M_GALV;
	next_state = PREP_E;
}


void eis() {

	// TODO : OJO a este modo. Vamos a poder funcionar como potenciostato o galvanostato. Tenerlo en
	// cuenta a la hora de gestionar el estado y la configuración del equipo en base a lo que se
	// seleccione.
	mode_working = M_EIS;
	next_state = PREP_E;
}

/* STATE: Prep_e ------------------------------------------------ */
void PrepE() {

	if (pc_ready_to_receive == READY){				// PC está listo para comenzar a recibir datos...
	

		if (mode_working == M_BIPOT) {
			// Configuramos FS
			// Configuramos filtros
			// Generamos primer refresco LUT1 y LUT2		
			// lut1A_state = REFRESHED y lut2A_state
			generate_data(&experimentWE1, &exp_config_we1, LUTWE1A);
			generate_data(&experimentWE1, &exp_config_we1, LUTWE1B);
			generate_data(&experimentWE2, &exp_config_we2, LUTWE2A);
			generate_data(&experimentWE2, &exp_config_we2, LUTWE2B);
			
			lutwe1A_state = L_REFRESHED;
			lutwe1B_state = L_REFRESHED;
			lutwe2A_state = L_REFRESHED;
			lutwe2B_state = L_REFRESHED;
			
			leerLUTAWE1 = YES;
			leerLUTBWE1 = NO;
			leerLUTAWE2 = YES;
			leerLUTBWE2 = NO;
			enviar_dato_DAC_we1 = NO;
			enviar_dato_DAC_we2 = NO;
			real_pret_we1 = tCond;
			real_pret_we2 = tCond;
			envio_end_we1 = YES;
			envio_end_we2 = YES;
			// Habilitamos electródos

		}
		else if (mode_working == M_POT) {
			// Configuramos FS auto o no
			// Configuramos filtros
			// Generamos primer refresco de LUT
			generate_data(&experimentWE1, &exp_config_we1, LUTWE1A);
			generate_data(&experimentWE1, &exp_config_we1, LUTWE1B);
			
			lutwe1A_state = L_REFRESHED;
			lutwe1B_state = L_REFRESHED;
			
			leerLUTAWE1 = YES;		// comenzamos a leer LUTA
			leerLUTBWE1 = NO;
			enviar_dato_DAC_we1 = NO;
			real_pret_we1 = tCond;
			envio_end_we1 = YES;
			
			// Habilitamos electródos

			// Aplicamos el pretratamiento que proceda
			


		}
		else if (mode_working == M_GALV) {
			// Configuramos FS auto o no
			// Configuramos filtros
			//Generamos primer refresco de LUT
			// lut1A_state = REFRESHED;

			// Habilitamos electródos

			// Aplicamos el pretratamiento que proceda

		}
		else if (mode_working == M_EIS) {
			// TODO

		}
		
		// Dejamos la recepción de mensajes de STOP
		while(uartHandle.rx_state != HAL_UART_STATE_READY){};		
		hal_uart_rx(&uartHandle, UART_rxBuff, 39);
		

		next_state = EXPERIMENT;
	
	}
	
	else{																		// PC NO está listo para recibir datos... (wait!)
		next_state = PREP_E;	
	}
		
}

/* STATE: Experiment ---------------------------------- */
void Experiment(void){
	
	if (df_mode == M_POT){
		subStateMachineWE1();
		
		if(eWE1 == E_FINISHED | eWE1 == E_CANCELLED){						// Ha terminado o ha sido cancelado el exp. en WE1...?
			
			if(eWE1 == E_FINISHED){
				
				if(envio_end_we1 == YES){	
					
					//uint8_t data_out[14] = {0};
					
					data_out[0] = 'E';
					data_out[1] = 'N';
					data_out[2] = 'D';
					data_out[3] = 0;
					data_out[4] = 0;
					data_out[5] = 0;
					data_out[6] = 0;
					data_out[7] = 0;
					data_out[8] = 0;
					data_out[9] = 0;
					data_out[10] = 0;
					data_out[11] = 0;
					data_out[12] = 0;
					data_out[13] = 'F';
					
					// Montamos trama datos finalización exp
					while(uartHandle.tx_state != HAL_UART_STATE_READY){};
					hal_uart_tx(&uartHandle, data_out, sizeof(data_out));
						
					envio_end_we1 = NO;
				
				}
			}
		
			else if(eWE1 == E_CANCELLED){
				
					if(envio_end_we1 == YES){
						// Deshabilitamos todas temporizaciones
						// WE1
						hal_tim67_int_disable(&tim6Handle); 					// Deshabilitamos interrupción
						hal_tim67_disable(&tim6Handle);								// Deshabilitamos timer
						hal_tim67_clear_flag(&tim6Handle);						// Borramos la flag de int pendiente
						
						// Montamos trama datos finalización por cancelación (mensaje END)
						while(uartHandle.tx_state != HAL_UART_STATE_READY){};
						hal_uart_tx(&uartHandle, end, sizeof(end)-1);
						
						// Habilitamos mensajes de CON de nuevo
						while(uartHandle.rx_state != HAL_UART_STATE_READY){};		
						hal_uart_rx(&uartHandle, UART_rxBuff, 39);
							
						envio_end_we1 = NO;
					}
			}
			
			
			
			
			next_state = ENDING;
		}
	}
	
	
	
	
	if (df_mode == M_BIPOT){
		subStateMachineWE1();
		subStateMachineWE2();
		
		if(eWE1 == E_FINISHED | eWE1 == E_CANCELLED | eWE2 == E_FINISHED | eWE2 == E_CANCELLED){		// Han terminado los exp. en WE1 y WE2...?
			
			/* Comprobamos estado experimento WE1 */
			if(eWE1 == E_FINISHED){
					
				if(envio_end_we1 == YES){	
						
					//uint8_t data_out[14] = {0};
						
					data_out[0] = 'E';
					data_out[1] = 'N';
					data_out[2] = 'D';
					data_out[3] = 0;
					data_out[4] = 0;
					data_out[5] = 0;
					data_out[6] = 0;
					data_out[7] = 0;
					data_out[8] = 0;
					data_out[9] = 0;
					data_out[10] = 0;
					data_out[11] = 0;
					data_out[12] = 0;
					data_out[13] = 'F';
						
					// Montamos trama datos finalización exp
					while(uartHandle.tx_state != HAL_UART_STATE_READY){};
					hal_uart_tx(&uartHandle, data_out, sizeof(data_out));
							
					envio_end_we1 = NO;
					
				}
			}
			
			
			/* Comprobamos estado experimento WE2 */
			if(eWE2 == E_FINISHED){
					
				if(envio_end_we2 == YES){	
						
					//uint8_t data_out[14] = {0};
						
					data_out[0] = 'E';
					data_out[1] = 'N';
					data_out[2] = 'D';
					data_out[3] = 1;
					data_out[4] = 0;
					data_out[5] = 0;
					data_out[6] = 0;
					data_out[7] = 0;
					data_out[8] = 0;
					data_out[9] = 0;
					data_out[10] = 0;
					data_out[11] = 0;
					data_out[12] = 0;
					data_out[13] = 'F';
						
					// Montamos trama datos finalización exp
					while(uartHandle.tx_state != HAL_UART_STATE_READY){};
					hal_uart_tx(&uartHandle, data_out, sizeof(data_out));
							
					envio_end_we2 = NO;
					
				}
			}
			
			
			
			/* Comprobamos si hemos terminado ambos experimentos. Si es así salimos. */
			//if((eWE1 == E_FINISHED | eWE1 == E_CANCELLED) & (eWE2 == E_FINISHED | eWE2 == E_CANCELLED)\
				& envio_end_we1 == NO & envio_end_we2 == NO){		// Han acabado los dos y se han enviado los mensajes de finalización? => nos vamos al estado ending...
				
			if(((eWE1 == E_FINISHED & envio_end_we1 == NO) & (eWE2 == E_FINISHED & envio_end_we2 == NO)) | (eWE1 == E_CANCELLED & eWE2 == E_CANCELLED)){
				
				
				/* antes de finalizar, si se han cancelado los experimentos deshabilitamos interrupciones y habilitamos el USART */
				if(eWE1 == E_CANCELLED & eWE2 == E_CANCELLED){
						
					// Deshabilitamos todas temporizaciones
					// WE1
					hal_tim67_int_disable(&tim6Handle); 					// Deshabilitamos interrupción
					hal_tim67_disable(&tim6Handle);								// Deshabilitamos timer
					hal_tim67_clear_flag(&tim6Handle);						// Borramos la flag de int pendiente
					
					// Deshabilitamos todas temporizaciones
					// WE2
					hal_tim67_int_disable(&tim7Handle); 					// Deshabilitamos interrupción
					hal_tim67_disable(&tim7Handle);								// Deshabilitamos timer
					hal_tim67_clear_flag(&tim7Handle);						// Borramos la flag de int pendiente
					
					// Montamos trama datos finalización por cancelación (mensaje END)
					while(uartHandle.tx_state != HAL_UART_STATE_READY){};
					hal_uart_tx(&uartHandle, end, sizeof(end)-1);
					
					// Habilitamos mensajes de CON de nuevo
					while(uartHandle.rx_state != HAL_UART_STATE_READY){};		
					hal_uart_rx(&uartHandle, UART_rxBuff, 39);
						
					envio_end_we1 = NO;
					envio_end_we2 = NO;
				
				}
				
				next_state = ENDING;
			}
			
		}
		
	}
	
	
}

/* Code for FSM WE1 */
void subStateMachineWE1(void){

	
	state_table_we1[next_state_exp_WE1]();

}


/* code for FSM WE2 */
void subStateMachineWE2(void){


	state_table_we2[next_state_exp_WE2]();
	
}


void pretreatment_we1(void){

	if(pWE1 == P_NONE){
		
		
		if(real_pret_we1 == tCond){
			
			if(pretreat_we1.tCond != 0){
				pWE1 = P_RUNNING;
				// Configuramos TIM6 para DAC WE1
				// Con los 25MHz del oscilador y con 1ms de disparo
				// para el timer por cada punto, necesitamos esta configuración :
				tim6Handle.Init.Period = 49;			
				tim6Handle.Init.Prescaler = 512;
					
				// Calculamos hasta donde debe de contar el contador según el tiempo que cuente el timer...
				// sabiendo que en cada disparo el timer cuenta aprox 1ms (0.001).
				contPretreatment_we1 = ceil((pretreat_we1.tCond / 1000.0) / 0.001);					// tiempo_pretratamiento(seg) / tiempo de trigger del timer
					
					
				hal_tim67_int_enable(&tim6Handle);
				hal_tim67_init(&tim6Handle);
					
				// Sacamos el valor por el DAC
	//			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
	//			DACvalue[0] = (uint8_t) pretreat_we1.tCond;
	//			DACvalue[1] = (uint8_t) (pretreat_we1.tCond >> 8);
	//			hal_spi_master_tx(&SpiHandle, DACvalue, 2);
	//			while(SpiHandle.State != HAL_SPI_STATE_READY);
	//			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH	
			}
			else{
				real_pret_we1 = tDep;
			}
			
		}
			
		else if(real_pret_we1 == tDep){
			
			if(pretreat_we1.tDep != 0){
				pWE1 = P_RUNNING;
				contPretreatment_we1 = ceil((pretreat_we1.tDep / 1000.0) / 0.001);
					
				tim6Handle.Init.Period = 49;			
				tim6Handle.Init.Prescaler = 512;
				hal_tim67_int_enable(&tim6Handle);
				hal_tim67_init(&tim6Handle);
					
				// Sacamos el valor por el DAC
	//			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
	//			DACvalue[0] = (uint8_t) pretreat_we1.tDep;
	//			DACvalue[1] = (uint8_t) (pretreat_we1.tDep >> 8);
	//			hal_spi_master_tx(&SpiHandle, DACvalue, 2);
	//			while(SpiHandle.State != HAL_SPI_STATE_READY);
	//			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH	
			
			}
			
			else {
				real_pret_we1 = tEq;
			}
		}
			
		else if(real_pret_we1 == tEq){
			
			if(pretreat_we1.tEq != 0){
				pWE1 = P_RUNNING;
				contPretreatment_we1 = ceil((pretreat_we1.tEq / 1000.0) / 0.001);
					
				tim6Handle.Init.Period = 49;			
				tim6Handle.Init.Prescaler = 512;
				hal_tim67_int_enable(&tim6Handle);
				hal_tim67_init(&tim6Handle);
					
				// Sacamos el valor por el DAC
	//			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
	//			DACvalue[0] = (uint8_t) pretreat_we1.tEq;
	//			DACvalue[1] = (uint8_t) (pretreat_we1.tEq >> 8);
	//			hal_spi_master_tx(&SpiHandle, DACvalue, 2);
	//			while(SpiHandle.State != HAL_SPI_STATE_READY);
	//			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH	
			}
			
			else{
				pWE1 = P_FINISHED;
			}
				
		}

	}
	else if(pWE1 == P_RUNNING){
		// Esperamos a que termine el pretratamiento en WE1...
		
	}
	
	else if(pWE1 == P_FINISHED | pWE1 == P_CANCELLED){
		
		//uint8_t data_out[14] = {0};
		
		// Enviamos finalización de pretratamiento
		data_out[0] = 'E';
		data_out[1] = 'N';
		data_out[2] = 'D';
		data_out[3] = 'P';
		data_out[4] = 'R';
		data_out[5] = 'E';
		data_out[6] = 'T';
		data_out[7] = 'W';
		data_out[8] = 'E';
		data_out[9] = '1';
		data_out[10] = 0;
		data_out[11] = 0;
		data_out[12] = 0;
		data_out[13] = 'F';
			
		// Montamos trama datos finalización exp
		while(uartHandle.tx_state != HAL_UART_STATE_READY){};
		hal_uart_tx(&uartHandle, data_out, sizeof(data_out));
		
		next_state_exp_WE1 = MEASURING_WE;
		
	}

}


void pretreatment_we2(void){
	// configurar el pretreatment WE2 aquí
	if(pWE2 == P_NONE){
		
		pWE2 = P_RUNNING;
		
		// Generamos señales de pretratamiento
			
		if(real_pret_we2 == tCond){
			// Configuramos TIM6 para DAC WE1
			// Con los 25MHz del oscilador y con 1ms de disparo
			// para el timer por cada punto, necesitamos esta configuración :
			tim7Handle.Init.Period = 49;			
			tim7Handle.Init.Prescaler = 512;
				
			// Calculamos hasta donde debe de contar el contador según el tiempo que cuente el timer...
			// sabiendo que en cada disparo el timer cuenta aprox 1ms (0.001).
			contPretreatment_we2 = ceil((pretreat_we2.tCond / 1000.0) / 0.001);					// tiempo_pretratamiento / tiempo de trigger del timer
				
				
			hal_tim67_int_enable(&tim7Handle);
			hal_tim67_init(&tim7Handle);
				
			// Sacamos el valor por el DAC
			// TODO
		}
			
		else if(real_pret_we2 == tDep){
			contPretreatment_we2 = ceil((pretreat_we2.tDep / 1000.0) / 0.001);
				
			tim7Handle.Init.Period = 49;			
			tim7Handle.Init.Prescaler = 512;
			hal_tim67_int_enable(&tim7Handle);
			hal_tim67_init(&tim7Handle);
				
			// Sacamos el valor por el DAC
			// TODO
			
		}
			
		else if(real_pret_we2 == tEq){
			contPretreatment_we2 = ceil((pretreat_we2.tEq / 1000.0) / 0.001);
				
			tim7Handle.Init.Period = 49;			
			tim7Handle.Init.Prescaler = 512;
			hal_tim67_int_enable(&tim7Handle);
			hal_tim67_init(&tim7Handle);
				
			// Sacamos el valor por el DAC
			// TODO
			
		}
		
	}
	
	else if(pWE2 == P_RUNNING){
		// Esperamos a que termine el pretratamiento en WE2...
		
	}
	
	else if (pWE2 == P_FINISHED | pWE2 == P_CANCELLED){
		
		//uint8_t data_out[14] = {0};
		
		// Enviamos finalización de pretratamiento
		data_out[0] = 'E';
		data_out[1] = 'N';
		data_out[2] = 'D';
		data_out[3] = 'P';
		data_out[4] = 'R';
		data_out[5] = 'E';
		data_out[6] = 'T';
		data_out[7] = 'W';
		data_out[8] = 'E';
		data_out[9] = '2';
		data_out[10] = 0;
		data_out[11] = 0;
		data_out[12] = 0;
		data_out[13] = 'F';
			
		// Montamos trama datos finalización exp
		while(uartHandle.tx_state != HAL_UART_STATE_READY){};
		hal_uart_tx(&uartHandle, data_out, sizeof(data_out));
		
		next_state_exp_WE2 = MEASURING_WE;
		
	}


}

void measuring_we1 (void){
	// configurar experimento WE1 aquí
	if (eWE1 == E_NONE){
		
		eWE1 = E_RUNNING;
			
		// Configuramos TIM6
		// Configuramos timer para fSampling = 10000
		tim6Handle.Init.Prescaler = 512;
		tim6Handle.Init.Period = ceil((1 / 10000.0) / ((tim6Handle.Init.Prescaler + 1) * (1 / MCU_FREQ)) );
			
		hal_tim67_int_enable(&tim6Handle);
		hal_tim67_init(&tim6Handle);
	}
	
	else if (eWE1 == E_RUNNING){
		
		if (enviar_dato_DAC_we1 == YES){
				if(leerLUTAWE1 == YES){							// Leemos de LUTA...
//					// Pasamos el sample al DAC
//					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
//					DACvalue[0] = (uint8_t) LUTWE1A[contSamplesLUTWE1];
//					DACvalue[1] = (uint8_t) (LUTWE1A[contSamplesLUTWE1] >> 8);
//					hal_spi_master_tx(&SpiHandle, DACvalue, 2);
//					while(SpiHandle.State != HAL_SPI_STATE_READY);
//					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH					

					experimentWE1.contSamplesLUT++;
								
							
					// ¿Hay que cambiar de LUT...?
					if(experimentWE1.contSamplesLUT == experimentWE1.nSamplesLUT){
						leerLUTAWE1 = NO;								// Cambiamos a LUTB
						leerLUTBWE1 = YES;
						lutwe1A_state = L_FINISHED;			// Avisamos a la FSM para que refresque...
						experimentWE1.contSamplesLUT = 0;					// Reiniciamos contador
					
					}

				}		
				else if(leerLUTBWE1 == YES){				// Leemos de LUTB
//					// Pasamos el sample al DAC
//					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
//					DACvalue[0] = (uint8_t) LUTWE1A[contSamplesLUTWE1];
//					DACvalue[1] = (uint8_t) (LUTWE1A[contSamplesLUTWE1] >> 8);
//					hal_spi_master_tx(&SpiHandle, DACvalue, 2);
//					while(SpiHandle.State != HAL_SPI_STATE_READY);
//					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH		

					experimentWE1.contSamplesLUT++;
					
					
					
					// ¿Hay que cambiar de LUT...?
					if(experimentWE1.contSamplesLUT == experimentWE1.nSamplesLUT){
						leerLUTBWE1 = NO;								// Cambiamos a LUTA
						leerLUTAWE1 = YES;
						lutwe1B_state = L_FINISHED;			// Avisamos a la FSM para que refresque...
						experimentWE1.contSamplesLUT = 0;					// Reiniciamos contador
					}
				
				}
				
				enviar_dato_DAC_we1 = NO;

			}

			/* Hay que tomar medidas en el ADC ? */
			if(medidasADC_we1 == YES){
				// Tomamos medida
				dataADCwe1[contMedidasADC_we1] = read_ADC_W1();
				
				contMedidasADC_we1++;

				if(contMedidasADC_we1 == NMEDIDASWE1){				// ¿Paramos de tomar medidas...?
					// Hacemos medida y enviamos datos
					// TODO
					//uint8_t data_out[14] = {0};
					//calculate media(dataADC);

					
					// Montamos el paquete de datos
					// Esto dependerá de cada tipo de técnica
					data_out[0] = 'D';
					data_out[1] = 'A';
					data_out[2] = 'T';
					data_out[3] = 0;
					data_out[4] = 5; //rand();
					data_out[5] = 5; //rand();
					data_out[6] = 5; //rand();
					data_out[7] = 5; //rand();
					data_out[8] = 5; //rand();
					data_out[9] = 5; //rand();
					data_out[10] = 0;
					data_out[11] = 0;
					data_out[12] = 0;
					data_out[13] = 'F';

					// Enviamos los datos
					while(uartHandle.tx_state != HAL_UART_STATE_READY){};
					hal_uart_tx(&uartHandle, data_out, sizeof(data_out));
					
					//enviar = YES;
					medidasADC_we1 = NO;
					contMedidasADC_we1 = 0;
					contPtos_we1++;
						

				}
			}

		
		/* Comprobamos si debemos de refrescar alguna LUT */
		if (lutwe1A_state == L_FINISHED) {
			// Refrescamos la LUT
			generate_data(&experimentWE1, &exp_config_we1, LUTWE1A);
			lutwe1A_state = L_REFRESHED;
			
			contRefrescosLUTWE1A++;
			
		}
		else if (lutwe1B_state == L_FINISHED) {
			// Refrescamos la LUT
			generate_data(&experimentWE1, &exp_config_we1, LUTWE1B);
			lutwe1B_state = L_REFRESHED;
			
			contRefrescosLUTWE1B++;
		}
		
		/* Comprobamos si debemos cambiar el FS */
		if (status_I_we1 != I_DEFAULT) {
			next_state_exp_WE1 = FS_CH_WE;
		}
		
				
	}
	
	
	else if (eWE1 == E_FINISHED | eWE1 == E_CANCELLED){
		// Estado de espera una vez el experimento haya terminado
		
	
	}

}


void measuring_we2 (void){
	// configurar experimento WE2 aquí
	if(eWE2 == E_NONE){
		
		eWE2 = E_RUNNING;


		// Configuramos TIM7
		// Configuramos timer para fSampling = 10000
		tim7Handle.Init.Prescaler = 512;
		tim7Handle.Init.Period = ceil((1 / 10000.0) / ((tim7Handle.Init.Prescaler + 1) * (1 / MCU_FREQ)) );
			
		hal_tim67_int_enable(&tim7Handle);
		hal_tim67_init(&tim7Handle);
	}
	
	else if (eWE2 == E_RUNNING){
		
				if (enviar_dato_DAC_we2 == YES){
					if(leerLUTAWE2 == YES){							// Leemos de LUTA...
//					// Pasamos el sample al DAC
//					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
//					DACvalue[0] = (uint8_t) LUTWE1A[contSamplesLUTWE1];
//					DACvalue[1] = (uint8_t) (LUTWE1A[contSamplesLUTWE1] >> 8);
//					hal_spi_master_tx(&SpiHandle, DACvalue, 2);
//					while(SpiHandle.State != HAL_SPI_STATE_READY);
//					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH					

					experimentWE2.contSamplesLUT++;
					
					// Habilitamos de nuevo el timer para el siguiente sample
					//hal_tim67_int_enable(&tim7Handle);				// Habilitamos interrupción TIM6
					//hal_tim67_enable(&tim7Handle);						// Habilitamos el timer					
					

					
					// ¿Hay que cambiar de LUT...?
					if(experimentWE2.contSamplesLUT == experimentWE2.nSamplesLUT){
						leerLUTAWE2 = NO;								// Cambiamos a LUTB
						leerLUTBWE2 = YES;
						lutwe2A_state = L_FINISHED;			// Avisamos a la FSM para que refresque...
						experimentWE2.contSamplesLUT = 0;					// Reiniciamos contador
					
					}

				}		
				else if(leerLUTBWE2 == YES){				// Leemos de LUTB
//					// Pasamos el sample al DAC
//					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
//					DACvalue[0] = (uint8_t) LUTWE1A[contSamplesLUTWE1];
//					DACvalue[1] = (uint8_t) (LUTWE1A[contSamplesLUTWE1] >> 8);
//					hal_spi_master_tx(&SpiHandle, DACvalue, 2);
//					while(SpiHandle.State != HAL_SPI_STATE_READY);
//					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH		

					experimentWE2.contSamplesLUT++;
					
					// Habilitamos de nuevo el timer para el siguiente sample
					//hal_tim67_int_enable(&tim7Handle);				// Habilitamos interrupción TIM6
					//hal_tim67_enable(&tim7Handle);						// Habilitamos el timer	
					
					

					
					// ¿Hay que cambiar de LUT...?
					if(experimentWE2.contSamplesLUT == experimentWE2.nSamplesLUT){
						leerLUTBWE2 = NO;								// Cambiamos a LUTA
						leerLUTAWE2 = YES;
						lutwe2B_state = L_FINISHED;			// Avisamos a la FSM para que refresque...
						experimentWE2.contSamplesLUT = 0;					// Reiniciamos contador
					}
				
				}
				enviar_dato_DAC_we2 = NO;
			
			}

			/* Hay que tomar medidas en el ADC ? */
			if(medidasADC_we2 == YES){
				// Tomamos medida
				//dataADC[contMedidasADC_we2] = read_ADC_W1();
				
				contMedidasADC_we2++;

				if(contMedidasADC_we2 == NMEDIDASWE2){				// ¿Paramos de tomar medidas...?
					// Hacemos medida y enviamos datos
					// TODO
					//uint8_t data_out[14] = {0};
					//calculate media(dataADC);

					
					// Montamos el paquete de datos
					// Esto dependerá de cada tipo de técnica
					data_out[0] = 'D';
					data_out[1] = 'A';
					data_out[2] = 'T';
					data_out[3] = 1;
					data_out[4] = 8; //rand();
					data_out[5] = 8; //rand();
					data_out[6] = 8; //rand();
					data_out[7] = 8; //rand();
					data_out[8] = 8; //rand();
					data_out[9] = 8; //rand();
					data_out[10] = 0;
					data_out[11] = 0;
					data_out[12] = 0;
					data_out[13] = 'F';

					// Enviamos los datos
					while(uartHandle.tx_state != HAL_UART_STATE_READY){};
					hal_uart_tx(&uartHandle, data_out, sizeof(data_out));
					
					//enviar = YES;
					medidasADC_we2 = NO;
					contMedidasADC_we2 = 0;
					contPtos_we2++;
				}
			}
		
		/* Comprobamos si debemos de refrescar alguna LUT */
		if (lutwe2A_state == L_FINISHED) {
			// Refrescamos la LUT
			generate_data(&experimentWE2, &exp_config_we2, LUTWE2A);
			lutwe2A_state = L_REFRESHED;
			
			contRefrescosLUTWE2A++;
		}
		else if (lutwe2B_state == L_FINISHED) {
			// Refrescamos la LUT
			generate_data(&experimentWE2, &exp_config_we2, LUTWE2B);
			lutwe2B_state = L_REFRESHED;
			
			contRefrescosLUTWE2B++;
		}
		
		/* Comprobamos si debemos cambiar el FS */
		if (status_I_we2 != I_DEFAULT) {
			next_state_exp_WE2 = FS_CH_WE;
		}
		
		hal_tim67_int_enable(&tim7Handle);				// Habilitamos interrupción TIM6
		hal_tim67_enable(&tim7Handle);						// Habilitamos el timer		
		
	}
	
	else if (eWE2 == E_FINISHED | eWE2 == E_CANCELLED){
		// Estado de espera una vez el experimento haya terminado
		
		
	}

}

void FS_change_we1 (void){
	// configurar cambio escala WE1
	if (status_I_we1 == I_SATURA) {
				// Cambiamos FS WE1 a un FS mayor
				// Reseteamos FLAG_SATURA_WE1 => flag_satura_we1 = DEFAULT;
			}
			else if (status_I_we1 == I_BELOW_THRESHOLD) {
				// Cambiamos FS WE1 a una FS menor
				// Reseteamos status_I_we1 => status_I_we1 = DEFAULT;

			}

	status_I_we1 = I_DEFAULT;		// Reseteamos el flag
	
	next_state_exp_WE1 = MEASURING_WE;

}

void FS_change_we2 (void){	
	// configurar cambio escala WE2
	if (status_I_we2 == I_SATURA) {
			// Cambiamos FS WE1 a un FS mayor
			// Reseteamos FLAG_SATURA_WE1 => flag_satura_we1 = DEFAULT;
		}
		else if (status_I_we2 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE1 a una FS menor
			// Reseteamos status_I_we1 => status_I_we1 = DEFAULT;

		}

	status_I_we2 = I_DEFAULT;		// Reseteamos el flag
	
	next_state_exp_WE2 = MEASURING_WE;

		
}



/* STATE: End ------------------------------------------- */
void Ending(void){
	
	// finalizamos e inicializamos todo lo necesario y volvemos a IDLE
	// Limpiamos todos los eventos
	mode_working = M_NONE;
	df_mode = M_NONE;
	status_I_we1 = I_DEFAULT;
	status_I_we2 = I_DEFAULT;
	pWE1 = P_NONE;
	pWE2 = P_NONE;
	eWE1 = E_NONE;
	eWE2 = E_NONE;
	lutwe1A_state = L_EMPTY;
	lutwe1B_state = L_EMPTY;
	lutwe2A_state = L_EMPTY;
	lutwe2B_state = L_EMPTY;


	pc_ready_to_receive = NOT_READY;
			
	// Variables de testeo ptos enviados al PC
	contPtos_we1 = 0;		// Nº de data frames enviados durante experimento WE1
	contPtos_we2 = 0;
	
	// Variables testeo refrescos de LUTs
	contRefrescosLUTWE1A = 0;
	contRefrescosLUTWE1B = 0;
	contRefrescosLUTWE2A = 0;
	contRefrescosLUTWE2B = 0;
	
	
	// Reinicializamos todos los estados
	next_state_exp_WE1 = PRETREATMENT_WE;
	next_state_exp_WE2 = PRETREATMENT_WE;	
	next_state = IDLE;
	

	
	
	
}



//-----------------------------------------------------------------------------------------------


void Error() {
	// En esta rutina podríamos gestionar los errores de alguna
	// manera o poner al equipo en estado de seguridad si detecta algún fallo.
	// Toda la gestión de los fallos está por definir.

}


void calibration() {
	// TODO :  queda por definir como va a funcionar el equipo durante la calibración


}





int main(void)
{
	
	int32_t c;
	gpio_pin_conf_t pinTest;
	
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
	
	/* TIM SECTION ---------------------------------------- */
	/*** TIM6 ***/
	/* enable clock for TIM6 */
	_HAL_RCC_TIM6_CLK_ENABLE();
	
	tim6Handle.Instance = TIM6;
	
	tim6Handle.Init.CounterMode = TIM_OPM_ENABLE;
	tim6Handle.Init.Prescaler = 512;
	tim6Handle.Init.AutoReloadPreload = TIM_ENABLE_AUTO_RELOAD;
	
	/* initialize the event flag */
	tim6Handle.int_event = NONE_EVENT;
	
	/* enable the IRQ of TIM6 peripheral */
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	
	/*** TIM7  ***/
	/* enable clock for TIM7 */
	_HAL_RCC_TIM7_CLK_ENABLE();
	
	tim7Handle.Instance = TIM7;
	tim7Handle.Init.CounterMode = TIM_OPM_ENABLE;
	tim7Handle.Init.Prescaler = 512;
	tim7Handle.Init.AutoReloadPreload = TIM_ENABLE_AUTO_RELOAD;
	
	/* initialize event flag */
	tim7Handle.int_event = NONE_EVENT;
	
	/* enable the IRQ of TIM7 peripheral */
	NVIC_EnableIRQ(TIM7_IRQn);
	
	
	// Establecemos prioridades de las interrupciones:
	//	=> la comunicación tiene prioridad sobre los datos que se envían al DAC
	//	y los que se envían al ADC
	NVIC_SetPriorityGrouping(4);

	NVIC_SetPriority(USART6_IRQn, 0);
	NVIC_SetPriority(TIM6_DAC_IRQn, 1);
	NVIC_SetPriority(TIM7_IRQn,1);
	NVIC_SetPriority(SPI2_IRQn, 1);
	
		
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
	// Reseteamos el BT al arrancar
	hal_gpio_write_to_pin(GPIOC, BT_RESET_PIN, 0);
	//Delay para esperar a que arranque el BT
	for(c = 0; c < 5000000; c++){}	
	hal_gpio_write_to_pin(GPIOC, BT_RESET_PIN, 1);
		
	/* ======= PIN TESTEO ====== */
	// Pin para TESTEO de tiempos de interrupción
	
	pinTest.pin = 		10;
	pinTest.mode = 		GPIO_PIN_OUTPUT_MODE;
	pinTest.op_type = 	GPIO_PIN_OP_TYPE_PUSHPULL;
	pinTest.speed =		GPIO_PIN_SPEED_HIGH;
	pinTest.pull = 		GPIO_PIN_NO_PULL_PUSH;
	hal_gpio_init(GPIOF, &pinTest);
	
		
		
	// Quedamos a la espera de recibir CONECT
	//while(uartHandle.rx_state != HAL_UART_STATE_READY);
	hal_uart_rx(&uartHandle, UART_rxBuff, 6);		
		
					
		
	/* Arrancamos la FSM */	
	start();
		
#if 1
	while(1)
	{

			state_table[next_state]();
		

	}

#endif 
	
}





//================================================
/* Handlers para la gestión de interrupciones */
// -----------------------------------------------


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
void TIM6_DAC_IRQHandler(void){
	

	
	
	/* TIM6 => contador para WE1 */
	hal_tim67_int_disable(&tim6Handle); 					// Deshabilitamos interrupción
	hal_tim67_disable(&tim6Handle);								// Deshabilitamos timer
	hal_tim67_handle_interrupt(&tim6Handle);			// Gestionamos la interrupción
	
	

	/* Si evento UPDATE EVENT */
	if(tim6Handle.int_event == UPDATE_EVENT){
	

		
		if(next_state_exp_WE1 == PRETREATMENT_WE){  	// Si estamos en pretreatment....
			
			contPretreatment_we1--;
			
			if(contPretreatment_we1 == 0){				// Hemos agotado el tiempo del pretratamiento...
				if(real_pret_we1 == tCond){							// Si estamos en t cond pasamos a t dep
					real_pret_we1 = tDep;
					pWE1 = P_NONE;					
				}
				else if(real_pret_we1 == tDep){					// Si estamos en t dep pasamos a t eq
					real_pret_we1 = tEq;	
					pWE1 = P_NONE;
				}
				else if(real_pret_we1 == tEq){					// Si estamos en t eq el pretratamiento ha finalizado 

					pWE1 = P_FINISHED;
				}
				
				hal_tim67_clear_flag(&tim6Handle);				// Borramos la flag de int pendiente
			}
			else{

				hal_tim67_clear_flag(&tim6Handle);							// Borramos la flag de int pendiente
				hal_tim67_int_enable(&tim6Handle);
				hal_tim67_enable(&tim6Handle);	
					
			}
			

					
		}

		else if(next_state_exp_WE1 == MEASURING_WE){		// Si estamos corriendo el experimento...
				

			/* enviamos al DAC otro sample */
			enviar_dato_DAC_we1 = YES;

			/* ¿empezamos a hacer medidas en el ADC ...? */
			if(contAux_we1 == (experimentWE1.nSamplesPer - NMEDIDASWE1)){		// >=
				medidasADC_we1 = YES;
			}
			
			
			/* Contamos el sample lanzado para controlar la finalización del experimento */
			experimentWE1.contSamplesExp++;
			contAux_we1++;												// Contador auxiliar para resolver cuando debemos comenzar a tomar medidas con el ADC
			if(contAux_we1 == experimentWE1.nSamplesPer){			// (>=) Si el contador llega al valor de un período completo lo reseteamos a 0 para el siguiente período
				contAux_we1 = 0;
			}
			
			
			/* ¿Hemos finalizado el experimento? */
			if(experimentWE1.contSamplesExp > experimentWE1.nSamplesExp){			// Hemos leído el último sample del experimento...
				eWE1 = E_FINISHED;
				
				hal_tim67_clear_flag(&tim6Handle);				// Borramos la flag de int pendiente
			}
			else{

				hal_tim67_clear_flag(&tim6Handle);				// Borramos la flag de int pendiente
				hal_tim67_int_enable(&tim6Handle);				// Habilitamos interrupción TIM6
				hal_tim67_enable(&tim6Handle);						// Habilitamos el timer
					
			}

		}
	}
	
	/* Reiniciamos la flag de evento */
	tim6Handle.int_event = NONE_EVENT;
	
	
	

}

	


/**
  * @brief  This function handles TIM7 interrupt request.
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler(){
		
	/* TIM7 => contador para WE2 */
	hal_tim67_int_disable(&tim7Handle); 					// Deshabilitamos interrupción
	hal_tim67_disable(&tim7Handle);								// Deshabilitamos timer
	hal_tim67_handle_interrupt(&tim7Handle);			// Gestionamos la interrupción
	
	

	/* Si evento UPDATE EVENT */
	if(tim7Handle.int_event == UPDATE_EVENT){
	

		
		if(next_state_exp_WE2 == PRETREATMENT_WE){  	// Si estamos en pretreatment....
			
			contPretreatment_we2--;
			
			if(contPretreatment_we2 == 0){				// Hemos agotado el tiempo del pretratamiento...
				if(real_pret_we2 == tCond){							// Si estamos en t cond pasamos a t dep
					real_pret_we2 = tDep;
					pWE2 = P_NONE;					
				}
				else if(real_pret_we2 == tDep){					// Si estamos en t dep pasamos a t eq
					real_pret_we2 = tEq;	
					pWE2 = P_NONE;
				}
				else if(real_pret_we2 == tEq){					// Si estamos en t eq el pretratamiento ha finalizado 
					pWE2 = P_FINISHED;
				}
				
				hal_tim67_clear_flag(&tim7Handle);				// Borramos la flag de int pendiente
			}
			else{

				hal_tim67_clear_flag(&tim7Handle);							// Borramos la flag de int pendiente
				hal_tim67_int_enable(&tim7Handle);
				hal_tim67_enable(&tim7Handle);	
					
			}
			

					
		}

		else if(next_state_exp_WE2 == MEASURING_WE){		// Si estamos corriendo el experimento...
				

			/* enviamos al DAC otro sample */
			enviar_dato_DAC_we2 = YES;

			/* ¿empezamos a hacer medidas en el ADC ...? */
			if(contAux_we2 == (experimentWE2.nSamplesPer - NMEDIDASWE2)){		// >=
				medidasADC_we2 = YES;
			}
			
			
			/* Contamos el sample lanzado para controlar la finalización del experimento */
			experimentWE2.contSamplesExp++;
			contAux_we2++;
			if(contAux_we2 == experimentWE2.nSamplesPer){	// >=
				contAux_we2 = 0;
			}
			
			/* ¿Hemos finalizado el experimento? */
			if(experimentWE2.contSamplesExp > experimentWE2.nSamplesExp){			// Hemos leído el último sample del experimento...
				eWE2 = E_FINISHED;
				
				hal_tim67_clear_flag(&tim7Handle);				// Borramos la flag de int pendiente
			}
			else{

				hal_tim67_clear_flag(&tim7Handle);				// Borramos la flag de int pendiente
				hal_tim67_int_enable(&tim7Handle);				// Habilitamos interrupción TIM6
				hal_tim67_enable(&tim7Handle);						// Habilitamos el timer
					
			}

		}
	}
	
	/* Reiniciamos la flag de evento */
	tim7Handle.int_event = NONE_EVENT;
	
	
	
	
}


//================================================

//================================================
/* Application callbacks */
// -----------------------------------------------

/* UART Callbacks */
/*This callback will be called by the driver when driver finishes the transmission of data */
void app_tx_cmp_callback(void *size)
{

	
}

/*This callback will be called by the driver when the application receives the command */
void app_rx_cmp_callback(void *size)
{
	//we got a command,  parse it 
	parse_cmd(UART_rxBuff);
	
}







//================================================

