#ifndef __HAL_TIM6_DRIVER_H
#define __HAL_TIM6_DRIVER_H

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

typedef struct{
	
	uint32_t Period;			// Period : 0 - 65535, 16 bits
	
	uint32_t Prescaler;			// 2 - 65536
	
	uint8_t CounterMode;
	

}tim_init_t;


typedef struct __tim_handle_t{

}tim_handle_t;

/************************************************/
/*		   		Driver exposed APIs				*/
/************************************************/

void hal_tim_init();

void hal_tim_irq_handler();





#endif