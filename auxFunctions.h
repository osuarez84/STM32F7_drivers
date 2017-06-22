#ifndef __AUX_FUNCTIONS_H
#define __AUX_FUNCTIONS_H



/* MCU specific headerfile */
#include "stm32f769xx.h"


/**
	* @brief  Reads the W1 ADC value
	* @param  None
	* @retval None
	*/
uint16_t read_ADC_W1(void);


/**
* @brief  This function parses the command and takes action
* @param  *cmd :
* @retval None
*/
void 	parse_cmd(uint8_t* cmd, DF_CVTypeDef* df_cv, DF_LSVTypeDef* df_lsv, DF_SCVTypeDef* df_scv, \
	DF_DPVTypeDef* df_dpv, DF_NPVTypeDef* df_npv, DF_DNPVTypeDef* df_dnpv, DF_SWVTypeDef* df_swv, DF_ACTypeDef* df_acv, \
	float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete);


/* Functions to control the user LEDs in the discovery board */
void led_turn_on(GPIO_TypeDef *GPIOx, uint16_t pin);
void led_turn_off(GPIO_TypeDef *GPIOx, uint16_t pin);
void led_toggle(GPIO_TypeDef *GPIOx, uint16_t pin);




#endif
