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


/**
* @brief	Computes the DAC value from the voltage value 
* @param	Pointer to LUT with output voltage data
* @param	Pointer to an array to save the DAC LUT
* @param	No. samples of LUT with the output voltage data
* @retval	None
*/
void generateDACValues(float* lut, uint16_t* data, uint32_t n) {

	uint32_t i;
	
	/* Rutina para calcular la potencia de 2^NBITSDAC (para evitar el uso de pow())*/
	uint32_t DACp = 2;		// base
	for (i = 0; i < (NBITSDAC - 1); i++) {
		DACp *= 2;
	}

	for (i = 0; i <= n; i++) {
		data[i] = (( (DACp + 1) * lut[i]) / VREF);
	}

}


/************************************************/
/*					APIs						*/
/************************************************/
/**
* @brief	Generates CV waveform for 1 complete scan
* @param	Pointer to data structure
* @param	Pointer to an auxiliar array
* @param	Pointer to an auxiliar array
* @param	Pointer to an auxiliar array
* @param	Pointer to an array to sace complete LUT
* @retval	Lenght of the LUT
*/
uint32_t generateCVsignal(DF_CVTypeDef* df, float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete) {

	uint32_t nSamples1 = generateRamp(df->Measurement.start, df->Measurement.vtx1, df->Measurement.step, LUT1);
	uint32_t nSamples2 = generateRamp(df->Measurement.vtx1, df->Measurement.vtx2, df->Measurement.step, LUT2);
	uint32_t nSamples3 = generateRamp(df->Measurement.vtx2, df->Measurement.vtx1, df->Measurement.step, LUT3);

	uint32_t lengthLUT = concatenateLUTs(LUT1, LUT2, LUT3, LUTcomplete, nSamples1, nSamples2, nSamples3);

	return lengthLUT;
}

/**
* @brief	Generates LSV waveform for 1 complete scan
* @param	Pointer to data structure
* @param	Pointer to an array to sace complete LUT
* @retval	Lenght of the LUT
*/
uint32_t generateLSVsignal(DF_LSVTypeDef* df, float* LUTcomplete) {
	
	uint32_t lengthLUT = generateRamp(df->Measurement.start, df->Measurement.stop, df->Measurement.step, LUTcomplete);

	/* Escribimos el Estop en el último valor de la LUT */
	LUTcomplete[lengthLUT-1] = df->Measurement.stop;

	return lengthLUT;
}


/**
* @brief	Generates SCV waveform for 1 complete scan
* @param	Pointer to data structure
* @param	Pointer to an auxiliar array
* @param	Pointer to an auxiliar array
* @param	Pointer to an auxiliar array
* @param	Pointer to an array to sace complete LUT
* @retval	Lenght of the LUT
*/
uint32_t generateSCVsignal(DF_SCVTypeDef* df, float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete) {

	uint32_t lengthLUT;
	uint32_t nSamples1 = generateRamp(df->Measurement.start, df->Measurement.stop, df->Measurement.step, LUT1);
	uint32_t nSamples2 = generateRamp(df->Measurement.stop, df->Measurement.start, df->Measurement.step, LUT2);

	/* Enviamos el último punto del período completo, que es el start */
	// Usamos la misma función concatenateLUTs, pero como en este caso son dos tramos hacemos
	// este truco y enviamos el último punto en la tercera LUT. Así no necesitamos definir
	// otra función.
	uint32_t nSamples3 = 1;
	LUT3[0] = df->Measurement.start;

	lengthLUT = concatenateLUTs(LUT1, LUT2, LUT3, LUTcomplete, nSamples1, nSamples2, nSamples3);

	return lengthLUT;

}


