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
static uint32_t concatenateLUTs(float* lut1, float* lut2, float* lut3, float* lutC, uint32_t n1, uint32_t n2, uint32_t n3) {

	uint32_t i;
	
	n1--;																// El �ltimo pto de la LUT1 es igual al primero de la LUT2
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

	/* Escribimos el Estop en el �ltimo valor de la LUT */
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

	/* Enviamos el �ltimo punto del per�odo completo, que es el start */
	// Usamos la misma funci�n concatenateLUTs, pero como en este caso son dos tramos hacemos
	// este truco y enviamos el �ltimo punto en la tercera LUT. As� no necesitamos definir
	// otra funci�n.
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

	/* Establecemos un n� m�nimo de samples en el pulso para evitar aliasing a frecuencias altas */
	uint16_t nSamplesPulse = 10;

	/* Tiempo de disparo de cada sample */
	float tTimer = df->Measurement.tPulse / nSamplesPulse;

	/* Calculamos t interval (t dc + t pulse) */
	float tInt = df->Measurement.step / df->Measurement.sr;

	/* Calculamos n� de samples en la zona DC */
	float nSamplesDC = ceil((tInt - df->Measurement.tPulse) / tTimer);

	/* Calculamos el n� de steps */
	// TODO
	uint32_t nSteps = abs((df->Measurement.stop - df->Measurement.start) / df->Measurement.step);

	uint32_t contRow = 0;	// Lleva el seguimiento de la posici�n de la LUT 


	/* Generamos el patr�n de se�al */
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
	
	/* Establecemos un n� m�nimo de samples en el pulso para evitar aliasing a frecuencias altas */
	uint16_t nSamplesPulse = 10;

	/* Tiempo de disparo de cada sample */
	float tTimer = df->Measurement.tPulse / nSamplesPulse;

	/* Calculamos t interval (t dc + t pulse) */
	float tInt = df->Measurement.step / df->Measurement.sr;

	/* Calculamos n� de samples en la zona DC */
	float nSamplesDC = ceil((tInt - df->Measurement.tPulse) / tTimer);

	/* Calculamos el n� de steps */
	uint32_t nSteps = abs((df->Measurement.stop - df->Measurement.start) / df->Measurement.step);

	uint32_t contRow = 0;

	/* Generamos el patr�n de se�al */
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
	
	/* Establecemos un n� m�nimo de samples en el pulso para evitar aliasing a frecuencias altas */
	uint16_t nSamplesPulse1 = 10;

	/* Tiempo de disparo de cada sample */
	float tTimer = df->Measurement.tPulse1 / nSamplesPulse1;

	/* Calculamos t interval (t dc + t pulse) */
	float tInt = df->Measurement.step / df->Measurement.sr;

	/* Calculamos n� de samples en la zona DC */
	float nSamplesDC = ceil((tInt - (df->Measurement.tPulse1 + df->Measurement.tPulse2 )) / tTimer);

	/* N� de samples en pulse1 */
	float nSamplesP1 = ceil(df->Measurement.tPulse1 / tTimer);

	/* N� de samples en pulse2 */
	float nSamplesP2 = ceil(df->Measurement.tPulse2 / tTimer);

	/* Calculamos el n� de steps */
	uint32_t nSteps = abs((df->Measurement.stop - df->Measurement.start) / df->Measurement.step);

	uint32_t contRow = 0;

	/* Generamos el patr�n de se�al */
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


	/* Calculamos el n� de steps */
	uint32_t nSteps = abs((df->Measurement.start - df->Measurement.stop) / df->Measurement.step);

	/* Hacemos la se�al en dos pasos... */
	/* PASO 1: generamos la onda cuadrada */
	float nSamples1 = (1 / df->Measurement.freq) / tTimer;
	float nSamples2 = (1 / df->Measurement.freq) / tTimer;

	for (i = 0; i < nSamples1; i++) {				// Primer semiper�odo...

		LUT1[i] = df->Measurement.amplitude;
	}

	for (i = 0; i < nSamples2; i++) {				// Segundo semiper�odo...

		LUT2[i] = - (df->Measurement.amplitude);
	}


	/* PASO 2: generamos el DC offset */


	if (df->Measurement.start < df->Measurement.stop) {		// Si steps suben...

		for (i = 0; i < nSteps; i++) {

			for (j = 0; j < nSamples1; j++) {			// Generamos primer semiper�odo m�s parte DC

				LUTcomplete[j + contRow] = (df->Measurement.start + df->Measurement.step * i) + \
					LUT1[j];
			}
			contRow += j;

			for (j = 0; j < nSamples2; j++) {			// Generamos segundo semiper�odo m�s parte DC

				LUTcomplete[j + contRow] = (df->Measurement.start + df->Measurement.step * i) + \
					LUT2[j];
			}
			contRow += j;
		}

	}

	else {												// Si steps bajan...

		for (i = 0; i < nSteps; i++) {

			for (j = 0; j < nSamples1; j++) {			// Generamos primer semiper�odo m�s parte DC

				LUTcomplete[j + contRow] = (df->Measurement.start - df->Measurement.step * i) + \
					LUT1[j];
			}
			contRow += j;

			for (j = 0; j < nSamples2; j++) {			// Generamos segundo semiper�odo m�s parte DC

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

	/* PASO 1 : generaci�n de la senoide para todo el per�odo */
	/* Calculamos n� per�odos AC que caben en t interval */
	float nPerAC = tInt / (1 / df->Measurement.freq);

	/* Esto nos da el n� de ptos totales del t interval */
	float nSamplesTint = ceil(nPerAC * nSamplesAC);

	/* Generamos la senoidal para todo el t int */
	/* IMPORTANTE */
	/* En caso de que el n� de per�odos AC que caben en el t itnerval no sea
	 un n�mero entero, debemos de poder sacar los puntos del per�odo incompleto.
	 Esto se hace en la segunda parte del "if" */

	uint32_t contRow = 0;

	for (i = 0; i < ceil(nPerAC); i++) {

		if (i < nPerAC) {						// Si no es el �ltimo per�odo...
			for (j = 0; j < nSamplesAC; j++) {

				LUT1[j + contRow] = df->Measurement.ACamplitude * sin((2 * PI / nSamplesAC)*j);
			}
			contRow += j;
		}
		else {									// Si �ltimo per�odo...

			float nSamplesRest = nSamplesTint - contRow;

			for (j = 0; j < nSamplesRest; j++) {

				LUT1[j + contRow] = df->Measurement.ACamplitude * sin((2 * PI / nSamplesAC)*j);
			}
		}

	}

	/* Generaci�n del offset + AC para toda la prueba */
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



/* FUNCION EN PRUEBAS PARA GENERAR LUT PERIODICA */
void generateDPVwaveform(DF_DPVTypeDef* df, float* LUT){

	uint32_t i,j;
	
	uint32_t fSampling = 10000;		// TODO: este valor depender� del filtro seleccionado.
																// Hemos seleccionado aqu� la frec para un filtro de butterworth del equipo de CEMITEC.	
																// La idea es que esta frecuencia sea de aprox 20x la frec de corte del filtro.
																// Hay que enviar el valor del filtro seleccionado a esta funci�n como argumento.
	
	float tSampling = 1 / fSampling;
	float tInt = df->Measurement.step / df->Measurement.sr;
	
	uint32_t nSamples1 = ceil((tInt - df->Measurement.tPulse) / tSampling);
	uint32_t nSamples2 = ceil((df->Measurement.tPulse / tSampling));
	
	
	// Calculamos n� de steps
	uint16_t nSteps = ceil(abs(df->Measurement.stop - df->Measurement.start) / df->Measurement.step);
	
	
	if (df->Measurement.stop > df->Measurement.start){			// Si steps suben....
		
		// Primera parte de la onda...
		for(j = 0; j < nSamples1; j++){
			LUT[j] = df->Measurement.start + (df->Measurement.step * (df->Measurement.realStep));
		
		}
		
		// Segunda parte de la onda...
		for(j = 0; j < nSamples2; j++){
			LUT[j + nSamples1] = (df->Measurement.start + df->Measurement.ePulse) + (df->Measurement.step * df->Measurement.realStep);
		
		}
		df->Measurement.realStep++;				// Nos sirve para llevar cuenta de en qu� step estamos generando la forma de onda. Comienza en cero.
	}
	
	else {																									// Si steps bajan...
	
		// Primera parte de la onda...
		for(j = 0; j < nSamples1; j++){
			LUT[j] = df->Measurement.start - (df->Measurement.step * (df->Measurement.realStep));
		
		}
		
		// Segunda parte de la onda...
		for(j = 0; j < nSamples2; j++){
			LUT[j + nSamples1] = (df->Measurement.start + df->Measurement.ePulse) - (df->Measurement.step * df->Measurement.realStep);
		
		}
		df->Measurement.realStep++;
	}
	
}
	
	
	
	
void load_data(uint8_t* buff, DF_CVTypeDef* df_cv, DF_LSVTypeDef* df_lsv, DF_SCVTypeDef* df_scv, \
	DF_DPVTypeDef* df_dpv, DF_NPVTypeDef* df_npv, DF_DNPVTypeDef* df_dnpv, DF_SWVTypeDef* df_swv, DF_ACTypeDef* df_acv, pretreat_t* p,\
	exp_config_t* e){
	
	/* Recogemos datos pretratamiento */
	p->tCond = (int)(((( ((buff[10] << 8) | (buff[11] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	p->eCond = (int)(((( ((buff[12] << 8) | (buff[13] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	p->tDep = (int)(((( ((buff[14] << 8) | (buff[15] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	p->eDep = (int)(((( ((buff[16] << 8) | (buff[17] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	p->tEq = (int)(((( ((buff[18] << 8) | (buff[19] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	p->eEq = (int)(((( ((buff[22] << 8) | (buff[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
		
	/* Recogemos otros datos del experimento */
	e->bipot = buff[3];
	e->cell_on = buff[9];
	e->exp = buff[5];
	
	
	/* en funci�n de la t�cnica guardamos los datos del experimento en la estructura correspondiente */
	switch(e->exp){
		case 0:													// CV
			load_CV_data(df_cv, buff);
			break;
		
		case 1:													// LSV
			load_LSV_data(df_lsv, buff);
			break;
		
		case 2:													// SCV
			load_SCV_data(df_scv, buff);
			break;
		
		case 3:													// DPV
			load_DPV_data(df_dpv, buff);
			break;
			
		case 4:													// NPV
			load_NPV_data(df_npv, buff);
			break;
		
		case 5:													// DNPV
			load_DNPV_data(df_dnpv, buff);
			break;
		
		case 6:													// SWV
			load_SWV_data(df_swv, buff);
			break;
		
		case 7:													// ACV
			load_ACV_data(df_acv, buff);
			break;
	
	
	
	}

}
	

void generate_data(DF_CVTypeDef* df_cv, DF_LSVTypeDef* df_lsv, DF_SCVTypeDef* df_scv, \
	DF_DPVTypeDef* df_dpv, DF_NPVTypeDef* df_npv, DF_DNPVTypeDef* df_dnpv, DF_SWVTypeDef* df_swv, DF_ACTypeDef* df_acv,\
	exp_config_t* e, float* lut1, float* lut2){

	/* En funci�n de la t�cnica guardamos los datos del experimento en la estructura correspondiente */
	switch(e->exp){
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
			generateDPVwaveform(df_dpv, lut1);
			generateDPVwaveform(df_dpv, lut2);
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
void load_CV_data(DF_CVTypeDef* df, uint8_t* cmd){
								
	/* Experiment values */
	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.vtx1 = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.vtx2 = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.step = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.sr = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.scans = ((cmd[32] << 8) | (cmd[33] & 0xFF));

					
}


void load_LSV_data(DF_LSVTypeDef* df, uint8_t* cmd){

	/* Experiment values */
	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.stop = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.step = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.sr = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	

}



void load_SCV_data(DF_SCVTypeDef* df, uint8_t* cmd){

	/* Experiment values */
	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.stop = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.step = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.tHold = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.sr = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.scans = ((cmd[32] << 8) | (cmd[33] & 0xFF));	

}

void load_DPV_data(DF_DPVTypeDef* df, uint8_t* cmd){
					
	/* Experiment values */
	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.stop = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.step = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.ePulse = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.tPulse = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.sr = (int)(((( ((cmd[32] << 8) | (cmd[33] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
		
	df->Measurement.realStep = 0;

}

void load_NPV_data(DF_NPVTypeDef* df, uint8_t* cmd){
				
	/* Experiment values */
	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.stop = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.step = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.tPulse = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.sr = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;

}


void load_DNPV_data(DF_DNPVTypeDef* df, uint8_t* cmd){

	/* Experiment values */
	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.stop = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.step = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.ePulse = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.tPulse1 = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.tPulse2 = (int)(((( ((cmd[32] << 8) | (cmd[33] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;

}


void load_SWV_data(DF_SWVTypeDef* df, uint8_t* cmd){

	/* Experiment values */
	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.stop = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.step = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.amplitude = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.freq = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;


}

void load_ACV_data(DF_ACTypeDef* df, uint8_t* cmd){
					
	/* Experiment values */
	df->Measurement.start = (int)(((( ((cmd[22] << 8) | (cmd[23] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.stop = (int)(((( ((cmd[24] << 8) | (cmd[25] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.step = (int)(((( ((cmd[26] << 8) | (cmd[27] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.ACamplitude = (int)(((( ((cmd[28] << 8) | (cmd[29] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.freq = (int)(((( ((cmd[30] << 8) | (cmd[31] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;
	df->Measurement.sr = (int)(((( ((cmd[32] << 8) | (cmd[33] & 0xFF)) - 32768.0) * VREF) / 32768.0) * 10000.0) / 10000.0;


}



