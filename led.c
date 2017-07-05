#include <stdint.h>
#include "hal_gpio_driver.h"
#include "hal_spi_driver.h"
#include "hal_usart_driver.h"
#include "hal_EQ_techniques.h "
//#include "hal_timer6_driver.h"
#include "auxFunctions.h"
#include "led.h"




/* PRIVATE VARIABLES --------------------------------------------- */
/* Devices handlers */
spi_handle_t SpiHandle;
uart_handle_t uartHandle;
tim67_handle_t tim6Handle;
tim67_handle_t tim7Handle;

/* master read/write buffers */
// TEST DATA
uint16_t master_write_data[]={ 	0x8000,0xa78d,0xcb3c,0xe78d,0xf9bb,0xffff,0xf9bb,0xe78d,
																0xcb3c,0xa78d,0x8000,0x5872,0x34c3,0x1872,0x644,0x0,
																0x644,0x1872,0x34c3,0x5872,0x8000};
uint8_t DACvalue[2];

uint16_t dataADC[1000];
uint32_t cont = 5;
																
uint8_t data_out[14] = {0};															
uint8_t ack[] = "ACK";
uint8_t end[] = "END";


/* UART buffers */
uint8_t UART_rxBuff[100];
uint8_t UART_txBuff[100];

																
/* LUTs for generating testing waveforms*/
//float LUT1[10001];
//float LUT2[10001];
//float LUT3[10001];
//float LUTcomplete[31000];																
//uint16_t LUTdac[31000];				
												
/* LUTs periódicas para el refresco continuo */												
uint16_t LUTWE1A[20000] = {0};
uint16_t LUTWE1B[20000] = {0};
uint16_t LUTWE2A[20000] = {0};
uint16_t LUTWE2B[20000] = {0};

uint16_t LUTeCond[200] = {0};
uint16_t LUTeDep[200] = {0};
uint16_t LUTeEq[200] = {0};


uint8_t cont_bipot = 0;			// Contador para controlar la recepción de datos en modo bipot

/* Estructuras para guardar datos experimentos */
DF_CVTypeDef DF_CV_we1;
DF_LSVTypeDef DF_LSV_we1;
DF_SCVTypeDef DF_SCV_we1;
DF_DPVTypeDef DF_DPV_we1;			
DF_NPVTypeDef DF_NPV_we1;
DF_DNPVTypeDef DF_DNPV_we1;
DF_SWVTypeDef DF_SWV_we1;
DF_ACTypeDef DF_ACV_we1;

DF_CVTypeDef DF_CV_we2;
DF_LSVTypeDef DF_LSV_we2;
DF_SCVTypeDef DF_SCV_we2;
DF_DPVTypeDef DF_DPV_we2;			
DF_NPVTypeDef DF_NPV_we2;
DF_DNPVTypeDef DF_DNPV_we2;
DF_SWVTypeDef DF_SWV_we2;
DF_ACTypeDef DF_ACV_we2;

pretreat_t pretreat_we1;
pretreat_t pretreat_we2;
exp_config_t exp_config_we1;
exp_config_t exp_config_we2;

/* Variables para guardar el número de samples tanto de
cada LUT/período como del experimento completo. Esto
nos permite llevar el control de si debemos de 
refrescar alguna LUT o si hemos llegado al final
del experimento */
uint32_t nSamplesLUTWE1;
uint32_t nSamplesLUTWE2;
uint32_t nSamplesExpWE1;
uint32_t nSamplesExpWE2;

uint32_t contSamplesLUTWE1 = 0;
uint32_t contSamplesLUTWE2 = 0;
uint32_t contSamplesExpWE1 = 0;
uint32_t contSamplesExpWE2 = 0;

uint16_t contMedidasADC = 0;

