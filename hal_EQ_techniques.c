// TODO : WORK IN PROGRESS

#include "hal_EQ_techniques.h"
#include "led.h"








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
	// es necesario cambiar la funci�n fabsf que devuelve un float
	// a abs, que devuelve un int

	uint32_t i;
	
	uint32_t nSamples = ceil(fabsf(eStart - eStop) / eStep) + 1;				// Calculamos n�mero de puntos...
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
//static uint32_t concatenateLUTs(float* lut1, float* lut2, float* lut3, float* lutC, uint32_t n1, uint32_t n2, uint32_t n3) {

//	uint32_t i;
//	
//	n1--;																// El �ltimo pto de la LUT1 es igual al primero de la LUT2
//	n2--;																// por lo que lo desechamos. Lo mismo para LUT2 y LUT3
//	
//	for (i = 0; i < n1; i++) {									
//		lutC[i] = lut1[i];												
//	}

//	for (i = 0; i < n2; i++) {									
//		lutC[i + n1] = lut2[i];
//	}

//	for (i = 0; i < n3; i++) {
//		lutC[i + (n1 + n2)] = lut3[i];
//	}

//	return (n1 + n2 + n3);

//}


/**
* @brief	Computes the DAC value from the voltage value 
* @param	Pointer to LUT with output voltage data
* @param	Pointer to an array to save the DAC LUT
* @param	No. samples of LUT with the output voltage data
* @retval	None
*/
//void generateDACValues(float* lut, uint16_t* data, uint32_t n) {

//	uint32_t i;
//	
//	/* Rutina para calcular la potencia de 2^NBITSDAC (para evitar el uso de pow())*/
//	uint32_t DACp = 2;		// base
//	for (i = 0; i < (NBITSDAC - 1); i++) {
//		DACp *= 2;
//	}

//	for (i = 0; i <= n; i++) {
//		data[i] = (( (DACp + 1) * lut[i]) / VREF);
//	}

//}


/************************************************/
/*					APIs						*/
/************************************************/
///**
//* @brief	Generates CV waveform for 1 complete scan
//* @param	Pointer to data structure
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an array to sace complete LUT
//* @retval	Lenght of the LUT
//*/
//uint32_t generateCVsignal(DF_CVTypeDef* df, float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete) {

//	uint32_t nSamples1 = generateRamp(df->start, df->vtx1, df->step, LUT1);
//	uint32_t nSamples2 = generateRamp(df->vtx1, df->vtx2, df->step, LUT2);
//	uint32_t nSamples3 = generateRamp(df->vtx2, df->vtx1, df->step, LUT3);

//	uint32_t lengthLUT = concatenateLUTs(LUT1, LUT2, LUT3, LUTcomplete, nSamples1, nSamples2, nSamples3);

//	return lengthLUT;
//}

///**
//* @brief	Generates LSV waveform for 1 complete scan
//* @param	Pointer to data structure
//* @param	Pointer to an array to sace complete LUT
//* @retval	Lenght of the LUT
//*/
//uint32_t generateLSVsignal(DF_LSVTypeDef* df, float* LUTcomplete) {
//	
//	uint32_t lengthLUT = generateRamp(df->start, df->stop, df->step, LUTcomplete);

//	/* Escribimos el Estop en el �ltimo valor de la LUT */
//	LUTcomplete[lengthLUT-1] = df->stop;

//	return lengthLUT;
//}


///**
//* @brief	Generates SCV waveform for 1 complete scan
//* @param	Pointer to data structure
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an array to sace complete LUT
//* @retval	Lenght of the LUT
//*/
//uint32_t generateSCVsignal(DF_SCVTypeDef* df, float* LUT1, float* LUT2, float* LUT3, float* LUTcomplete) {

//	uint32_t lengthLUT;
//	uint32_t nSamples1 = generateRamp(df->start, df->stop, df->step, LUT1);
//	uint32_t nSamples2 = generateRamp(df->stop, df->start, df->step, LUT2);

//	/* Enviamos el �ltimo punto del per�odo completo, que es el start */
//	// Usamos la misma funci�n concatenateLUTs, pero como en este caso son dos tramos hacemos
//	// este truco y enviamos el �ltimo punto en la tercera LUT. As� no necesitamos definir
//	// otra funci�n.
//	uint32_t nSamples3 = 1;
//	LUT3[0] = df->start;

