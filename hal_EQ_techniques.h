#ifndef __HAL_EQ_TECHNIQUES_H
#define __HAL_EQ_TECHNIQUES_H

// template para preparar una HAL para la generación de las señales de cada técnica
// Se establecerán unas APIs para su utilización
#include <stdint.h>
#include <math.h>
#include <stdlib.h>


#define		NBITSDAC	16
#define		VREF		5
#define		PI			3.14159

// TODO : WORK IN PROGRESS
typedef enum {
	WE_1,
	WE_2
}workingE;

typedef enum{

	/* FS para corriente */
	FS_1,
	FS_2,
	FS_3,
	FS_4,
	FS_5

}FS_state;

/************************************************/
/*		Structures for handling output Data				*/
/************************************************/
typedef struct {
	char init_1;
	char init_2;
	char init_3;
	
	workingE w;
	uint8_t nCiclos;					// nos dice el ciclo en el que estamos, para distinguir los datos entre ciclos
														// Se utiliza en multistep y CV
	uint8_t valor_y1_l;
	uint8_t valor_y1_h;
	
	uint8_t valor_y2_l;
	uint8_t valor_y2_h;
	
	uint8_t valor_x_l;
	uint8_t valor_x_h;
	
	FS_state fs;
	
	uint8_t dummy1;
	uint8_t dummy2;
	uint8_t dummy3;
	char dummy4;
	
	
}DF_OUTPUT;


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
}pretreat_t;


typedef struct{
	uint8_t bipot;
	uint8_t exp;
	uint16_t rango;
	uint8_t high_gain;
	uint8_t cell_on;
	uint16_t cell;
}exp_config_t;



/* Estructuras para VOLTAMETRIAS */
// CV
typedef struct {
	uint16_t start;
	uint16_t vtx1;
	uint16_t vtx2;
	uint16_t step;
	uint16_t sr;
	uint16_t scans;
} DF_CVTypeDef;

// LSV
typedef struct {
	uint16_t start;
	uint16_t stop;
	uint16_t step;
	uint16_t sr;
} DF_LSVTypeDef;

// SCV
typedef struct {
	uint16_t start;
	uint16_t stop;
	uint16_t step;
	uint16_t sr;
	uint16_t tHold;
	uint16_t scans;
} DF_SCVTypeDef;

// DPV												// Estructura en testeo
typedef struct{
	uint16_t start;
	uint16_t stop;
	uint16_t step;
	uint16_t ePulse;
	uint16_t tPulse;
	uint16_t sr;

}DF_DPV_Init_t;

typedef struct {
	DF_DPV_Init_t Init;
	uint16_t start;
	uint16_t stop;
	uint16_t step;
	uint16_t ePulse;
	uint16_t tPulse;
	uint16_t sr;
	uint32_t realStep;					// Parámetro en testeo
	uint32_t contSamplesLUT;		// Parámetro en testeo
	uint32_t nSamplesLUT;				// Parámetro en testeo
	uint32_t nSteps;						// Parámetro en testeo
	uint32_t nSamples1;						// Parámetro en testeo
	uint32_t nSamples2;						// Parámetro en testeo
	uint32_t tSampling;						// Parámetro en testeo					
} DF_DPVTypeDef;

// NPV
typedef struct {
	uint16_t start;
	uint16_t stop;
	uint16_t		step;
	uint16_t tPulse;
	uint16_t sr;
} DF_NPVTypeDef;

// DNPV
typedef struct {
	uint16_t start;
	uint16_t stop;
	uint16_t step;
	uint16_t ePulse;
	uint16_t tPulse1;
	uint16_t tPulse2;
	uint16_t sr;
} DF_DNPVTypeDef;

// SWV
typedef struct {
	uint16_t start;
	uint16_t stop;
	uint16_t step;
	uint16_t amplitude;
	uint16_t freq;
} DF_SWVTypeDef;

// ACV | SHACV | FTACV
typedef struct {
	uint16_t start;
	uint16_t stop;
	uint16_t step;
	uint16_t ACamplitude;
	uint16_t sr;
	uint16_t freq;
}DF_ACTypeDef;


/* Estructuras para AMPEROMETRIAS */
// TODO

/* Estructuras para POTENCIOMETRIAS */
// TODO

/* Estructuras para EIS */
// TODO


/************************************************/
/*					APIs						*/
/************************************************/
void generateDACValues(float* lut, uint16_t* data, uint32_t n);


void load_data(uint8_t* buff, DF_CVTypeDef* df_cv, DF_LSVTypeDef* df_lsv, DF_SCVTypeDef* df_scv, \
	DF_DPVTypeDef* df_dpv, DF_NPVTypeDef* df_npv, DF_DNPVTypeDef* df_dnpv, DF_SWVTypeDef* df_swv, DF_ACTypeDef* df_acv, pretreat_t* p,\
	exp_config_t* e);
void generate_data(DF_CVTypeDef* df_cv, DF_LSVTypeDef* df_lsv, DF_SCVTypeDef* df_scv, \
	DF_DPVTypeDef* df_dpv, DF_NPVTypeDef* df_npv, DF_DNPVTypeDef* df_dnpv, DF_SWVTypeDef* df_swv, DF_ACTypeDef* df_acv,\
	exp_config_t* e, uint16_t* lut, uint32_t* nSamplesExp, uint32_t* nSamplesLUT);

/* Pretreatment */
// TODO


/* Voltammetries */
uint32_t generateCVsignal(DF_CVTypeDef* df, float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete);
uint32_t generateLSVsignal(DF_LSVTypeDef* df, float* LUTcomplete);
uint32_t generateSCVsignal(DF_SCVTypeDef* df, float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete);
uint32_t generateDPVsignal(DF_DPVTypeDef* df, float* LUTcomplete);
uint32_t generateNPVsignal(DF_NPVTypeDef* df, float* LUTcomplete);
uint32_t generateDNPVsignal(DF_DNPVTypeDef* df, float* LUTcomplete);
uint32_t generateSWVsignal(DF_SWVTypeDef* df, float* LUT1, float* LUT2, float* LUTcomplete);
uint32_t generateACVsignal(DF_ACTypeDef* df, float* LUT1, float* LUTcomplete);


void generateDPVwaveform(DF_DPVTypeDef* df, uint16_t* LUT, uint32_t* nSExp, uint32_t* nSLUT);  // función en pruebas!!


void load_CV_data(DF_CVTypeDef* df, uint8_t* cmd);
void load_LSV_data(DF_LSVTypeDef* df, uint8_t* cmd);
void load_SCV_data(DF_SCVTypeDef* df, uint8_t* cmd);
void load_DPV_data(DF_DPVTypeDef* df, uint8_t* cmd);
void load_NPV_data(DF_NPVTypeDef* df, uint8_t* cmd);
void load_DNPV_data(DF_DNPVTypeDef* df, uint8_t* cmd);
void load_SWV_data(DF_SWVTypeDef* df, uint8_t* cmd);
void load_ACV_data(DF_ACTypeDef* df, uint8_t* cmd);



// TODO

/* Amperometries */
// TODO


/* Potentiometries */
// TODO

#endif