/* Estructuras para formar el paquete
de los datos enviados */
DF_OUTPUT df_out_we1;
DF_OUTPUT df_out_we2;

															
/* Variables para el seguimiento de los estados
y eventos que disparan las transiciones */
stateType next_state;
status_I_measure status_I_we1;
status_I_measure status_I_we2;
mode_com communication_mode;
state_experiment experiment;
lut_state lutwe1A_state;
lut_state lutwe1B_state;
lut_state lutwe2A_state;
lut_state lutwe2B_state;
mode df_mode;
mode mode_working;
state_pretreatment pretreatment;
general_state state_equipment;	
state_pc pc_ready_to_receive;

																
enum yesno leerLUTAWE1;
enum yesno leerLUTBWE1;
enum yesno leerLUTAWE2;
enum yesno leerLUTBWE2;
enum yesno medidasADC;
enum yesno enviar_dato_DAC;

/* Testing functions *************************************************************/

void sendSineSPI(){
	
	uint32_t count, c;
	
	
	for (count = 0; count <= 20; count++){

			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
//			addrcmd[0] = (uint8_t) master_write_data[count];
//			addrcmd[1] = (uint8_t) (master_write_data[count] >> 8);
//			hal_spi_master_tx(&SpiHandle, addrcmd, 2);
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
			//addrcmd[0] = (uint8_t) LUTdac[count];
			//addrcmd[1] = (uint8_t) (LUTdac[count] >> 8);
//			hal_spi_master_tx(&SpiHandle, addrcmd, 2);
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
	
		
			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
//			addrcmd[0] = (uint8_t) LUTdac[count];
//			addrcmd[1] = (uint8_t) (LUTdac[count] >> 8);
//			hal_spi_master_tx(&SpiHandle, addrcmd, 2);
			while(SpiHandle.State != HAL_SPI_STATE_READY);
			for(c = 0; c < 3000; c++){}													// MUST : Esperamos 0.1 ms entre cada envío para cumplir Timing del CLK (medido con Saleae)
			hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH 
			for(c = 0; c < 30; c++){}														// MUST : Generamos un CS HIGH de 4.5us para cumplir los requisitos de Timing del CS (medido con Saleae)
//			dataADC = read_ADC_W1();


			
			/* Send data using BT */
			UART_txBuff[0] = (uint8_t) dataADC;
//			UART_txBuff[1] = (uint8_t) (dataADC >> 8);
			while(uartHandle.tx_state != HAL_UART_STATE_READY);
			hal_uart_tx(&uartHandle, UART_txBuff, 2);		
	}

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
	conection,
	Idle,
	calibration,
	bipot,
	pot,
	galv,
	eis,
	PrepE,
	Pretreatment,
	Measuring,
	FS_ch,
	Ending,
	Error
};

void start() {

	// inicializamos todo el equipo paso a paso
	mode_working = M_NONE;
	df_mode = M_NONE;
	status_I_we1 = I_DEFAULT;
	status_I_we2 = I_DEFAULT;
	communication_mode = C_NONE;
	pretreatment = P_NONE;
	lutwe1A_state = L_EMPTY;
	lutwe1B_state = L_EMPTY;
	lutwe2A_state = L_EMPTY;
	lutwe2B_state = L_EMPTY;
	experiment = E_NONE;
	
	pc_ready_to_receive = NOT_READY;

	
	cont_bipot = 2;
	
	next_state = CONECT;

}