//	lengthLUT = concatenateLUTs(LUT1, LUT2, LUT3, LUTcomplete, nSamples1, nSamples2, nSamples3);

//	return lengthLUT;

//}


///**
//* @brief	Generates DPV waveform for 1 complete experiment
//* @param	Pointer to data structure
//* @param	Pointer to an array to sace complete LUT
//* @retval	Lenght of the LUT
//*/
//uint32_t generateDPVsignal(DF_DPVTypeDef* df, float* LUTcomplete) {

//	uint16_t i, j;

//	/* Establecemos un n� m�nimo de samples en el pulso para evitar aliasing a frecuencias altas */
//	uint16_t nSamplesPulse = 10;

//	/* Tiempo de disparo de cada sample */
//	float tTimer = df->tPulse / nSamplesPulse;

//	/* Calculamos t interval (t dc + t pulse) */
//	float tInt = df->step / df->sr;

//	/* Calculamos n� de samples en la zona DC */
//	float nSamplesDC = ceil((tInt - df->tPulse) / tTimer);

//	/* Calculamos el n� de steps */
//	// TODO
//	uint32_t nSteps = abs((df->stop - df->start) / df->step);

//	uint32_t contRow = 0;	// Lleva el seguimiento de la posici�n de la LUT 


//	/* Generamos el patr�n de se�al */
//	if (df->stop > df->start) {		// Si steps suben...

//		for (i = 0; i < nSteps; i++) {
//			for (j = 0; j < nSamplesDC; j++) {				// Generamos parte DC..
//				
//				LUTcomplete[j + contRow] = df->start + (df->step * (i));

//			}
//			contRow += j;

//			for (j = 0; j < nSamplesPulse; j++) {			// Generamos pulso...

//				LUTcomplete[j + contRow] = (df->start + df->ePulse) + \
//					(df->step * i);
//			}

//			contRow += j;

//		}

//	}

//	else {													// Si steps bajan...
//		
//		for (i = 0; i < nSteps; i++) {
//			for (j = 0; j < nSamplesDC; j++) {				// Generamos parte DC..

//				LUTcomplete[j + contRow] = df->start - (df->step * (i));

//			}
//			contRow += j;

//			for (j = 0; j < nSamplesPulse; j++) {			// Generamos pulso...

//				LUTcomplete[j + contRow] = (df->start + df->ePulse) - \
//					(df->step * i);
//			}

//			contRow += j;

//		}

//	}

//	return contRow;

//}

///**
//* @brief	Generates NPV waveform for 1 complete experiment
//* @param	Pointer to data structure
//* @param	Pointer to an array to sace complete LUT
//* @retval	Lenght of the LUT
//*/
//uint32_t generateNPVsignal(DF_NPVTypeDef* df, float* LUTcomplete) {

//	uint16_t i, j;
//	
//	/* Establecemos un n� m�nimo de samples en el pulso para evitar aliasing a frecuencias altas */
//	uint16_t nSamplesPulse = 10;

//	/* Tiempo de disparo de cada sample */
//	float tTimer = df->tPulse / nSamplesPulse;

//	/* Calculamos t interval (t dc + t pulse) */
//	float tInt = df->step / df->sr;

//	/* Calculamos n� de samples en la zona DC */
//	float nSamplesDC = ceil((tInt - df->tPulse) / tTimer);

//	/* Calculamos el n� de steps */
//	uint32_t nSteps = abs((df->stop - df->start) / df->step);

//	uint32_t contRow = 0;

//	/* Generamos el patr�n de se�al */
//	if (df->stop > df->start) {			// Si step sube...

//		for (i = 0; i < nSteps; i++) {
//			for (j = 0; j < nSamplesDC; j++) {					// Generamos parte DC...

//				LUTcomplete[j + contRow] = df->start;
//			}

//			contRow += j;

//			for (j = 0; j < nSamplesPulse; j++) {				// Generamos pulso...

//				LUTcomplete[j + contRow] = df->start + (df->step * i);
//			}
//			contRow += j;
//		}
//	}
//	
//	else {														// Si step baja...

//		for (i = 0; i < nSteps; i++) {
//			for (j = 0; j < nSamplesDC; j++) {					// Generamos parte DC...

//				LUTcomplete[j + contRow] = df->start;
//			}
//			contRow += j;

//			for (j = 0; j < nSamplesPulse; j++) {				// Generamos pulso...

