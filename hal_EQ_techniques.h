// template para preparar una HAL para la generación de las señales de cada técnica
// Se establecerán unas APIs para su utilización
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
/* Definición estructuras datos recibidas por el USB */
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

// LSV
typedef struct {
	float start;
	float stop;
	float step;
	float sr;
} DF_MEASUREMENT_PAR_LSV;

// SCV
typedef struct {
	float start;
	float stop;
	float step;
	float sr;
	float tHold;
	uint16_t scans;
} DF_MEASUREMENT_PAR_SCV;

// DPV
typedef struct {
	float start;
	float stop;
	float step;
	float ePulse;
	float tPulse;
	float sr;
}DF_MEASUREMENT_PAR_DPV;

// NPV
typedef struct {
	float start;
	float stop;
	float step;
	float tPulse;
	float sr;
}DF_MEASUREMENT_PAR_NPV;

// DNPV
typedef struct {
	float start;
	float stop;
	float step;
	float ePulse;
	float tPulse1;
	float tPulse2;
	float sr;
}DF_MEASUREMENT_PAR_DNPV;

// SWV
typedef struct {
	float start;
	float stop;
	float step;
	float amplitude;
	float freq;
}DF_MEASUREMENT_PAR_SWV;

// ACV | SHACV | FTACV
typedef struct {
	float start;
	float stop;
	float step;
	float ACamplitude;
	float sr;
	float freq;
}DF_MEASUREMENT_PAR_AC;


/* DEFINICIÓN DE DATA FRAMES COMPLETOS */
/* Data frames de VOLTAMETRIAS */
// CV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_CV Measurement;
} DF_CVTypeDef;

// LSV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_LSV Measurement;
}DF_LSVTypeDef;

// SCV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_SCV Measurement;
}DF_SCVTypeDef;

// DPV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_DPV Measurement;
}DF_DPVTypeDef;

// NPV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_NPV Measurement;
}DF_NPVTypeDef;

// DNPV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_DNPV Measurement;
}DF_DNPVTypeDef;

// SWV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_SWV Measurement;
}DF_SWVTypeDef;

// ACV | SHACV | FTACV
typedef struct {
	DF_HEADER Header;
	DF_PRETREATMENT_PAR Pretreatment;
	DF_MEASUREMENT_PAR_AC Measurement;
}DF_ACTypeDef;

/************************************************/
/*					APIs						*/
/************************************************/
void generateDACValues(float* lut, uint16_t* data, uint32_t n);


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
uint32_t generateACVsignal(DF_ACTypeDef* df, float* LUT1, float* LUTcomplete) ;
// TODO

/* Amperometries */
// TODO


/* Potentiometries */
// TODO