void conection() {

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
		load_data(UART_rxBuff, &DF_CV_we2, &DF_LSV_we2, &DF_SCV_we2, &DF_DPV_we2, &DF_NPV_we2,\
											&DF_DNPV_we2, &DF_SWV_we2, &DF_ACV_we2, &pretreat_we2,\
											&exp_config_we2);
	
		// Generamos LUTWE2A y LUTWE2B
		generate_data(&DF_CV_we2, &DF_LSV_we2, &DF_SCV_we2, &DF_DPV_we2, &DF_NPV_we2, &DF_DNPV_we2,\
											&DF_SWV_we2, &DF_ACV_we2, &exp_config_we2, LUTWE2A, &nSamplesExpWE2, &nSamplesLUTWE2);
		generate_data(&DF_CV_we2, &DF_LSV_we2, &DF_SCV_we2, &DF_DPV_we2, &DF_NPV_we2, &DF_DNPV_we2,\
											&DF_SWV_we2, &DF_ACV_we2, &exp_config_we2, LUTWE2B, &nSamplesExpWE2, &nSamplesLUTWE2);		
		
		cont_bipot = 2;									// Reseteamos contador para la siguiente vez...
		mode_working = M_BIPOT;
		next_state = PREP_E;
	}
	else{															// No
		// Cargamos la estructura para el electródo WE1...
		load_data(UART_rxBuff, &DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, \
											&DF_DNPV_we1, &DF_SWV_we1, &DF_ACV_we1, &pretreat_we1,\
											&exp_config_we1);
	
		// Generamos LUTWE1A y LUTWE1B
		generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
											&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE1A, &nSamplesExpWE1, &nSamplesLUTWE1);
		generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
											&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE1B, &nSamplesExpWE1, &nSamplesLUTWE1);
		
		// Volvemos al estado de espera de nuevo...
		df_mode = M_NONE;				
		next_state = IDLE;
	}

}

void pot() {
	

		mode_working = M_POT;
		
		// Cargamos los datos del experimento
		load_data(UART_rxBuff, &DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1,\
												&DF_SWV_we1, &DF_ACV_we1, &pretreat_we1,\
												&exp_config_we1);
		
		// Generamos LUTWE1A y LUTWE1B
		generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
												&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE1A, &nSamplesExpWE1, &nSamplesLUTWE1);
		generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
												&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE1B, &nSamplesExpWE1, &nSamplesLUTWE1);

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



