// TODO : WORK IN PROGRESS
#include "hal_timer6_driver.h"

// ***************************
// *    HELPER FUNCTIONS
// ***************************

static void hal_timer67_configure_counter_mode(TIM_TypeDef* TIMx, uint32_t counterMode){

	if(counterMode){
		TIMx->CR1 |= TIM_REG_CR1_CEN;
	}
	else{
		TIMx->CR1 &= ~TIM_REG_CR1_CEN;
	}
}


static void hal_timer67_configure_auto_reload_preload(TIM_TypeDef* TIMx, uint32_t autoReload){

	if(autoReload){
		TIMx->CR1 |= TIM_REG_CR1_ARPE;
	}
	else{
		TIM1->CR1 &= ~TIM_REG_CR1_ARPE;
	}
}



static void hal_timer67_configure_auto_reload_value(TIM_TypeDef* TIMx, uint32_t reloadValue){

	TIMx->ARR = (uint16_t)reloadValue;

}



static void hal_timer67_configure_prescaler(TIM_TypeDef* TIMx, uint32_t prescaler){

	TIMx->PSC = (uint16_t)prescaler;

}




// ***************************
// *    APIs 
// ***************************


void hal_tim67_init(tim67_handle_t* tim_h){

	/* Select the Counter Mode */
	hal_timer67_configure_counter_mode(tim_h->Instance, tim_h->Init.CounterMode);
	
	/* Set the auto-reload preload */
	hal_timer67_configure_auto_reload_preload(tim_h->Instance, tim_h->Init.AutoReloadPreload);
	
	/* Set the Auto-reload value */
	hal_timer67_configure_auto_reload_value(tim_h->Instance, tim_h->Init.Period);
	
	/* Set the Prescaler value */
	hal_timer67_configure_prescaler(tim_h->Instance, tim_h->Init.Prescaler);

}


