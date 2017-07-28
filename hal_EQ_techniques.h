#ifndef __HAL_EQ_TECHNIQUES_H
#define __HAL_EQ_TECHNIQUES_H

// template para preparar una HAL para la generación de las señales de cada técnica
// Se establecerán unas APIs para su utilización
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "led.h"


#define		NBITSDAC	16
#define		VREF		5
#define		PI			3.14159


/************************************************/
/*		Structures for handling output Data				*/
/************************************************/
//typedef struct {
//	char init_1;
//	char init_2;
//	char init_3;
//	
//	workingE w;
//	uint8_t nCiclos;					// nos dice el ciclo en el que estamos, para distinguir los datos entre ciclos
//														// Se utiliza en multistep y CV
//	uint8_t valor_y1_l;
//	uint8_t valor_y1_h;
//	
//	uint8_t valor_y2_l;
//	uint8_t valor_y2_h;
//	
//	uint8_t valor_x_l;
//	uint8_t valor_x_h;
//	
//	FS_state fs;
//	
//	uint8_t dummy1;
//	uint8_t dummy2;
//	uint8_t dummy3;
//	char dummy4;
//	
//	
//}DF_OUTPUT;


/************************************************/
/*		Structures for handling input Data				*/
/************************************************/
/* Definición estructuras datos recibidas por el USB */

typedef struct{
	uint16_t tCond;
	uint16_t eCond;
	uint16_t tDep;
	uint16_t eDep;
	uint16_t tEq;
	uint16_t eEq;	
}pretreat_param_t;


typedef struct{
	uint8_t bipot;
	uint8_t exp;
	uint16_t rango;
	uint8_t high_gain;
	uint8_t cell_on;
	uint16_t cell;
}exp_config_t;

typedef struct{
	uint16_t start;
	uint16_t eDC;
	uint16_t stop;
	uint16_t step;
	uint16_t sr;
	uint16_t scans;
	uint16_t vtx1;
	uint16_t vtx2;
	
	uint16_t tHold;
	uint16_t ePulse1;
	uint16_t ePulse2;			// DSCA
	float tInterval;		// DSCA
	uint16_t tPulse1;
	uint16_t tPulse2;
	uint16_t amplitude;
	uint16_t freq;
	uint16_t ACamplitude;	
}param_t;

typedef struct{
	param_t Init;
	param_t runTime;
	
	uint8_t upwardsStep;		// Flag para control suben o bajan escalones (1 o 0)
	uint8_t flagSube;				// flag para la SCV
	
	// Parámetros para el control de la generación de la waveform
	uint32_t contStep;
	uint32_t contSamplesPer;
	uint32_t contSamplesExp;
	uint16_t contSamplesLUT;
	
	uint16_t nSamplesLUT;
	uint32_t nSamplesExp;
	uint32_t nSamplesPer;
	uint32_t nSteps;
	uint16_t nSamples1;
	uint16_t nSamples2;
	uint16_t nSamples3;
	uint16_t fSampling;		// fSampling dependerá del filtro seleccionado
	
	// ACV
	uint16_t nSamplesAC;
	uint16_t contSin;
	
	
}exp_param_values_t;



/* Estructuras para AMPEROMETRIAS */
// TODO

/* Estructuras para POTENCIOMETRIAS */
// TODO

/* Estructuras para EIS */
// TODO


/************************************************/
/*											APIs										*/
/************************************************/
void load_data(uint8_t* buff, exp_param_values_t* e, pretreat_param_t* p, exp_config_t* eConfig);
void generate_data(exp_param_values_t* e,	exp_config_t* eConfig, uint16_t* lut);



/* Amperometries */
// TODO


/* Potentiometries */
// TODO

#endif

