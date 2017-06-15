#ifndef __HAL_TIM67_DRIVER_H
#define __HAL_TIM67_DRIVER_H

// TODO : WORK IN PROGRESS


// template para la creación de una HAL para los timer6/7 del STM32F769
// .h file

/* MCU specific headerfile */
#include "stm32f769xx.h"

/************************************************/
/*		TIM6/7 register bit definitions			*/
/************************************************/

/* CR1 */
#define TIM_REG_CR1_ARPE								((uint32_t) 1 << 7);				// Auto-reload preload enable
#define TIM_ENABLE_AUTO_RELOAD					1
#define TIM_DISABLE_AUTO_RELOAD 				0

#define TIM_REG_CR1_CEN									((uint32_t) 1 << 0);				// Counter enable
#define TIM_COUNTER_ENABLE							1
#define TIM_COUNTER_DISABLE							0

#define TIM_REG_CR1_OPM									((uint32_t) 1 << 3);				// One-pulse mode
#define TIM_OPM_ENABLE									1														// counter stops at next update event (clearing CEN bit)
#define TIM_OPM_DISABLE									0

#define TIM_REG_CR1_UDIS								((uint32_t) 1 << 1)					// Update disable (interrupt UE enable/disable)


/* CR2 */

/* DIER */
#define TIM_REG_DIER_UDE								((uint32_t) 1 << 8);				// Update DMA request enable
#define TIM_DMA_REQ_ENABLE							1
#define TIM_DMA_REQ_DISABLE							0

#define TIM_REG_DIER_UIE								((uint32_t) 1 << 0);				// Update interrupt enable
#define TIM_INT_ENABLE									1
#define TIM_INT_DISABLE									0


/* SR */
// TIM6/TIM7
#define TIM_REG_SR_UI_FLAG							((uint32_t) 1 << 0);				// Update interrupt flag (Set by HW. Cleared by SW)

/* EGR */


/* Macros to enable the clocks for various TIMERs */
#define _HAL_RCC_TIM6_CLK_ENABLE()			(RCC->APB1ENR |= (1 << 4));
#define _HAL_RCC_TIM7_CLK_ENABLE()			(RCC->APB1ENR |= (1 << 5));



/************************************************/
/*		Data structures used by TIM6/7			*/
/************************************************/
typedef enum{
	
	HAL_TIM67_STATE_RESET				= 0x00,					/* Peripheral not yet initialized or disable */
	HAL_TIM67_STATE_READY				= 0x01,					/* Peripheral Initialized and ready for use */
	HAL_TIM67_STATE_BUSY				= 0x02,					/* An internal process is ongoing */
	HAL_TIM67_STATE_TIMEOUT			= 0x03,					/* Timeout state */
	HAL_TIM67_STATE_ERROR				= 0x04					/* An error has ocurred */
}hal_tim67_state_t;


typedef struct{
	
	uint32_t Period;				// 0x0000 - 0xFFFF
	
	uint32_t Prescaler;			// 0x0000 - 0xFFFF
	
	uint32_t CounterMode;		
	
	uint32_t AutoReloadPreload;

}tim67_init_t;

/* Application callbacks typedef */
typedef void (UE_CB_t) (TIM_TypeDef *i, hal_tim67_state_t s);


typedef struct __tim67_handle_t{

	TIM_TypeDef					*Instance;		/* Register base address */
	
	tim67_init_t				Init;					/* Time base required parameters */
	
	hal_tim67_state_t		state;				/* TIM operation state */
	
	UE_CB_t							*ue_cb;				/* Callback for handle the UE interrupt */
	
}tim67_handle_t;





/************************************************/
/*		   		Driver exposed APIs				*/
/************************************************/

void hal_tim67_init(tim67_handle_t* tim_h);

void hal_tim67_int_enable(tim67_handle_t* tim_h);

void hal_tim67_int_disable(tim67_handle_t* tim_h);

void hal_tim67_handle_interrupt(tim67_handle_t* tim_h);

void hal_tim67_enable(tim67_handle_t* handle);

void hal_tim67_disable(tim67_handle_t* handle);

void hal_tim67_clear_flag(tim67_handle_t* handle);

/*
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);
//Non-Blocking mode: Interrupt 
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
*/


#endif