/**
* @brief	Generates DPV waveform for 1 complete experiment
* @param	Pointer to data structure
* @param	Pointer to an array to sace complete LUT
* @retval	Lenght of the LUT
*/
uint32_t generateDPVsignal(DF_DPVTypeDef* df, float* LUTcomplete) {

	uint16_t i, j;

	/* Establecemos un nº mínimo de samples en el pulso para evitar aliasing a frecuencias altas */
	uint16_t nSamplesPulse = 10;

	/* Tiempo de disparo de cada sample */
	float tTimer = df->Measurement.tPulse / nSamplesPulse;

	/* Calculamos t interval (t dc + t pulse) */
	float tInt = df->Measurement.step / df->Measurement.sr;

	/* Calculamos nº de samples en la zona DC */
	float nSamplesDC = ceil((tInt - df->Measurement.tPulse) / tTimer);

	/* Calculamos el nº de steps */
	// TODO
	uint32_t nSteps = abs((df->Measurement.stop - df->Measurement.start) / df->Measurement.step);

	uint32_t contRow = 0;	// Lleva el seguimiento de la posición de la LUT 


	/* Generamos el patrón de señal */
	if (df->Measurement.stop > df->Measurement.start) {		// Si steps suben...

		for (i = 0; i < nSteps; i++) {
			for (j = 0; j < nSamplesDC; j++) {				// Generamos parte DC..
				
				LUTcomplete[j + contRow] = df->Measurement.start + (df->Measurement.step * (i));

			}
			contRow += j;

			for (j = 0; j < nSamplesPulse; j++) {			// Generamos pulso...

				LUTcomplete[j + contRow] = (df->Measurement.start + df->Measurement.ePulse) + \
					(df->Measurement.step * i);
			}

			contRow += j;

		}

	}

	else {													// Si steps bajan...
		
		for (i = 0; i < nSteps; i++) {
			for (j = 0; j < nSamplesDC; j++) {				// Generamos parte DC..

				LUTcomplete[j + contRow] = df->Measurement.start - (df->Measurement.step * (i));

			}
			contRow += j;

			for (j = 0; j < nSamplesPulse; j++) {			// Generamos pulso...

				LUTcomplete[j + contRow] = (df->Measurement.start + df->Measurement.ePulse) - \
					(df->Measurement.step * i);
			}

			contRow += j;

		}

	}

	return contRow;

}

/**
* @brief	Generates NPV waveform for 1 complete experiment
* @param	Pointer to data structure
* @param	Pointer to an array to sace complete LUT
* @retval	Lenght of the LUT
*/
uint32_t generateNPVsignal(DF_NPVTypeDef* df, float* LUTcomplete) {

	uint16_t i, j;
	
	/* Establecemos un nº mínimo de samples en el pulso para evitar aliasing a frecuencias altas */
	uint16_t nSamplesPulse = 10;

	/* Tiempo de disparo de cada sample */
	float tTimer = df->Measurement.tPulse / nSamplesPulse;

	/* Calculamos t interval (t dc + t pulse) */
	float tInt = df->Measurement.step / df->Measurement.sr;

	/* Calculamos nº de samples en la zona DC */
	float nSamplesDC = ceil((tInt - df->Measurement.tPulse) / tTimer);

	/* Calculamos el nº de steps */
	uint32_t nSteps = abs((df->Measurement.stop - df->Measurement.start) / df->Measurement.step);

	uint32_t contRow = 0;

	/* Generamos el patrón de señal */
	if (df->Measurement.stop > df->Measurement.start) {			// Si step sube...

		for (i = 0; i < nSteps; i++) {
			for (j = 0; j < nSamplesDC; j++) {					// Generamos parte DC...

				LUTcomplete[j + contRow] = df->Measurement.start;
			}

			contRow += j;

			for (j = 0; j < nSamplesPulse; j++) {				// Generamos pulso...

				LUTcomplete[j + contRow] = df->Measurement.start + (df->Measurement.step * i);
			}
			contRow += j;
		}
	}
	
	else {														// Si step baja...

		for (i = 0; i < nSteps; i++) {
			for (j = 0; j < nSamplesDC; j++) {					// Generamos parte DC...

				LUTcomplete[j + contRow] = df->Measurement.start;
			}
			contRow += j;

			for (j = 0; j < nSamplesPulse; j++) {				// Generamos pulso...

				LUTcomplete[j + contRow] = df->Measurement.start - (df->Measurement.step * i);
			}
			contRow += j;
		}
		
	}

	return contRow;

}