void PrepE() {

	if (pc_ready_to_receive == READY){				// PC está listo para comenzar a recibir datos...
	
		/* MODO POT/BIPOT */
		if (mode_working == M_BIPOT) {
			// Configuramos FS
			// Configuramos filtros
			// Generamos primer refresco LUT1 y LUT2		
			// lut1A_state = REFRESHED y lut2A_state
			lutwe1A_state = L_REFRESHED;
			lutwe1B_state = L_REFRESHED;
			lutwe2A_state = L_REFRESHED;
			lutwe2A_state = L_REFRESHED;
			

			
			// Habilitamos electródos

			/* Hay que preparar dos LUTs, una hará de buffer y se irá cargando mientras se envía la primera */

			// Aplicamos el pretratamiento que proceda

		}
		else if (mode_working == M_POT) {
			// Configuramos FS auto o no
			// Configuramos filtros
			// Generamos primer refresco de LUT
			lutwe1A_state = L_REFRESHED;
			lutwe1B_state = L_REFRESHED;
			
			leerLUTAWE1 = YES;		// comenzamos a leer LUTA
			leerLUTBWE1 = NO;
			enviar_dato_DAC = NO;

			
		
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
		
		next_state = PRETREATMENT;

		
	
	}
	
	else{																		// PC NO está listo para recibir datos... (wait!)
		next_state = PREP_E;	
	}
		
}


void Pretreatment() {
	
	if (pretreatment == P_NONE){
		
		// Lanzame la temporización para el pretreatment
		if(mode_working == M_BIPOT){
			// TODO: configurame la temporización para los dos WE
		
		}
		
		else if (mode_working == M_POT){
			
			// Configuramos TIM6 para DAC WE1
			//hal_tim67_int_enable(&tim6Handle);
			//hal_tim67_init(&tim6Handle);

		
		}
		else if (mode_working == M_GALV){
			// TODO
			// Configurar TIM6 para DAC WE1
		
		}

		else if(mode_working == M_EIS){
			// TODO
			
		
		}
		
		
		pretreatment = P_RUNNING;
	}
	
	else if(pretreatment == P_RUNNING){
		// Esperamos a que termine el pretreatment
		
		
		
		#if DEBUG												// Si en pruebas nos saltamos el pretreatment
			pretreatment = 	P_FINISHED;									 
		#endif
	
	}
	
	else if (pretreatment == P_FINISHED) {
		// Esperamos a que el pretratamiento termine
		if (mode_working == M_BIPOT) {
			// Enviamos LUT1A a DAC WE1
			// Enviamos LUT2A a DAC WE2

			// Comenzamos siguiente refresco LUT1B
			// Comenzamos siguiente refresco LUT2B

		}
		else if (mode_working == M_POT) {
			// Generamos LUT1A y LUT1B según la técnica

			// Comenzamos siguiente refresco LUT1B
			
			

		}
		else if (mode_working == M_GALV) {
			// Enviamos LUT1A a DAC WE1

			// Comenzamos siguiente refresco LUT1B
		}
		else if (mode_working == M_EIS) {
			// TODO

		}

		next_state = MEASURING;
	}

}


void Measuring() {


	if(experiment == E_NONE){
		
		// Ponemos YA el experimento en running antes de habilitar interrupciones, para que las haga correctamente
		experiment = E_RUNNING;	
		
		
		if(mode_working == M_POT){
			// Arrancamos temporización DAC WE1 
			
			// Configuramos TIM6 para DAC WE1
			hal_tim67_int_enable(&tim6Handle);
			hal_tim67_init(&tim6Handle);
		
		}
		
		else if(mode_working == M_BIPOT){
			// Arrancamos temporización DAC WE1
			// Arrancamos temporización DAC WE2
			
		}
		
		else if(mode_working == M_GALV){
		// TODO
		
		}
		
		else if(mode_working == M_EIS){
		// TODO
		
		
		}

		

	
	}
	
	else if(experiment == E_RUNNING){
		
		// En la ISR pondremos experiment = E_FINISHED o E_CANCELLED cuando corresponda
		
		
		if (mode_working == M_POT) {
			
			/* Enviar datos a DAC? */
			if (enviar_dato_DAC == YES){
				if(leerLUTAWE1 == YES){							// Leemos de LUTA...
					// Pasamos el sample al DAC
					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
					DACvalue[0] = (uint8_t) LUTWE1A[contSamplesLUTWE1];
					DACvalue[1] = (uint8_t) (LUTWE1A[contSamplesLUTWE1] >> 8);
					hal_spi_master_tx(&SpiHandle, DACvalue, 2);
					while(SpiHandle.State != HAL_SPI_STATE_READY);
					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH										
					
					contSamplesLUTWE1++;
					
					// ¿Hay que cambiar de LUT...?
					if(contSamplesLUTWE1 == nSamplesLUTWE1){
						leerLUTAWE1 = NO;								// Cambiamos a LUTB
						leerLUTBWE1 = YES;
						lutwe1A_state = L_FINISHED;			// Avisamos a la FSM para que refresque...
						contSamplesLUTWE1 = 0;					// Reiniciamos contador
					
					}

				}		
				else if(leerLUTBWE1 == YES){				// Leemos de LUTB
					// Pasamos el sample al DAC
					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 0);				// CS to LOW
					DACvalue[0] = (uint8_t) LUTWE1A[contSamplesLUTWE1];
					DACvalue[1] = (uint8_t) (LUTWE1A[contSamplesLUTWE1] >> 8);
					hal_spi_master_tx(&SpiHandle, DACvalue, 2);
					while(SpiHandle.State != HAL_SPI_STATE_READY);
					hal_gpio_write_to_pin(GPIOB, SPI_CS_PIN, 1);				// CS to HIGH		

					contSamplesLUTWE1++;
					
					// ¿Hay que cambiar de LUT...?
					if(contSamplesLUTWE1 == nSamplesLUTWE1){
						leerLUTBWE1 = NO;								// Cambiamos a LUTA
						leerLUTAWE1 = YES;
						lutwe1B_state = L_FINISHED;			// Avisamos a la FSM para que refresque...
						contSamplesLUTWE1 = 0;					// Reiniciamos contador
					}
				
				}
				enviar_dato_DAC = NO;
			
			}

			/* Hay que tomar medidas en el ADC ? */
			if(medidasADC == YES){
				// Tomamos medida
				dataADC[contMedidasADC] = read_ADC_W1();
				
				contMedidasADC++;

				if(contMedidasADC == NMEDIDAS){				// ¿Paramos de tomar medidas...?
					// Hacemos medida y enviamos datos
					// TODO
					//calculate media(dataADC);

					
					// Montamos el paquete de datos
					data_out[0] = 'D';
					data_out[1] = 'A';
					data_out[2] = 'T';
					data_out[3] = 0;
					data_out[4] = rand();
					data_out[5] = rand();
					data_out[6] = rand();
					data_out[7] = rand();
					data_out[8] = rand();
					data_out[9] = rand();
					data_out[10] = 0;
					data_out[11] = 0;
					data_out[12] = 0;
					data_out[13] = 'F';

					// Enviamos los datos
					while(uartHandle.tx_state != HAL_UART_STATE_READY){};
					hal_uart_tx(&uartHandle, data_out, sizeof(data_out));
					
					//enviar = YES;
					medidasADC = NO;
					contMedidasADC = 0;
				}
			}
			
			
			
			
			/* Comprobamos si debemos de refrescar alguna LUT */
			if (lutwe1A_state == L_FINISHED) {
				// Refrescamos la LUT
				generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
											&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE1A, &nSamplesExpWE1, &nSamplesLUTWE1);
				lutwe1A_state = L_REFRESHED;
			}
			else if (lutwe1B_state == L_FINISHED) {
				// Refrescamos la LUT
				generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
											&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE1B, &nSamplesExpWE1, &nSamplesLUTWE1);
				lutwe1B_state = L_REFRESHED;
			}


			/* Comprobamos si debemos cambiar el FS */
			if (status_I_we1 != I_DEFAULT) {
				next_state = FS_CH;
			}

			
			
		}
		else if (mode_working == M_BIPOT) {
			if (lutwe1A_state == L_FINISHED) {
				// Refrescamos la LUTWE1A
				generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
											&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE1A, &nSamplesExpWE1, &nSamplesLUTWE1);
				lutwe1A_state = L_REFRESHED;
			}
			else if (lutwe1B_state == L_FINISHED) {
				// Refrescamos la LUTWE1B
				generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
											&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE1B, &nSamplesExpWE1, &nSamplesLUTWE1);
				lutwe2A_state = L_REFRESHED;
			}
			if (lutwe2A_state == L_FINISHED) {
				// Refrescamos la LUTWE2A
				generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
											&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE2A, &nSamplesExpWE2, &nSamplesLUTWE2);
				lutwe1B_state = L_REFRESHED;
			}
			else if (lutwe2B_state == L_FINISHED) {
				// Refrescamos la LUTWE2B
				generate_data(&DF_CV_we1, &DF_LSV_we1, &DF_SCV_we1, &DF_DPV_we1, &DF_NPV_we1, &DF_DNPV_we1, \
											&DF_SWV_we1, &DF_ACV_we1, &exp_config_we1, LUTWE2B, &nSamplesExpWE2, &nSamplesLUTWE2);
				lutwe2B_state = L_REFRESHED;
			}

			if ((status_I_we1 != I_DEFAULT) | (status_I_we2 != I_DEFAULT)) {
				next_state = FS_CH;
			}
		}
		else if (mode_working == M_GALV) {
			/* Comprobamos si debemos de refrescar alguna LUT */
			if (lutwe1A_state == L_FINISHED) {
				// Refrescamos la LUT
				lutwe1A_state = L_REFRESHED;
			}
			else if (lutwe1B_state == L_FINISHED) {
				// Refrescamos la LUT
				lutwe2A_state = L_REFRESHED;
			}
			if (lutwe1B_state == L_FINISHED) {
				// Refrescamos la LUT
				lutwe1B_state = L_REFRESHED;
			}

			/* Comprobamos si debemos cambiar el FS */
			if (status_I_we1 != I_DEFAULT) {
				next_state = FS_CH;
			}
		}
		else if (mode_working == M_EIS) {
			// TODO
		}
	}
	



	else if ((experiment == E_FINISHED) | (experiment == E_CANCELLED)) {
		
		if(experiment == E_FINISHED){
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
			

		}
		

		
		else if(experiment == E_CANCELLED){
			
			// Montamos trama datos finalización por cancelación (mensaje END)
			while(uartHandle.tx_state != HAL_UART_STATE_READY){};
			hal_uart_tx(&uartHandle, end, sizeof(end)-1);
			
			
			// Deshabilitamos todas temporizaciones
			// WE1

			hal_tim67_int_disable(&tim6Handle); 					// Deshabilitamos interrupción
			hal_tim67_disable(&tim6Handle);								// Deshabilitamos timer
			hal_tim67_clear_flag(&tim6Handle);						// Borramos la flag de int pendiente
			
			
			// WE2
			// TODO
	
	
		}
		

		// Reseteamos contadores
		contSamplesExpWE1 = 0;
		contSamplesLUTWE1 = 0;
		//contSamplesExpWE2 = 0;
		//contSamplesLUTWE2 = 0;
		
		// Habilitamos mensajes de CON de nuevo
		while(uartHandle.rx_state != HAL_UART_STATE_READY){};		
		hal_uart_rx(&uartHandle, UART_rxBuff, 39);
		
		next_state = ENDING;
		
	
		// switch off USER LED testing
		led_turn_off(GPIOJ, LED_GREEN);
		
		
	}
	

}