//				LUTcomplete[j + contRow] = df->start - (df->step * i);
//			}
//			contRow += j;
//		}
//		
//	}

//	return contRow;

//}


///**
//* @brief	Generates DNPV waveform for 1 complete experiment
//* @param	Pointer to data structure
//* @param	Pointer to an array to sace complete LUT
//* @retval	Lenght of the LUT
//*/
//uint32_t generateDNPVsignal(DF_DNPVTypeDef* df, float* LUTcomplete) {

//	uint16_t i, j;
//	
//	/* Establecemos un n� m�nimo de samples en el pulso para evitar aliasing a frecuencias altas */
//	uint16_t nSamplesPulse1 = 10;

//	/* Tiempo de disparo de cada sample */
//	float tTimer = df->tPulse1 / nSamplesPulse1;

//	/* Calculamos t interval (t dc + t pulse) */
//	float tInt = df->step / df->sr;

//	/* Calculamos n� de samples en la zona DC */
//	float nSamplesDC = ceil((tInt - (df->tPulse1 + df->tPulse2 )) / tTimer);

//	/* N� de samples en pulse1 */
//	float nSamplesP1 = ceil(df->tPulse1 / tTimer);

//	/* N� de samples en pulse2 */
//	float nSamplesP2 = ceil(df->tPulse2 / tTimer);

//	/* Calculamos el n� de steps */
//	uint32_t nSteps = abs((df->stop - df->start) / df->step);

//	uint32_t contRow = 0;

//	/* Generamos el patr�n de se�al */
//	if (df->stop > df->start) {		// Si steps suben...
//		for (i = 0; i < nSteps; i++) {

//			for (j = 0; j < nSamplesDC; j++) {				// Generamos parte DC...

//				LUTcomplete[j + contRow] = df->start;
//			}
//			contRow += j;

//			for (j = 0; j < nSamplesP1; j++) {				// Generamos pulse1...

//				LUTcomplete[j + contRow] = df->start + (df->step * i);
//			}
//			contRow += j;

//			for (j = 0; j < nSamplesP2; j++) {				// Generamos pulse2...

//				LUTcomplete[j + contRow] = (df->start + df->ePulse) + \
//					(df->step * i);
//			}
//			contRow += j;

//		}


//	}

//	else {													// Si steps bajan...
//		for (i = 0; i < nSteps; i++) {

//			for (j = 0; j < nSamplesDC; j++) {				// Generamos parte DC...

//				LUTcomplete[j + contRow] = df->start;
//			}
//			contRow += j;

//			for (j = 0; j < nSamplesP1; j++) {				// Generamos pulse1...

//				LUTcomplete[j + contRow] = df->start - (df->step * i);
//			}
//			contRow += j;

//			for (j = 0; j < nSamplesP2; j++) {				// Generamos pulse2...

//				LUTcomplete[j + contRow] = (df->start + df->ePulse) - \
//					(df->step * i);
//			}
//			contRow += j;

//		}


//	}

//	return contRow;
//}


///**
//* @brief	Generates SWV waveform for 1 complete experiment
//* @param	Pointer to data structure
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an array to sace complete LUT
//* @retval	Lenght of the LUT
//*/
//uint32_t generateSWVsignal(DF_SWVTypeDef* df, float* LUT1, float* LUT2, float* LUTcomplete) {

//	uint16_t i, j;
//	uint32_t contRow = 0;

//	/* Sacamos samples totales conociendo frecuencia */
//	float fSampling = 100 * df->freq;		// oversampling...

//	/* Tiempo de disparo de cada sample */
//	float tTimer = 1 / df->freq;


//	/* Calculamos el n� de steps */
//	uint32_t nSteps = abs((df->start - df->stop) / df->step);

//	/* Hacemos la se�al en dos pasos... */
//	/* PASO 1: generamos la onda cuadrada */
//	float nSamples1 = (1 / df->freq) / tTimer;
//	float nSamples2 = (1 / df->freq) / tTimer;

//	for (i = 0; i < nSamples1; i++) {				// Primer semiper�odo...

//		LUT1[i] = df->amplitude;
//	}

//	for (i = 0; i < nSamples2; i++) {				// Segundo semiper�odo...

//		LUT2[i] = - (df->amplitude);
//	}


//	/* PASO 2: generamos el DC offset */


//	if (df->start < df->stop) {		// Si steps suben...

//		for (i = 0; i < nSteps; i++) {

