// template para preparar una HAL para la generaci�n de las se�ales de cada t�cnica
// Se establecer�n unas APIs para su utilizaci�n
#include <stdint.h>
#include <math.h>
#include <stdlib.h>


#define		NBITSDAC	16
#define		VREF		5
#define		PI			3.14159

// TODO : WORK IN PROGRESS

/************************************************/
/*					Structures for handling Data				*/
/************************************************/
/* Definici�n estructuras datos recibidas por el USB */
typedef struct {
	char init_1;
	char init_2;
	char init_3;
	uint8_t bipot, working;
} DF_HEADER;


typedef struct {
	uint8_t	exp;
	uint16_t rango;
	uint8_t high_gain;
	uint8_t cell_on;
	float tCond;
	float eCond;
	float tDep;
	float eDep;
	float tEq;
	float eEq;
	uint16_t cell;
}DF_PRETREATMENT_PAR;

/* Estructuras para VOLTAMETRIAS */
// CV
typedef struct {
	float start;
	float vtx1;
	float vtx2;
	float step;
	float sr;
	uint16_t scans;
} DF_MEASUREMENT_PAR_CV;


// DPV
typedef struct {
	float start;
	float stop;
	float step;
	float ePulse;
	float tPulse;
	float sr;
}DF_MEASUREMENT_PAR_DPV;

/* DEFINICI�N DE DATA FRAMES COMPLETOS */
/* Data frames de VOLTAMETRIAS */

// CV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_CV Measurement;
} DF_CVTypeDef;


// DPV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_DPV Measurement;
}DF_DPVTypeDef;


/************************************************/
/*					APIs						*/
/************************************************/
void generateDACValues(float* lut, uint16_t* data, uint32_t n);


/* Pretreatment */
// TODO


/* Voltammetries */
uint32_t generateCVsignal(DF_CVTypeDef* df, float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete);

uint32_t generateDPVsignal(DF_DPVTypeDef* df, float* LUTcomplete);
// TODO

/* Amperometries */
// TODO


/* Potentiometries */
// TODO