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

/* CR2 */

/* DIER */

/* SR */

/* EGR */

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


typedef struct __tim67_handle_t{

	TIM_TypeDef				*Instance;		/* Register base address */
	
	tim67_init_t				Init;					/* Time base required parameters */
	
	hal_tim67_state_t		state;				/* TIM operation state */
	
}tim67_handle_t;

/************************************************/
/*		   		Driver exposed APIs				*/
/************************************************/

void hal_tim67_init();

void hal_tim67_irq_handler();

/*
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);
/*Non-Blocking mode: Interrupt 
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
*/


#endif