//			for (j = 0; j < nSamples1; j++) {			// Generamos primer semiper�odo m�s parte DC

//				LUTcomplete[j + contRow] = (df->start + df->step * i) + \
//					LUT1[j];
//			}
//			contRow += j;

//			for (j = 0; j < nSamples2; j++) {			// Generamos segundo semiper�odo m�s parte DC

//				LUTcomplete[j + contRow] = (df->start + df->step * i) + \
//					LUT2[j];
//			}
//			contRow += j;
//		}

//	}

//	else {												// Si steps bajan...

//		for (i = 0; i < nSteps; i++) {

//			for (j = 0; j < nSamples1; j++) {			// Generamos primer semiper�odo m�s parte DC

//				LUTcomplete[j + contRow] = (df->start - df->step * i) + \
//					LUT1[j];
//			}
//			contRow += j;

//			for (j = 0; j < nSamples2; j++) {			// Generamos segundo semiper�odo m�s parte DC

//				LUTcomplete[j + contRow] = (df->start - df->step * i) + \
//					LUT2[j];
//			}
//			contRow += j;
//		}

//	}


//	return contRow;
//}

///**
//* @brief	Generates ACV waveform for 1 complete experiment
//* @param	Pointer to data structure
//* @param	Pointer to an auxiliar array
//* @param	Pointer to an array to sace complete LUT
//* @retval	Lenght of the LUT
//*/
//uint32_t generateACVsignal(DF_ACTypeDef* df, float* LUT1, float* LUTcomplete) {

//	uint16_t i, j;
//	float nSteps;

//	/* Sacamos samples totales conociendo la frecuencia */
//	float fSampling = df->freq * 100;
//	float tTimer = 1 / fSampling;
//	float nSamplesAC = ceil((1 / df->freq) / tTimer);

//	/* Calculamos el t interval */
//	float tInt = df->step / df->sr;

//	/* PASO 1 : generaci�n de la senoide para todo el per�odo */
//	/* Calculamos n� per�odos AC que caben en t interval */
//	float nPerAC = tInt / (1 / df->freq);

//	/* Esto nos da el n� de ptos totales del t interval */
//	float nSamplesTint = ceil(nPerAC * nSamplesAC);

//	/* Generamos la senoidal para todo el t int */
//	/* IMPORTANTE */
//	/* En caso de que el n� de per�odos AC que caben en el t itnerval no sea
//	 un n�mero entero, debemos de poder sacar los puntos del per�odo incompleto.
//	 Esto se hace en la segunda parte del "if" */

//	uint32_t contRow = 0;

//	for (i = 0; i < ceil(nPerAC); i++) {

//		if (i < nPerAC) {						// Si no es el �ltimo per�odo...
//			for (j = 0; j < nSamplesAC; j++) {

//				LUT1[j + contRow] = df->ACamplitude * sin((2 * PI / nSamplesAC)*j);
//			}
//			contRow += j;
//		}
//		else {									// Si �ltimo per�odo...

//			float nSamplesRest = nSamplesTint - contRow;

//			for (j = 0; j < nSamplesRest; j++) {

//				LUT1[j + contRow] = df->ACamplitude * sin((2 * PI / nSamplesAC)*j);
//			}
//		}

//	}

//	/* Generaci�n del offset + AC para toda la prueba */
//	nSteps = ceil(abs((df->start - df->stop) / df->step));

//	contRow = 0;

//	if (df->start < df->stop) {			// Si steps suben...
//		for (i = 0; i < nSteps; i++) {
//			for (j = 0; j < nSamplesTint; j++) {

//				LUTcomplete[j + contRow] = (df->start + (df->step*i)) + \
//					LUT1[j];
//			}
//			contRow += j;
//		}
//	}
//	else {														// Si steps bajan...
//		for (i = 0; i < nSteps; i++) {
//			for (j = 0; j < nSamplesTint; j++) {

//				LUTcomplete[j + contRow] = (df->start - (df->step*i)) + \
//					LUT1[j];
//			}
//			contRow += j;

//		}
//	}

//	return contRow;
//}



/* FUNCION EN PRUEBAS PARA GENERAR LUT PERIODICA */
//void generateDPVwaveform(DF_DPVTypeDef* df, uint16_t* LUT, uint32_t* nSExp, uint32_t* nSLUT){

