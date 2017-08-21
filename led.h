#ifndef __LED_H
#define __LED_H

#include "hal_usart_driver.h"
#include "hal_spi_driver.h"
#include "hal_timer6_driver.h"

#define NMEDIDASWE1	4			// Variable para el número de medidas a tomar en el experimento.
													// Aquí se define como cte, pero debe calcularse en función
													// del número de puntos que va a tener la zona donde midamos.
#define NMEDIDASWE2 3
#define MCU_FREQ		25000000.0
#define NSAMPLESLUT	1024


/* Global variables */

extern uint8_t UART_rxBuff[];
extern uint8_t UART_txBuff[];

extern spi_handle_t SpiHandle;
extern uart_handle_t uartHandle;


/* Macros used for configuring gpios for I/O s */
#define 	GPIOA_PIN_6			6
#define 	GPIOA_PIN_4			4
#define 	GPIOA_PIN_11		11
#define 	GPIOB_PIN_14		14
#define 	GPIOB_PIN_8			8
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
#define 	W1_CH5						GPIOA_PIN_11		
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
#define 	BT_RESET_PIN			GPIOC_PIN_2				// Actualmente no se utiliza
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


/* FSM definitions ----------------------------------------------- */

/* FSM defines */
#define NSTATES			60
#define DEBUG			1

// Estados FSM general
typedef enum {
	CONECT = 0,
	IDLE,
	CAL,
	BIPOT,
	POT,
	GALV,
	EIS,
	PREP_E,
	EXPERIMENT,
	ENDING,
	ERR

}stateType;

// Estados FSMs de cada WE
typedef enum {
	PRETREATMENT_WE,
	MEASURING_WE,
	FS_CH_WE
}state_exp_t;



typedef enum {
	M_NONE = 0x00,
	M_BIPOT = 0x01,
	M_POT = 0x02,
	M_GALV = 0x03,
	M_EIS = 0x04
}mode;


typedef enum {
	I_DEFAULT = 0x00,
	I_SATURA = 0x01,
	I_BELOW_THRESHOLD = 0x02
}status_I_measure;

typedef enum {
	C_NONE = 0x00,
	C_BT = 0x01,
	C_USB = 0x02
}mode_com;

typedef enum {
	L_EMPTY = 0x00,
	L_REFRESHED = 0x01,
	L_FINISHED = 0x02
}lut_status;

typedef enum {
	P_NONE = 0x00,
	P_RUNNING = 0x01,
	P_CANCELLED = 0x02,
	P_FINISHED = 0x03
}status_pretreatment;

typedef enum {
	E_NONE = 0x00,
	E_RUNNING = 0x01,
	E_CANCELLED = 0x02,
	E_FINISHED = 0x03,
	E_ERROR = 0x04
}status_experiment;


typedef enum{
	tCond = 0x00,
	tDep = 0x01,
	tEq = 0x02

}pret_value_t;

typedef enum{
	NO = 0x00,
	YES = 0x01
}flag;

typedef enum{
	NOT_READY = 0x00,
	READY = 0x01
}state_pc;



/* FSM global variables */
extern stateType next_state;
extern status_I_measure status_I_we1;
extern status_I_measure status_I_we2;
extern mode_com communication_mode;
extern status_experiment eWE1, eWE2;
extern lut_status lutwe1A_state;
extern lut_status lutwe1B_state;
extern lut_status lutwe2A_state;
extern lut_status lutwe2B_state;
extern mode df_mode;
extern mode mode_working;
extern status_pretreatment pWE1, pWE2;
extern state_pc pc_ready_to_receive;
extern state_exp_t next_state_we1;
extern state_exp_t next_state_we2;



/* FSM Functions definition */
void start(void);
void connection(void);
void Idle(void);
void bipot(void);
void pot(void);
void galv(void);
void eis(void);
void PrepE(void);
void Pretreatment(void);
void Experiment(void);
void Ending(void);
void Error(void);
void calibration(void);

void pretreatment_we1(void);
void measuring_we1(void);
void FS_change_we1(void);
void pretreatment_we2(void);
void measuring_we2(void);
void FS_change_we2(void);
void subStateMachineWE1(void);
void subStateMachineWE2(void);



/* USART callback function */
void app_tx_cmp_callback(void *size);
void app_rx_cmp_callback(void *size);

#endif 

