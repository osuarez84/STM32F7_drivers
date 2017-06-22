// TODO : WORK IN PROGRESS
#include "hal_timer6_driver.h"

// ***************************
// *    HELPER FUNCTIONS
// ***************************

static void hal_tim67_configure_counter_mode(TIM_TypeDef* TIMx, uint32_t counterMode){

	if(counterMode){
		TIMx->CR1 |= TIM_REG_CR1_CEN;
	}
	else{
		TIMx->CR1 &= ~TIM_REG_CR1_CEN;
	}
}


static void hal_tim67_configure_auto_reload_preload(TIM_TypeDef* TIMx, uint32_t autoReload){

	if(autoReload){
		TIMx->CR1 |= TIM_REG_CR1_ARPE;
	}
	else{
		TIM1->CR1 &= ~TIM_REG_CR1_ARPE;
	}
}



static void hal_tim67_configure_auto_reload_value(TIM_TypeDef* TIMx, uint32_t reloadValue){

	TIMx->ARR = (uint16_t)reloadValue;

}



static void hal_tim67_configure_prescaler(TIM_TypeDef* TIMx, uint32_t prescaler){

	TIMx->PSC = (uint16_t)prescaler;

}





// ***************************
// *    APIs 
// ***************************


void hal_tim67_init(tim67_handle_t* tim_h){

	/* Select the Counter Mode */
	hal_tim67_configure_counter_mode(tim_h->Instance, tim_h->Init.CounterMode);
	
	/* Set the auto-reload preload */
	hal_tim67_configure_auto_reload_preload(tim_h->Instance, tim_h->Init.AutoReloadPreload);
	
	/* Set the Auto-reload value */
	hal_tim67_configure_auto_reload_value(tim_h->Instance, tim_h->Init.Period);
	
	/* Set the Prescaler value */
	hal_tim67_configure_prescaler(tim_h->Instance, tim_h->Init.Prescaler);

	/* Enable timer  */
	hal_tim67_enable(tim_h);
	
	// Inicializamos los estados
	tim_h->state = HAL_TIM67_STATE_READY;
}


void hal_tim67_int_enable(tim67_handle_t* tim_h){

	tim_h->Instance->CR1 &= ~TIM_REG_CR1_UDIS;
	tim_h->Instance->DIER |= TIM_REG_DIER_UIE;
	tim_h->state = HAL_TIM67_STATE_BUSY;
	
}

void hal_tim67_int_disable(tim67_handle_t* tim_h){

	tim_h->Instance->CR1 |= TIM_REG_CR1_UDIS;
	tim_h->Instance->DIER &= ~TIM_REG_DIER_UIE;
	tim_h->state = HAL_TIM67_STATE_READY;
}
	
void hal_tim67_handle_interrupt(tim67_handle_t* tim_h){


	// ¿qué interrupción ha saltado?
	
	uint32_t tmp1 = 0, tmp2 = 0;
	
	tmp1 = tim_h->Instance->SR & TIM_REG_SR_UI_FLAG;
	tmp2 = tim_h->Instance->CR1 & TIM_REG_CR1_UDIS;
	
	/* Update Event occurred */
	if((tmp1) && (tmp2)){
		// Utilizamos una callback function en la app para discernir los diferentes casos del update event
		tim_h->int_event = UPDATE_EVENT;
		}
	
}

void hal_tim67_enable(tim67_handle_t* handle){

	handle->Instance->CR1 |= TIM_REG_CR1_CEN;

}

void hal_tim67_disable(tim67_handle_t* handle){

	handle->Instance->CR1 &= ~TIM_REG_CR1_CEN;
}


void hal_tim67_clear_flag(tim67_handle_t* handle){

	handle->Instance->SR &= ~TIM_REG_SR_UI_FLAG;			// Reseteamos la flag de interrupción

}