/**
* @brief	Generates DNPV waveform for 1 complete experiment
* @param	Pointer to data structure
* @param	Pointer to an array to sace complete LUT
* @retval	Lenght of the LUT
*/
uint32_t generateDNPVsignal(DF_DNPVTypeDef* df, float* LUTcomplete) {

	uint16_t i, j;
	
	/* Establecemos un nº mínimo de samples en el pulso para evitar aliasing a frecuencias altas */
	uint16_t nSamplesPulse1 = 10;

	/* Tiempo de disparo de cada sample */
	float tTimer = df->Measurement.tPulse1 / nSamplesPulse1;

	/* Calculamos t interval (t dc + t pulse) */
	float tInt = df->Measurement.step / df->Measurement.sr;

	/* Calculamos nº de samples en la zona DC */
	float nSamplesDC = ceil((tInt - (df->Measurement.tPulse1 + df->Measurement.tPulse2 )) / tTimer);

	/* Nº de samples en pulse1 */
	float nSamplesP1 = ceil(df->Measurement.tPulse1 / tTimer);

	/* Nº de samples en pulse2 */
	float nSamplesP2 = ceil(df->Measurement.tPulse2 / tTimer);

	/* Calculamos el nº de steps */
	uint32_t nSteps = abs((df->Measurement.stop - df->Measurement.start) / df->Measurement.step);

	uint32_t contRow = 0;

	/* Generamos el patrón de señal */
	if (df->Measurement.stop > df->Measurement.start) {		// Si steps suben...
		for (i = 0; i < nSteps; i++) {

			for (j = 0; j < nSamplesDC; j++) {				// Generamos parte DC...

				LUTcomplete[j + contRow] = df->Measurement.start;
			}
			contRow += j;

			for (j = 0; j < nSamplesP1; j++) {				// Generamos pulse1...

				LUTcomplete[j + contRow] = df->Measurement.start + (df->Measurement.step * i);
			}
			contRow += j;

			for (j = 0; j < nSamplesP2; j++) {				// Generamos pulse2...

				LUTcomplete[j + contRow] = (df->Measurement.start + df->Measurement.ePulse) + \
					(df->Measurement.step * i);
			}
			contRow += j;

		}


	}

	else {													// Si steps bajan...
		for (i = 0; i < nSteps; i++) {

			for (j = 0; j < nSamplesDC; j++) {				// Generamos parte DC...

				LUTcomplete[j + contRow] = df->Measurement.start;
			}
			contRow += j;

			for (j = 0; j < nSamplesP1; j++) {				// Generamos pulse1...

				LUTcomplete[j + contRow] = df->Measurement.start - (df->Measurement.step * i);
			}
			contRow += j;

			for (j = 0; j < nSamplesP2; j++) {				// Generamos pulse2...

				LUTcomplete[j + contRow] = (df->Measurement.start + df->Measurement.ePulse) - \
					(df->Measurement.step * i);
			}
			contRow += j;

		}


	}

	return contRow;
}


/**
* @brief	Generates SWV waveform for 1 complete experiment
* @param	Pointer to data structure
* @param	Pointer to an auxiliar array
* @param	Pointer to an auxiliar array
* @param	Pointer to an array to sace complete LUT
* @retval	Lenght of the LUT
*/
uint32_t generateSWVsignal(DF_SWVTypeDef* df, float* LUT1, float* LUT2, float* LUTcomplete) {

	uint16_t i, j;
	uint32_t contRow = 0;

	/* Sacamos samples totales conociendo frecuencia */
	float fSampling = 100 * df->Measurement.freq;		// oversampling...

	/* Tiempo de disparo de cada sample */
	float tTimer = 1 / df->Measurement.freq;


	/* Calculamos el nº de steps */
	uint32_t nSteps = abs((df->Measurement.start - df->Measurement.stop) / df->Measurement.step);

	/* Hacemos la señal en dos pasos... */
	/* PASO 1: generamos la onda cuadrada */
	float nSamples1 = (1 / df->Measurement.freq) / tTimer;
	float nSamples2 = (1 / df->Measurement.freq) / tTimer;

	for (i = 0; i < nSamples1; i++) {				// Primer semiperíodo...

		LUT1[i] = df->Measurement.amplitude;
	}

	for (i = 0; i < nSamples2; i++) {				// Segundo semiperíodo...

		LUT2[i] = - (df->Measurement.amplitude);
	}


	/* PASO 2: generamos el DC offset */


	if (df->Measurement.start < df->Measurement.stop) {		// Si steps suben...

		for (i = 0; i < nSteps; i++) {

			for (j = 0; j < nSamples1; j++) {			// Generamos primer semiperíodo más parte DC

				LUTcomplete[j + contRow] = (df->Measurement.start + df->Measurement.step * i) + \
					LUT1[j];
			}
			contRow += j;

			for (j = 0; j < nSamples2; j++) {			// Generamos segundo semiperíodo más parte DC

				LUTcomplete[j + contRow] = (df->Measurement.start + df->Measurement.step * i) + \
					LUT2[j];
			}
			contRow += j;
		}

	}

	else {												// Si steps bajan...

		for (i = 0; i < nSteps; i++) {

			for (j = 0; j < nSamples1; j++) {			// Generamos primer semiperíodo más parte DC

				LUTcomplete[j + contRow] = (df->Measurement.start - df->Measurement.step * i) + \
					LUT1[j];
			}
			contRow += j;

			for (j = 0; j < nSamples2; j++) {			// Generamos segundo semiperíodo más parte DC

				LUTcomplete[j + contRow] = (df->Measurement.start - df->Measurement.step * i) + \
					LUT2[j];
			}
			contRow += j;
		}

	}


	return contRow;
}

