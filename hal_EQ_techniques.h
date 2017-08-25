#ifndef __HAL_EQ_TECHNIQUES_H
#define __HAL_EQ_TECHNIQUES_H

// template para preparar una HAL para la generaci�n de las se�ales de cada t�cnica
// Se establecer�n unas APIs para su utilizaci�n
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h> 		// OJO : esto es de C99
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

/* Enum para selecci�n fondo de escala CEMITEC */
typedef enum {
	/* GALVANOSTAT */
	FS_G_500nA,
	FS_G_500uA,
	FS_G_100mA,
	
	/* BIPOTENTIOSTAT */
	FS_B_5mA,
	FS_B_50uA,
	FS_B_500nA,
	FS_B_5_100_mA
}FS_GALV_BIPOT;

/* Enum para selecci�n filtros en modo POT CEMITEC */
typedef enum{
	BUTTER_250Hz,
	BESSEL_1point2KHz,
	BUTTER_200KHz
}FILTER_POT_SELEC;


	
/************************************************/
/*		Structures for handling input Data				*/
/************************************************/
/* Estructuras para EIS */
// TODO

typedef enum{
	LINEAR		= 0x00,
	LOG				= 0x01,
	DECADE		= 0x02
}freq_step_l;


typedef enum{
	TWO_W		= 0x00,
	THREE_W	= 0x01,
	FOUR_W	= 0x02
}number_wires_l;

typedef enum{
	SINE			= 0x00,
	MULTISINE	= 0x01
}waveType_l;


typedef struct{

	uint8_t OCP_on;
	
	// With frequency scans...
	freq_step_l freqStep;					// Linear, Log or freqs/dec
	// Multisine
	uint8_t noSines;
	uint8_t isScan;
	
	
	number_wires_l noW;
	waveType_l waveType;
	
}eis_exp_config_t;


typedef struct{

	uint16_t DCpotential;
	uint16_t DCstart;
	uint16_t DCstop;
	uint16_t DCstep;
	uint16_t nofSamples;			// n� de muestras a recoger por el equipo
	
	uint16_t amplitude;
	
	//uint16_t maxIntTime;
	//uint16_t minIntCyc;
	
	// Frequency scans
	uint16_t firstFreq;
	uint16_t lastFreq;
	uint16_t nFreq;
	

	
	
}eis_param_values_t;



/* Definici�n estructuras datos recibidas por el USB */

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
	FS_GALV_BIPOT rango;
	uint8_t high_gain;
	uint8_t cell_on;
	uint16_t cell;
	
	eis_exp_config_t eis;
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
	uint16_t tRun;			// DAP, DPA
	uint16_t tPulse1;
	uint16_t tPulse2;
	uint16_t amplitude;
	uint16_t freq;
	uint16_t ACamplitude;	
	
	// MSA
	uint16_t frSize;
	uint16_t cycles;
	uint16_t levels;
	uint16_t samples[100];		// OJO si se cambia esto hay que revisar el tama�o del buffer para poder recibir en un solo frame todos los valores.
	uint16_t potentials[100];
	uint16_t time[100];

	// Potentiometries
	uint16_t iDC;
	uint16_t currents[100];		// MSP
	uint16_t iPulse1;
	uint16_t iPulse2;
	uint16_t iStrip;					// PSA
	uint16_t eEnd;

	eis_param_values_t eis;

}param_t;

typedef struct{
	param_t Init;
	param_t runTime;
	
	uint8_t upwardsStep;		// Flag para control suben o bajan escalones (1 o 0)
	uint8_t flagSube;				// flag para la SCV
	
	// Par�metros para el control de la generaci�n de la waveform
	uint32_t contStep;
	uint32_t contSamplesPer;
	uint32_t contSamplesExp;
	uint16_t contSamplesLUT;
	uint16_t contScansCV;				// CV
	
	uint16_t nSamplesLUT;
	uint32_t nSamplesExp;
	uint32_t nSamplesPer;
	uint32_t nSteps;
	uint32_t nSteps1;
	uint32_t nSteps2;
	uint16_t nSamples1;
	uint16_t nSamples2;
	uint16_t nSamples3;
	uint16_t fSampling;		// fSampling depender� del filtro seleccionado
	
	uint16_t nScan;				// Para la CV. Aqu� guardamos el scan en que nos encontramos. Esto se lo pasamos
												// al PC para que este pueda controlar a qu� scan pertenecen los ptos que le est�n llegando.
	
	// ACV
	uint16_t nSamplesAC;
	uint16_t contSin;
	
	// MSA
	bool msa_second_frame; // Flag para saber si se va a recibir segundo frame con datos de los levels del MSA
	
	// CV
	bool flagPrimeraParte;		// Flag para controlar la parte de la se�al que estamos generando
	bool parteA;
	
	// Variable para selecci�n de filtro en modo POT
	FILTER_POT_SELEC filter;
	
}exp_param_values_t;



/* Estructuras para AMPEROMETRIAS */
// TODO

/* Estructuras para POTENCIOMETRIAS */
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