//	uint32_t i,j;
//	
//	if(eWE1 == E_NONE | eWE2 == E_NONE){				// Si entramos por primera vez a lanzar el experimento calculamos todos los datos necesarios...
//	
//		uint32_t fSampling = 10000;		// TODO: este valor depender� del filtro seleccionado.
//																	// Hemos seleccionado aqu� la frec para un filtro de butterworth del equipo de CEMITEC.	
//																	// La idea es que esta frecuencia sea de aprox 20x la frec de corte del filtro.
//																	// Hay que enviar el valor del filtro seleccionado a esta funci�n como argumento.
//		
//		// Recupero todos los decimales para los c�lculos...
//		float step =  df->Init.step / 1000.0;		// V  ( ((df->step / 1000.0) * VREF) / 32768.0)
//		float sr =  df->Init.sr / 1000.0;															// V/seg
//		
//		float tPulse = df->Init.tPulse / 1000.0;												// seg
//		float stop = ((int16_t)(df->Init.stop)) / 1000.0;			// V
//		float start = ((int16_t)(df->Init.start)) / 1000.0; //((df->start - 32768.0) * VREF) / 32768.0;		// V
//		float ePulse = ((int16_t)(df->Init.ePulse)) / 1000.0;		
//		
//		float tSampling = 1 / (float)fSampling;
//		float tInt = step / sr;
//		
//		// Transformo a valores DAC para generaci�n de forma de onda...
//		df->start = (((start * 32768.0) / VREF) + 32768.0);
//		df->stop = (((stop * 32768.0) / VREF) + 327680.0);
//		df->ePulse = ((ePulse * 32768.0) / VREF);
//		df->step = ((step * 32768.0) / VREF);
//		
//		// N�mero de samples para cada tramo de la se�al en cada per�odo
//		df->nSamples1 = ceil((tInt - tPulse) / tSampling);
//		df->nSamples2 = ceil((tPulse / tSampling));
//		
//		// Calculamos n� de steps
//		df->nSteps = ceil(fabs(stop - start) / step);
//		
//		
//		/* Inicializamos las variables para llevar la cuenta
//		de los samples que vamos leyendo de la LUT y de los 
//		samples totales del exp	en la ISR */
//		*nSLUT = df->nSamples1 + df->nSamples2;				// Samples de cada per�odo de LUT (suma samples cada tramo)
//		*nSExp = df->nSteps * (*nSLUT);								// Samples totales del experimento
//	}
//	
//	
//	if (df->stop > df->start){			// Si steps suben....
//		
//		// Primera parte de la onda...
//		for(j = 0; j < df->nSamples1; j++){
//			LUT[j] = df->start + (df->step * (df->realStep));
//		
//		}
//		
//		// Segunda parte de la onda...
//		for(j = 0; j < df->nSamples2; j++){
//			LUT[j + df->nSamples1] = (df->start + df->ePulse) + (df->step * df->realStep);
//		
//		}
//		//df->realStep++;				// Nos sirve para llevar cuenta de en qu� step estamos generando la forma de onda. Comienza en cero.
//	}
//	
//	else {																									// Si steps bajan...
//	
//		// Primera parte de la onda...
//		for(j = 0; j < df->nSamples1; j++){
//			LUT[j] = df->start - (df->step * (df->realStep));
//		
//		}
//		
//		// Segunda parte de la onda...
//		for(j = 0; j < df->nSamples2; j++){
//			LUT[j + df->nSamples1] = (df->start + df->ePulse) - (df->step * df->realStep);
//		
//		}
//		//df->realStep++;

//	}
//	
//	df->realStep++;																// subimos un step para la siguiente vez
//	
//}
	
//void generateDPVwaveform(exp_param_values_t* e, status_experiment* eWE, uint16_t* LUT){

//	uint32_t i,j;
//	
//	if(*eWE == E_NONE){				// Si entramos por primera vez a lanzar el experimento calculamos todos los datos necesarios...
//		
//		//uint32_t fSampling = 10000;		// TODO: este valor depender� del filtro seleccionado.
//																	// Hemos seleccionado aqu� la frec para un filtro de butterworth del equipo de CEMITEC.	
//																	// La idea es que esta frecuencia sea de aprox 20x la frec de corte del filtro.
//																	// Hay que enviar el valor del filtro seleccionado a esta funci�n como argumento.
//		
//		// Recupero todos los decimales para los c�lculos...
//		float step = e->Init.step / 1000.0;
//		float sr = e->Init.sr / 1000.0;
//		
//		float tPulse = e->Init.tPulse1 / 1000.0;
//		float stop = ((int16_t)(e->Init.stop)) / 1000.0;
//		float start = ((int16_t)(e->Init.start)) / 1000.0;
//		float ePulse = ((int16_t)(e->Init.ePulse1)) / 1000.0;
//		
//		float tSampling = 1 / (float)e->fSampling;
//		float tInt = step / sr;