// TODO: comprobamos si se ha habilitado MAS de un FS. En caso de que haya solamente uno, no debemos entrar en este estado.
void FS_ch() {

	if (mode_working == M_POT) {
		if (status_I_we1 == I_SATURA) {
			// Cambiamos FS WE1 a un FS mayor
			// Reseteamos FLAG_SATURA_WE1 => flag_satura_we1 = DEFAULT;
		}
		else if (status_I_we1 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE1 a una FS menor
			// Reseteamos status_I_we1 => status_I_we1 = DEFAULT;

		}

		status_I_we1 = I_DEFAULT;		// Reseteamos el flag

	}
	else if (mode_working == M_BIPOT) {
		if (status_I_we1 == I_SATURA) {
			// Cambiamos FS WE1 a un FS mayor
			// Reseteamos la status_I_we1 => status_I_we1 = DEFAULT;
		}
		else if (status_I_we1 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE1 a un FS menor
			// Reseteamos la status_I_we1 => status_I_we1 = DEFAULT;
		}
		if (status_I_we2 == I_SATURA) {
			// Cambiamos FS WE2 a un FS mayor
			// Reseteamos la status_I_we2 => status_I_we2 = DEFAULT;
		}
		else if (status_I_we2 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE2 a un FS menor
			// Reseteamos la status_I_we2 => status_I_we2 = DEFAULT;
		}
		status_I_we1 = I_DEFAULT;		// Reseteamos los flags
		status_I_we2 = I_DEFAULT;

	}

	else if (mode_working == M_GALV) {
		if (status_I_we1 == I_SATURA) {
			// Cambiamos FS WE1 a un FS mayor
			// Reseteamos FLAG_SATURA_WE1 => flag_satura_we1 = DEFAULT;
		}
		else if (status_I_we1 == I_BELOW_THRESHOLD) {
			// Cambiamos FS WE1 a una FS menor
			// Reseteamos status_I_we1 => status_I_we1 = DEFAULT;

		}

		status_I_we1 = I_DEFAULT;		// Reseteamos el flag
	}

	else if (mode_working == M_EIS) {
		// TODO
	}

	next_state = MEASURING;



}


