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
	
	
}DF_OUTPUT;


/************************************************/
/*		Structures for handling input Data				*/
/************************************************/
/* Definición estructuras datos recibidas por el USB */

typedef struct{
	float tCond;
	float eCond;
	float tDep;
	float eDep;
	float tEq;
	float eEq;	
}pretreat_t;


typedef struct{
	uint8_t bipot;
	uint8_t exp;
	uint16_t rango;
	uint8_t high_gain;
	uint8_t cell_on;
	uint16_t cell;
}exp_config_t;


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
	double tCond;
	double eCond;
	double tDep;
	double eDep;
	double tEq;
	double eEq;
	uint16_t cell;
}DF_PRETREATMENT_PAR;

/* Estructuras para VOLTAMETRIAS */
// CV
typedef struct {
	double start;
	double vtx1;
	double vtx2;
	double step;
	double sr;
	uint16_t scans;
} DF_MEASUREMENT_PAR_CV;

// LSV
typedef struct {
	double start;
	double stop;
	double step;
	double sr;
} DF_MEASUREMENT_PAR_LSV;

// SCV
typedef struct {
	double start;
	double stop;
	double step;
	double sr;
	double tHold;
	uint16_t scans;
} DF_MEASUREMENT_PAR_SCV;

// DPV
typedef struct {
	double start;
	double stop;
	double step;
	double ePulse;
	double tPulse;
	double sr;
	uint32_t realStep;
}DF_MEASUREMENT_PAR_DPV;

// NPV
typedef struct {
	double start;
	double stop;
	double step;
	double tPulse;
	double sr;
}DF_MEASUREMENT_PAR_NPV;

// DNPV
typedef struct {
	double start;
	double stop;
	double step;
	double ePulse;
	double tPulse1;
	double tPulse2;
	double sr;
}DF_MEASUREMENT_PAR_DNPV;

// SWV
typedef struct {
	double start;
	double stop;
	double step;
	double amplitude;
	double freq;
}DF_MEASUREMENT_PAR_SWV;

// ACV | SHACV | FTACV
typedef struct {
	double start;
	double stop;
	double step;
	double ACamplitude;
	double sr;
	double freq;
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

/* Data frames de AMPEROMETRIAS */
// TODO

/* Data frames de POTENCIOMETRIAS */
// TODO

/* Data frames de EIS */
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
	exp_config_t* e, float* lut1, float* lut2);

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


void generateDPVwaveform(DF_DPVTypeDef* df, float* LUT);  // función en pruebas!!


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