/**
* @brief	Generates ACV waveform for 1 complete experiment
* @param	Pointer to data structure
* @param	Pointer to an auxiliar array
* @param	Pointer to an array to sace complete LUT
* @retval	Lenght of the LUT
*/
uint32_t generateACVsignal(DF_ACTypeDef* df, float* LUT1, float* LUTcomplete) {

	uint16_t i, j;
	float nSteps;

	/* Sacamos samples totales conociendo la frecuencia */
	float fSampling = df->Measurement.freq * 100;
	float tTimer = 1 / fSampling;
	float nSamplesAC = ceil((1 / df->Measurement.freq) / tTimer);

	/* Calculamos el t interval */
	float tInt = df->Measurement.step / df->Measurement.sr;

	/* PASO 1 : generación de la senoide para todo el período */
	/* Calculamos nº períodos AC que caben en t interval */
	float nPerAC = tInt / (1 / df->Measurement.freq);

	/* Esto nos da el nº de ptos totales del t interval */
	float nSamplesTint = ceil(nPerAC * nSamplesAC);

	/* Generamos la senoidal para todo el t int */
	/* IMPORTANTE */
	/* En caso de que el nº de períodos AC que caben en el t itnerval no sea
	 un número entero, debemos de poder sacar los puntos del período incompleto.
	 Esto se hace en la segunda parte del "if" */

	uint32_t contRow = 0;

	for (i = 0; i < ceil(nPerAC); i++) {

		if (i < nPerAC) {						// Si no es el último período...
			for (j = 0; j < nSamplesAC; j++) {

				LUT1[j + contRow] = df->Measurement.ACamplitude * sin((2 * PI / nSamplesAC)*j);
			}
			contRow += j;
		}
		else {									// Si último período...

			float nSamplesRest = nSamplesTint - contRow;

			for (j = 0; j < nSamplesRest; j++) {

				LUT1[j + contRow] = df->Measurement.ACamplitude * sin((2 * PI / nSamplesAC)*j);
			}
		}

	}

	/* Generación del offset + AC para toda la prueba */
	nSteps = ceil(abs((df->Measurement.start - df->Measurement.stop) / df->Measurement.step));

	contRow = 0;

	if (df->Measurement.start < df->Measurement.stop) {			// Si steps suben...
		for (i = 0; i < nSteps; i++) {
			for (j = 0; j < nSamplesTint; j++) {

				LUTcomplete[j + contRow] = (df->Measurement.start + (df->Measurement.step*i)) + \
					LUT1[j];
			}
			contRow += j;
		}
	}
	else {														// Si steps bajan...
		for (i = 0; i < nSteps; i++) {
			for (j = 0; j < nSamplesTint; j++) {

				LUTcomplete[j + contRow] = (df->Measurement.start - (df->Measurement.step*i)) + \
					LUT1[j];
			}
			contRow += j;

		}
	}

	return contRow;
}