//		
//		// N�mero de samples para cada tramo de la se�al en cada per�odo
//		e->nSamples1 = ceil((tInt - tPulse) / tSampling);
//		e->nSamples2 = ceil((tPulse / tSampling));
//		
//		// Calculamos n� de steps
//		e->nSteps = ceil(fabs(stop - start) / step);
//		
//		
//		/* Inicializamos las variables para llevar la cuenta
//		de los samples que vamos leyendo de la LUT y de los 
//		samples totales del exp	en la ISR */
//		e->nSamplesLUT = e->nSamples1 + e->nSamples2;				// Samples de cada per�odo de LUT (suma samples cada tramo)
//		e->nSamplesExp = e->nSteps * e->nSamplesLUT;								// Samples totales del experimento
//		
//				
//		// Transformo a valores DAC para generaci�n de forma de onda...
//		e->runTime.start = (((start * 32768.0) / VREF) + 32768.0);
//		e->runTime.stop = (((stop * 32768.0) / VREF) + 327680.0);
//		e->runTime.ePulse1 = ((ePulse * 32768.0) / VREF);
//		e->runTime.step = ((step * 32768.0) / VREF);
//	}
//	
//	
//	if (e->Init.stop > e->Init.start){			// Si steps suben....
//		
//		// Primera parte de la onda...
//		for(j = 0; j < e->nSamples1; j++){
//			LUT[j] = e->runTime.start + (e->runTime.step * (e->contStep));
//		
//		}
//		
//		// Segunda parte de la onda...
//		for(j = 0; j < e->nSamples2; j++){
//			LUT[j + e->nSamples1] = (e->runTime.start + e->runTime.ePulse1) + (e->runTime.step * e->contStep);
//		
//		}

//	}
//	
//	else {																									// Si steps bajan...
//	
//		// Primera parte de la onda...
//		for(j = 0; j < e->nSamples1; j++){
//			LUT[j] = e->runTime.start - (e->runTime.step * (e->cont));
//		
//		}
//		
//		// Segunda parte de la onda...
//		for(j = 0; j < e->nSamples2; j++){
//			LUT[j + e->nSamples1] = (e->runTime.start + e->runTime.ePulse1) - (e->runTime.step * e->cont);
//		
//		}
//		//df->realStep++;

//	}
//	
//	e->cont++;																// subimos un step para la siguiente vez
//	
//}



// Funci�n que genera 1024 ptos de se�al DPV
void generateDPV(exp_param_values_t* e, uint16_t* LUT){
	
	uint16_t i;
	
	if(e->upwardsStep){										// Steps suben...
		
		for(i = 0; i < NSAMPLESLUT; i++){
			if(e->contSamplesPer <= e->nSamples1){									// Estamos en primera parte de la onda (DC)
			
				LUT[i] = e->Init.start + (e->Init.step * e->contStep);
			}
			
			else if((e->contSamplesPer - e->nSamples1) <= e->nSamples2){		// Estamos en segunda parte de la onda (pulse)
			
				LUT[i] = (e->Init.start + (e->Init.step * e->contStep)) + e->Init.ePulse1;
			}
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer > e->nSamplesPer){													// Hemos terminado de sacar un per�odo completo...?
			
				e->contSamplesPer = 0;																				// Reseteamos contadores y subimosr un step el siguiente per�odo...
				e->contStep++;
			}
		
		}
	}
	
	else{ 																								// Steps bajan...
		
		for(i = 0; i < NSAMPLESLUT; i++){
			if(e->contSamplesPer <= e->nSamples1){									// Estamos en primera parte de la onda (DC)
			
				LUT[i] = e->Init.start - (e->Init.step * e->contStep);
			}
			
			else if((e->contSamplesPer - e->nSamples1) <= e->nSamples2){		// Estamos en segunda parte de la onda (pulse)
			
				LUT[i] = (e->Init.start - (e->Init.step * e->contStep)) + e->Init.ePulse1;
			}
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer > e->nSamplesPer){													// Hemos terminado de sacar un per�odo completo...?
			
				e->contSamplesPer = 0;																				// Reseteamos contadores y subimosr un step el siguiente per�odo...
				e->contStep++;
			}
		
		}
	}

}