// TODO: Hay una opción para dejar el electródo conectado a un determinado potencial al finalizar el experimento. TENER EN CUENTA.
void Ending() {

	// Desconectar electródos
	// Resetear los demás periféricos a default
	// Enviar mensajes de finalización al software
	if (state_equipment != S_ERROR) {
		// borramos todos los flags al salir para dejarlos por defecto => default y NO_SATURA
		mode_working = M_NONE;
		df_mode = M_NONE;
		status_I_we1 = I_DEFAULT;
		status_I_we2 = I_DEFAULT;
		//communication_mode = C_NONE;
		pretreatment = P_NONE;
		lutwe1A_state = L_EMPTY;
		lutwe1B_state = L_EMPTY;
		lutwe2A_state = L_EMPTY;
		lutwe2B_state = L_EMPTY;
		experiment = E_NONE;
		
		pc_ready_to_receive = NOT_READY;
		
		next_state = IDLE;
	}
	else if (state_equipment == S_ERROR) {

		next_state = ERR;
	}

}

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
	/* enable clock for TIM6 */
	_HAL_RCC_TIM6_CLK_ENABLE();
	
	tim6Handle.Instance = TIM6;
	
	tim6Handle.Init.CounterMode = TIM_OPM_ENABLE;
	tim6Handle.Init.Period = 5;
	tim6Handle.Init.Prescaler = 512;
	tim6Handle.Init.AutoReloadPreload = TIM_ENABLE_AUTO_RELOAD;
	
	/* initialize the event flag */
	tim6Handle.int_event = NONE_EVENT;
	
	/* enable the IRQ of TIM6 peripheral */
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	
	
	// Establecemos prioridades de las interrupciones:
	//	=> la comunicación tiene prioridad sobre los datos que se envían al DAC
	//	y los que se envían al ADC
	NVIC_SetPriorityGrouping(4);

	NVIC_SetPriority(USART6_IRQn, 0);
	NVIC_SetPriority(TIM6_DAC_IRQn, 1);
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
void TIM6_DAC_IRQHandler(void){

	
	/* TIM6 => contador para WE1 */
	hal_tim67_int_disable(&tim6Handle); 					// Deshabilitamos interrupción
	hal_tim67_disable(&tim6Handle);								// Deshabilitamos timer
	hal_tim67_handle_interrupt(&tim6Handle);			// Gestionamos la interrupción
	
	

	/* Si evento UPDATE EVENT */
	if(tim6Handle.int_event == UPDATE_EVENT){
	
		// TODO : copiar el pretratamiento de la estructura hecha en el else if del experiment (más abajo)
		// Si estamos en pretreatment y ha finalizado el tiempo de pretratamiento...
		if(next_state == PRETREATMENT && pretreatment == P_RUNNING){  	
				
			if(cont == 0){				// Hemos leído el último sample del experimento...
				pretreatment = P_FINISHED;
			}
			else{

				hal_tim67_clear_flag(&tim6Handle);							// Borramos la flag de int pendiente
				hal_tim67_int_enable(&tim6Handle);
				hal_tim67_enable(&tim6Handle);	
					
				// Testing
				led_toggle(GPIOJ, LED_GREEN);
			}
					
		}

		else if(next_state == MEASURING && experiment == E_RUNNING){		// Si estamos corriendo el experimento...
				

			/* enviamos al DAC otro sample */
			enviar_dato_DAC = YES;

			/* ¿empezamos a hacer medidas en el ADC ...? */
			if(contSamplesLUTWE1 >= (nSamplesLUTWE1 - NMEDIDAS)){
				medidasADC = YES;
			}
			
			
			/* Contamos el sample lanzado para controlar la finalización del experimento */
			contSamplesExpWE1++;
			
			/* ¿Hemos finalizado el experimento? */
			if(contSamplesExpWE1 > nSamplesExpWE1){			// Hemos leído el último sample del experimento...
				experiment = E_FINISHED;
				
				hal_tim67_clear_flag(&tim6Handle);				// Borramos la flag de int pendiente
			}
			else{

				hal_tim67_clear_flag(&tim6Handle);				// Borramos la flag de int pendiente
				hal_tim67_int_enable(&tim6Handle);				// Habilitamos interrupción TIM6
				hal_tim67_enable(&tim6Handle);						// Habilitamos el timer
					
				// Testing
				led_toggle(GPIOJ, LED_GREEN);
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

