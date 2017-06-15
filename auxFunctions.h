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
void 	parse_cmd(uint8_t *cmd);







#endif