// Funci�n para cargar datos recibidos desde el PC en la estructura del experimento correspondiente
void load_data(uint8_t* buff, exp_param_values_t* e, pretreat_param_t* p, exp_config_t* eConfig){
	
	/* Recogemos datos pretratamiento */
	p->tCond =  ((buff[10] << 8) | (buff[11] & 0xFF));
	p->eCond = ((buff[12] << 8) | (buff[13] & 0xFF));
	p->tDep =  ((buff[14] << 8) | (buff[15] & 0xFF));
	p->eDep =  ((buff[16] << 8) | (buff[17] & 0xFF));
	p->tEq = ((buff[18] << 8) | (buff[19] & 0xFF));
	p->eEq = ((buff[22] << 8) | (buff[23] & 0xFF));
		
	/* Recogemos otros datos del experimento */
	eConfig->bipot = buff[3];
	eConfig->cell_on = buff[9];
	eConfig->exp = buff[5];
	
	
	/* en funci�n de la t�cnica guardamos los datos del experimento en la estructura correspondiente */
	switch(eConfig->exp){
		case 0:													// CV
			//load_CV_data(e, buff);
			break;
		
		case 1:													// LSV
			//load_LSV_data(e, buff);
			break;
		
		case 2:													// SCV
			//load_SCV_data(e, buff);
			break;
		
		case 3:													// DPV
			load_DPV_data(e, buff);
			break;
			
		case 4:													// NPV
			//load_NPV_data(e, buff);
			break;
		
		case 5:													// DNPV
			//load_DNPV_data(e, buff);
			break;
		
		case 6:													// SWV
			//load_SWV_data(e, buff);
			break;
		
		case 7:													// ACV
			//load_ACV_data(e, buff);
			break;
	
	
	
	}

}
	

void generate_data(exp_param_values_t* e,	exp_config_t* eConfig, uint16_t* lut){

	/* En funci�n de la t�cnica guardamos los datos del experimento en la estructura correspondiente */
	switch(eConfig->exp){
		case 0:													// CV
			// TODO
			break;

		case 1:													// LSV
			// TODO
			break;
		
		case 2:													// SCV
			// TODO
			break;
		
		case 3:													// DPV
			generateDPV(e, lut);
			break;
			
		case 4:													// NPV
			// TODO
			break;
		
		case 5:													// DNPV
			// TODO
			break;
		
		case 6:													// SWV
			// TODO
			break;
		
		case 7:													// ACV
			// TODO
			break;



	}
}



/* 	FUNCIONES PARA PRECARGA DE DATOS EN ESTRUCTURAS PARA GENERAR EL EXPERIMENTO */
//void load_CV_data(DF_CVTypeDef* df, uint8_t* cmd){
//								
//	/* Experiment values */
//	df->start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->vtx1 = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->vtx2 = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->step = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->sr = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->scans = ((cmd[32] << 8) | (cmd[33] & 0xFF));

//					
//}


//void load_LSV_data(DF_LSVTypeDef* df, uint8_t* cmd){

//	/* Experiment values */
//	df->start = (uint16_t) ((int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->stop = (uint16_t) ((int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->step = (uint16_t) ((int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->sr = (uint16_t) ((int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	

//}



//void load_SCV_data(DF_SCVTypeDef* df, uint8_t* cmd){

//	/* Experiment values */
//	df->start = (uint16_t)((int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->stop = (uint16_t)((int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->step = (uint16_t) ((int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->tHold = (uint16_t) ((int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->sr = (uint16_t) ((int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->scans = ((cmd[32] << 8) | (cmd[33] & 0xFF));	

//}



