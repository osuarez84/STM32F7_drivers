// TODO : WORK IN PROGRESS

#include "hal_EQ_techniques.h"

/************************************************/
/*				Helper functions				*/
/************************************************/

/**
* @brief	Generate the data points between start and stop points
* @param	Start point
* @param	End point
* @param	Step between two consecutive points
* @param	Pointer to an array to save the data
* @retval	No. samples 
*/
static uint32_t generateRamp(float eStart, float eStop, float eStep, float* lut) {

	// TODO : si trabajamos con valores uint16_t directamente
	// es necesario cambiar la función fabsf que devuelve un float
	// a abs, que devuelve un int

	uint32_t i;
	
	uint32_t nSamples = ceil(fabsf(eStart - eStop) / eStep) + 1;				// Calculamos número de puntos...
	lut[0] = eStart;															// Cargamos primer valor en LUT...

	for (i = 1; i < nSamples; i++) {

		if (eStart < eStop) {													// Rampa aumenta...
			lut[i] = eStart + (i * eStep);
		}
		else {																	// Rampa disminuye...
			lut[i] = eStart - (i * eStep);
		}

	}

	return nSamples;


}


/**
* @brief	Concatenates the arrays containing the LUTs
* @param	Pointer to LUT1
* @param	Pointer to LUT2
* @param	Pointer to LUT3
* @param	Pointer to LUT where to put the complete data
* @param	No. samples of the LUT1
* @param	No. samples of the LUT2
* @param	No. samples of the LUT3
* @retval	No. samples of the complete data (1 scan)
*/
static uint32_t concatenateLUTs(float* lut1, float* lut2, float* lut3, float* lutC, uint32_t n1, uint32_t n2, uint32_t n3) {

	uint32_t i;
	
	n1--;																// El último pto de la LUT1 es igual al primero de la LUT2
	n2--;																// por lo que lo desechamos. Lo mismo para LUT2 y LUT3
	
	for (i = 0; i < n1; i++) {									
		lutC[i] = lut1[i];												
	}

	for (i = 0; i < n2; i++) {									
		lutC[i + n1] = lut2[i];
	}

	for (i = 0; i < n3; i++) {
		lutC[i + (n1 + n2)] = lut3[i];
	}

	return (n1 + n2 + n3);

}



/************************************************/
/*					APIs						*/
/************************************************/

void generateCVsignal(DF_CVTypeDef df, float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete) {

	uint32_t nSamples1 = generateRamp(df.Measurement.start, df.Measurement.vtx1, df.Measurement.step, LUT1);
	uint32_t nSamples2 = generateRamp(df.Measurement.vtx1, df.Measurement.vtx2, df.Measurement.step, LUT2);
	uint32_t nSamples3 = generateRamp(df.Measurement.vtx2, df.Measurement.vtx1, df.Measurement.step, LUT3);

	uint32_t lengthLUT = concatenateLUTs(LUT1, LUT2, LUT3, LUTcomplete, nSamples1, nSamples2, nSamples3);


}