void load_DPV_data(exp_param_values_t* e, uint8_t* cmd){
				
	
	float step, sr, tPulse, stop, start, tSampling, tInt;
	
	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado
	
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.ePulse1 =((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.tPulse1 = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.sr = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	
	// Recupero todos los decimales para los c�lculos...
	step = e->Init.step / 1000.0;
	sr = e->Init.sr / 1000.0;
		
	tPulse = e->Init.tPulse1 / 1000.0;
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
		
	tSampling = 1 / (float)e->fSampling;
	tInt = step / sr;
	
	if(stop > start){
		e->upwardsStep = 1;
	}
	else{
		e->upwardsStep = 0;
	}
	
	// N�mero de samples para cada tramo de la se�al en cada per�odo
	e->nSamples1 = ceil((tInt - tPulse) / tSampling);
	e->nSamples2 = ceil((tPulse / tSampling));
		
	// Calculamos n� de steps
	e->nSteps = ceil(fabs(stop - start) / step);
		
		
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1 + e->nSamples2;				// Samples de cada per�odo de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;								// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;

}


void load_NPV_data(exp_param_values_t* e, uint8_t* cmd){
	
	float step, sr, tPulse, stop, start, tSampling, tInt;
	
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.tPulse1 = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.sr = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	
	// Recupero todos los decimales para los c�lculos...
	step = e->Init.step / 1000.0;
	sr = e->Init.sr / 1000.0;
		
	tPulse = e->Init.tPulse1 / 1000.0;
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
		
	tSampling = 1 / (float)e->fSampling;
	tInt = step / sr;
	
	
	// N�mero de samples para cada tramo de la se�al en cada per�odo
	e->nSamples1 = ceil((tInt - tPulse) / tSampling);
	e->nSamples2 = ceil((tPulse / tSampling));
		
	// Calculamos n� de steps
	e->nSteps = ceil(fabs(stop - start) / step);
		
		
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1 + e->nSamples2;				// Samples de cada per�odo de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;								// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
	
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
}


void load_DNPV_data(exp_param_values_t* e, uint8_t* cmd){

	float step, sr, tPulse1, tPulse2, stop, start, tSampling, tInt;
	
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.ePulse1 = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.tPulse1 = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.tPulse2 = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	
	// Recupero los decimales
	step = e->Init.step / 1000.0;
	sr = e->Init.sr / 1000.0;
	
	tPulse1 = e->Init.tPulse1 / 1000.0;
	tPulse2 = e->Init.tPulse2 / 1000.0;
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
	
	tSampling = 1 / (float)e->fSampling;
	tInt = step / sr;
	
	// N�mero de samples para cada tramo de la se�al en cada per�odo
	e->nSamples1 = ceil((tInt - (tPulse1 + tPulse2)) / tSampling);
	e->nSamples2 = ceil(tPulse1 / tSampling);
	e->nSamples3 = ceil(tPulse2 / tSampling);
		
	// Calculamos n� de steps
	e->nSteps = ceil(fabs(stop - start) / step);
		
		
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1 + e->nSamples2 + e->nSamples3;				// Samples de cada per�odo de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;												// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
	
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;


}

void load_SWV_data(exp_param_values_t* e, uint8_t* cmd){
	
	float step, period, stop, start, tSampling;
	
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.amplitude = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.freq = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	
	// Recupero los decimales
	step = e->Init.step / 1000.0;

	period = 1 / (float)e->Init.freq;
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
	
	tSampling = 1 / (float)e->fSampling;

	
	// N�mero de samples para cada tramo de la se�al en cada per�odo
	e->nSamples1 = ceil((period / 2) / tSampling);
	e->nSamples2 = ceil((period / 2) / tSampling);

		
	// Calculamos n� de steps
	e->nSteps = ceil(fabs(stop - start) / step);
		
		
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1 + e->nSamples2;				// Samples de cada per�odo de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;												// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;


}


///* Funci�n de muestra de c�mo estaba hecho anteriormente con doubles */
//void load_ACV_data(DF_ACTypeDef* df, uint8_t* cmd){
//					
//	/* Experiment values */
//	df->start = (uint16_t) ((int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->stop = (uint16_t) ((int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->step = (uint16_t) ((int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->ACamplitude = (uint16_t) ((int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->freq = (uint16_t) ((int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);
//	df->sr = (uint16_t) ((int)(((( ((cmd[32] << 8) | (cmd[33] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0);


//}

/* Funci�n de muestra de c�mo estaba hecho anteriormente con doubles */
//void load_ACV_data(DF_ACTypeDef* df, uint8_t* cmd){
//					
//	/* Experiment values */
//	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->Measurement.stop = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->Measurement.step = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->Measurement.ACamplitude = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->Measurement.freq = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
//	df->Measurement.sr = (int)(((( ((cmd[32] << 8) | (cmd[33] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;


//}



