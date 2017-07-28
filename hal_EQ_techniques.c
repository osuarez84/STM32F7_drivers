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
//static uint32_t concatenateLUTs(float* lut1, float* lut2, float* lut3, float* lutC, uint32_t n1, uint32_t n2, uint32_t n3) {

//	uint32_t i;
//	
//	n1--;																// El último pto de la LUT1 es igual al primero de la LUT2
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

//	/* Escribimos el Estop en el último valor de la LUT */
//	LUTcomplete[lengthLUT-1] = df->stop;

//	return lengthLUT;
//}


/* #######################################################
   #	VOLTAMMETRIES
   ####################################################### */

/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the SCV
* @param	None
* @retval	None
*/
// ANOTACIONES
// El algorítmo de generación de esta señal cambia un poco dependiendo de si la onda comienza
// subiendo o si la onda comiena bajando. Como hay que controlar posteriormente las subidas y 
// bajadas para ver si hemos llegado a los límites, se utiliza el flagSube. De esta manera
// si la señal comienza subiendo va a oscilar entre start - stop - start - stop - ...
// Si la señal comienza bajando : stop - start - stop - start - ...
// Este flag controla esas subidas y bajadas ya que el contador de steps aumenta y disminuye según 
// hay que subir o bajar al llegar al límite.
// OJO : acordarse de poner a 1 o 0 el flagSube antes de llamar a esta función!!

static void generateSCV(exp_param_values_t* e, uint16_t* LUT){
	
	uint16_t i;
	
	for(i = 0; i < NSAMPLESLUT; i++){
	
		if(e->upwardsStep){										// Steps comienzan subiendo...
			
			LUT[i] = e->runTime.start + (e->runTime.step * e->contStep);
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer == e->nSamplesPer){			// Hemos terminado el período..??
				if(e->flagSube){
					e->contStep++;													// SI: si estamos subiendo aumentamos un step
				}
				else{																			// SI: si estamos bajando disminuimos un step
					e->contStep--;
				}
				
				e->contSamplesPer = 0;										// Reseteamos el contador para el nuevo período
				
				if(e->contStep > e->nSteps){							// Hemos llegado al límite??
					e->flagSube = 0;												// Cambiamos la dirección 
					e->contStep -= 2;
				}
				else if(e->contStep == 0){								// Hemos llegado al otro límite??
					e->flagSube = 1;												// Cambiamos la dirección
				}		
			}
		}
		
		else{																	// Steps comienzan bajando....(estructura similar a la parte de subida)
			LUT[i] = e->runTime.start - (e->runTime.step * e->contStep);
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer == e->nSamplesPer){
			
				if(e->flagSube){
					e->contStep--;
				}
				else{
					e->contStep++;
				}
				
				e->contSamplesPer = 0;
				
				if(e->contStep == 0){
					e->flagSube = 0;
				}
				else if(e->contStep > e->nSteps){
					e->flagSube = 1;
					e->contStep -= 2;
				}
			}
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the DPV
* @param	None
* @retval	None
*/
static void generateDPV(exp_param_values_t* e, uint16_t* LUT){
	
	uint16_t i;
	
	if(e->upwardsStep){										// Steps suben...
		
		for(i = 0; i < NSAMPLESLUT; i++){
			if(e->contSamplesPer < e->nSamples1){									// Estamos en primera parte de la onda (DC)
			
				LUT[i] = e->runTime.start + (e->runTime.step * e->contStep);
			}
			
			else if((e->contSamplesPer - e->nSamples1) < e->nSamples2){		// Estamos en segunda parte de la onda (pulse)
			
				LUT[i] = (e->runTime.start + (e->runTime.step * e->contStep)) + e->runTime.ePulse1;
			}
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer == e->nSamplesPer){													// Hemos terminado de sacar un período completo...?
			
				e->contSamplesPer = 0;																				// Reseteamos contadores y subimosr un step el siguiente período...
				e->contStep++;
			}
		}
	}
	
	else{ 																								// Steps bajan...
		
		for(i = 0; i < NSAMPLESLUT; i++){
			if(e->contSamplesPer < e->nSamples1){									// Estamos en primera parte de la onda (DC)
			
				LUT[i] = e->runTime.start - (e->runTime.step * e->contStep);
			}
			
			else if((e->contSamplesPer - e->nSamples1) <= e->nSamples2){		// Estamos en segunda parte de la onda (pulse)
			
				LUT[i] = (e->runTime.start - (e->runTime.step * e->contStep)) + e->runTime.ePulse1;
			}
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer == e->nSamplesPer){													// Hemos terminado de sacar un período completo...?
			
				e->contSamplesPer = 0;																				// Reseteamos contadores y subimosr un step el siguiente período...
				e->contStep++;
			}
		
		}
	}
}

/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the NPV
* @param	None
* @retval	None
*/
static void generateNPV(exp_param_values_t* e, uint16_t* LUT){

	uint16_t i;
	
	if(e->upwardsStep){																						// Steps suben...
		for(i = 0; i < NSAMPLESLUT; i++){
			if(e->contSamplesPer < e->nSamples1){														// Primera parte de la onda
				LUT[i] = e->runTime.start;
			}
			
			else if((e->contSamplesPer - e->nSamples1) <= e->nSamples2){			// Segunda parte de la onda
			
				LUT[i] = (e->runTime.start + (e->runTime.step * e->contStep));
			}
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer == e->nSamplesPer){														// Hemos acabado un período..?
				e->contSamplesPer = 0;														// Reseteamos el contador de samples por período
				e->contStep++;																		// Aumentamos un step
			}
		
		}
	
	
	}
	
	else{																													// Steps bajan...
		for (i = 0; i < NSAMPLESLUT; i++) {
				if (e->contSamplesPer < e->nSamples1) {													// Primera parte de la onda

					LUT[i] = e->runTime.start;
				}

				else if ((e->contSamplesPer - e->nSamples1) <= e->nSamples2) {		// Segunda parte de la onda

					LUT[i] = (e->runTime.start - (e->runTime.step * e->contStep));
				}

				e->contSamplesPer++;

				if (e->contSamplesPer == e->nSamplesPer) {												// Hemos acabado un período...?

					e->contSamplesPer = 0;
					e->contStep++;
				}
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the DNPV
* @param	None
* @retval	None
*/
static void generateDNPV(exp_param_values_t* e, uint16_t* LUT){

	uint32_t i;

	if (e->upwardsStep) {																// Steps suben
		for (i = 0; i < NSAMPLESLUT; i++) {

			if (e->contSamplesPer < e->nSamples1) {																				// Primera parte onda
				LUT[i] = e->runTime.start;

			}

			else if ((e->contSamplesPer - e->nSamples1) < e->nSamples2) {									// Segunda parte onda
				LUT[i] = e->runTime.start + (e->runTime.step * e->contStep);
			}

			else if((e->contSamplesPer - (e->nSamples1 + e->nSamples2)) < e->nSamples3){		// Tercera parte onda
				LUT[i] = e->runTime.start + (e->runTime.step * e->contStep) + e->runTime.ePulse1;
			}

			e->contSamplesPer++;

			if (e->contSamplesPer == e->nSamplesPer) {							// Hemos terminado un período?
				e->contSamplesPer = 0;															// Reseteamos contador ptos de período
				e->contStep++;																			// Subimos un step
			}
		}
	}

	else {																																	// Steps bajan
		for (i = 0; i < NSAMPLESLUT; i++) {

			if (e->contSamplesPer < e->nSamples1) {																				// Primera parte onda
				LUT[i] = e->runTime.start;

			}

			else if ((e->contSamplesPer - e->nSamples1) < e->nSamples2) {									// Segunda parte onda
				LUT[i] = e->runTime.start - (e->runTime.step * e->contStep);
			}

			else if ((e->contSamplesPer - (e->nSamples1 + e->nSamples2)) < e->nSamples3) {	// Tercera parte onda
				LUT[i] = e->runTime.start - (e->runTime.step * e->contStep) + e->runTime.ePulse1;
			}


			e->contSamplesPer++;

			if (e->contSamplesPer == e->nSamplesPer) {					// Hemos terminado un período?
				e->contSamplesPer = 0;													// Reseteamos contador ptos de período
				e->contStep++;																	// Subimos un step
			}
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the SWV
* @param	None
* @retval	None
*/
static void generateSWV(exp_param_values_t* e, uint16_t* LUT){
	
	uint32_t i;

	if(e->upwardsStep){																													// Steps suben
		for (i = 0; i < NSAMPLESLUT; i++) {
			if (e->contSamplesPer < e->nSamples1) {																					// Primera parte onda
				LUT[i] = (e->runTime.start + (e->runTime.step * e->contStep)) + e->runTime.amplitude;

			}

			else if ((e->contSamplesPer - e->nSamples1) < e->nSamples2) {										// Segunda parte onda
				LUT[i] = (e->runTime.start + (e->runTime.step * e->contStep)) - e->runTime.amplitude;

			}

			e->contSamplesPer++;

			if (e->contSamplesPer == e->nSamplesPer) {					// Hemos terminado un período?
				e->contSamplesPer = 0;													// Reseteamos contador ptos de período
				e->contStep++;																	// Subimos un step

			}
		}

	}

	else {																									// Steps bajan
		for (i = 0; i < NSAMPLESLUT; i++) {
			if (e->contSamplesPer < e->nSamples1) {																				// Primera parte onda
				LUT[i] = (e->runTime.start - (e->runTime.step * e->contStep)) + e->runTime.amplitude;

			}

			else if ((e->contSamplesPer - e->nSamples1) < e->nSamples2) {									// Segunda parte onda
				LUT[i] = (e->runTime.start - (e->runTime.step * e->contStep)) - e->runTime.amplitude;

			}

			e->contSamplesPer++;

			if (e->contSamplesPer == e->nSamplesPer) {		// Hemos terminado un período?
				e->contSamplesPer = 0;										// Reseteamos contador ptos de período
				e->contStep++;														// Subimos un step

			}
		}
	}
}

static void generateACV(exp_param_values_t* e, uint16_t* LUT){
	
	uint32_t i;

	for (i = 0; i < NSAMPLESLUT; i++) {
		if (e->upwardsStep) {						// Steps suben
				
			LUT[i] = (e->runTime.start + (e->runTime.step * e->contStep)) + \
				(e->runTime.ACamplitude * sin((2 * PI / e->nSamplesAC) * e->contSin));		// Solo hay una parte


			e->contSamplesPer++;
			e->contSin++;

			if (e->contSamplesPer == e->nSamplesPer) {
				e->contSamplesPer = 0;
				e->contSin = 0;
				e->contStep++;
			}
		}

		else {									// Steps bajan
			
			LUT[i] = (e->runTime.start - (e->runTime.step * e->contStep)) + \
				(e->runTime.ACamplitude * sin((2 * PI / e->nSamplesAC) * e->contSin));		// Solo hay una parte


			e->contSamplesPer++;
			e->contSin++;

			if (e->contSamplesPer == e->nSamplesPer) {
				e->contSamplesPer = 0;
				e->contSin = 0;
				e->contStep++;
			}
		}
	}
}


/* ************************************************** */
/* 	FUNCIONES PARA PRECARGA DE DATOS  DEL EXPERIMENTO */
/* ************************************************** */

static void load_CV_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + start
	// + vtx1
	// + vtx2
	// + step
	// sr
	// scans
	
	// TODO

}

static void load_LSV_data(exp_param_values_t* e, uint8_t* cmd){
	
	// Parámetros
	// + start
	// + stop
	// + step
	// + sr
	
	// TODO
	
}

static void load_SCV_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + start
	// + stop
	// + step
	// + tHold
	// + sr
	// + scans
	
	float start, stop, step, sr, tSampling, tInt;
	
	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado
	
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.tHold = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.sr = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.scans = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	
	// Guardamos los parámetros convertidos a valores DAC
	e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.step = ceil((((e->Init.step / 1000.0) * 32768.0) / VREF));
	
	// Recupero todos los decimales para los cálculos...
	step = e->Init.step / 1000.0;
	sr = e->Init.sr / 1000.0;
	
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
		
	tSampling = 1 / (float)(e->fSampling);
	tInt = step / sr;
	
	if(stop > start){
		e->upwardsStep = 1;
		e->flagSube = 1;
	}
	else{
		e->upwardsStep = 0;
		e->flagSube = 0;
	}
	
	// Nº de samples
	e->nSamples1 = ceil(tInt / tSampling);
	
	// Nº de steps
	e->nSteps = ceil(fabs(stop - start) / step);
	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1;															// Samples de cada período de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;								// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	
}


static void load_DPV_data(exp_param_values_t* e, uint8_t* cmd){
				
	
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
	
	// Pasamos los parámetros a valores DAC
	e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
	
	// Recupero todos los decimales para los cálculos...
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
	
	// Número de samples para cada tramo de la señal en cada período
	e->nSamples1 = ceil((tInt - tPulse) / tSampling);
	e->nSamples2 = ceil((tPulse / tSampling));
		
	// Calculamos nº de steps
	e->nSteps = ceil(fabs(stop - start) / step);
		
		
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1 + e->nSamples2;				// Samples de cada período de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;								// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;

}


static void load_NPV_data(exp_param_values_t* e, uint8_t* cmd){
	
	float step, sr, tPulse, stop, start, tSampling, tInt;
	
	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado
	
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.tPulse1 = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.sr = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	
	
	// Pasamos los parámetros a valores DAC
	e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	
	// Recupero todos los decimales para los cálculos...
	step = e->Init.step / 1000.0;
	sr = e->Init.sr / 1000.0;
		
	tPulse = e->Init.tPulse1 / 1000.0;
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
		
	tSampling = 1 / (float)e->fSampling;
	tInt = step / sr;
	
	
	// Número de samples para cada tramo de la señal en cada período
	e->nSamples1 = ceil((tInt - tPulse) / tSampling);
	e->nSamples2 = ceil((tPulse / tSampling));
		
	// Calculamos nº de steps
	e->nSteps = ceil(fabs(stop - start) / step);
		
		
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1 + e->nSamples2;				// Samples de cada período de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;								// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
	
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
}


static void load_DNPV_data(exp_param_values_t* e, uint8_t* cmd){
	
	float step, sr, tPulse1, tPulse2, stop, start, tSampling, tInt;
	
	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado
	
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.ePulse1 = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.tPulse1 = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.tPulse2 = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	e->Init.sr = ((cmd[34] << 8) | (cmd[35] & 0xFF));
	
	// Pasamos los parámetros a valores DAC
	e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
	
	
	// Recupero los decimales
	step = e->Init.step / 1000.0;
	sr = e->Init.sr / 1000.0;
	
	tPulse1 = e->Init.tPulse1 / 1000.0;
	tPulse2 = e->Init.tPulse2 / 1000.0;
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
	
	tSampling = 1 / (float)e->fSampling;
	tInt = step / sr;
	
	// Número de samples para cada tramo de la señal en cada período
	e->nSamples1 = ceil((tInt - (tPulse1 + tPulse2)) / tSampling);
	e->nSamples2 = ceil(tPulse1 / tSampling);
	e->nSamples3 = ceil(tPulse2 / tSampling);
		
	// Calculamos nº de steps
	e->nSteps = ceil(fabs(stop - start) / step);
		
		
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1 + e->nSamples2 + e->nSamples3;				// Samples de cada período de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;												// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
	
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;


}

static void load_SWV_data(exp_param_values_t* e, uint8_t* cmd){
	
		float step, period, stop, start, tSampling;
	
	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado
	
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.amplitude = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.freq = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	
	// Pasamos los parámetros a valores DAC
	e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	e->runTime.amplitude = ceil(((e->Init.amplitude / 1000.0) * 32768.0) / VREF);
	
	// Recupero los decimales
	step = e->Init.step / 1000.0;

	period = 1 / (float)e->Init.freq;
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
	
	tSampling = 1 / (float)e->fSampling;

	
	// Número de samples para cada tramo de la señal en cada período
	e->nSamples1 = ceil((period / 2) / tSampling);
	e->nSamples2 = ceil((period / 2) / tSampling);

		
	// Calculamos nº de steps
	e->nSteps = ceil(fabs(stop - start) / step);
		
		
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1 + e->nSamples2;				// Samples de cada período de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSteps * e->nSamplesPer;												// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;


}


static void load_ACV_data(exp_param_values_t* e, uint8_t* cmd){
	
	// Parámetros
	// + start
	// + stop
	// + step
	// + ACamplitude
	// + freq
	// + sr
	
	float start, stop, step, sr, tSampling, tInt;

	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado

	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.ACamplitude = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.freq = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.sr = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	
	// Pasamos los parámetros a valores DAC
	e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	e->runTime.ACamplitude = ceil(((e->Init.ACamplitude / 1000.0) * 32768.0) / VREF);
	
	
	// Recupero los decimales
	step = e->Init.step / 1000.0;
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
	sr = e->Init.sr / 1000.0;
	
	tInt = step / sr;
	tSampling = 1 / (float)e->fSampling;

	// Calculamos nº de steps
	e->nSteps = ceil(fabs(start - stop) / step);

	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesAC = ceil((1 / e->Init.freq) / (1 / (float)e->fSampling));
	e->nSamples1 = ceil(tInt / tSampling);
	e->nSamplesPer = e->nSamples1;
	e->nSamplesExp = e->nSteps * e->nSamplesPer;
	e->nSamplesLUT = NSAMPLESLUT;
	
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	e->contSamplesPer = 0;
	e->contSin = 0;
	e->contStep = 0;
	
}

/* #######################################################
   #	AMPEROMETRIES
   ####################################################### */

static void generateDSCA(){

}

static void generateAD(){


}


static void generateFA(){


}


static void generatePAD(){

}


static void generateDPA(){


}

static void generateMSA(){


}


/* ************************************************** */
/* 	FUNCIONES PARA PRECARGA DE DATOS  DEL EXPERIMENTO */
/* ************************************************** */

static void load_DSCA_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + ePulse1
	// + tPulse1
	// + ePulse2
	// + tPulse2
	
	float tPulse1, tPulse2, tSampling;

	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado

	/* Experiment values */
	e->Init.ePulse1 = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tPulse1 = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.ePulse2 = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.tPulse2 = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.tInterval = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	
	// Pasamos los parámetros a valores DAC
	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.ePulse2 = ceil((((e->Init.ePulse2 / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.tInterval = e->Init.tInterval / 1000.0;																				// Valor de tiempo de cada medición ADC
	
	tSampling = 1 / (float)e->fSampling;

	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamples1 = ceil(tPulse1 / tSampling);
	e->nSamples2 = ceil(tPulse2 / tSampling);
	e->nSamplesExp = e->nSamples1 + e->nSamples2; 
	e->nSamplesPer = e->nSamplesExp;									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	e->contSamplesPer = 0;
	
}

static void load_AD_data(){

	// TODO
}


static void load_FA_data(){
	
	// TODO
	
}


static void load_PAD_data(){

	// Parámetros
	// + eDC
	// + ePulse1
	// + ePulse2
	// + tPulse1
	// + tPulse2
	
//		float tPulse1, tPulse2, tSampling;

//	// BORRAR
//	e->fSampling = 10000;		// Va a depender del filtro seleccionado

//	/* Experiment values */
//	e->Init.ePulse1 = ((cmd[24] << 8) | (cmd[25] & 0xFF));
//	e->Init.tPulse1 = ((cmd[26] << 8) | (cmd[27] & 0xFF));
//	e->Init.ePulse2 = ((cmd[28] << 8) | (cmd[29] & 0xFF));
//	e->Init.tPulse2 = ((cmd[30] << 8) | (cmd[31] & 0xFF));
//	e->Init.tInterval = ((cmd[32] << 8) | (cmd[33] & 0xFF));
//	
//	// Pasamos los parámetros a valores DAC
//	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
//	e->runTime.ePulse2 = ceil((((e->Init.ePulse2 / 1000.0) * 32768.0) / VREF) + 32768.0);
//	e->runTime.tInterval = e->Init.tInterval / 1000.0;																				// Valor de tiempo de cada medición ADC
//	
//	tSampling = 1 / (float)e->fSampling;

//	/* Inicializamos las variables para llevar la cuenta
//	de los samples que vamos leyendo de la LUT y de los 
//	samples totales del exp	en la ISR */
//	e->nSamples1 = ceil(tPulse1 / tSampling);
//	e->nSamples2 = ceil(tPulse2 / tSampling);
//	e->nSamplesExp = e->nSamples1 + e->nSamples2; 
//	e->nSamplesPer = e->nSamplesExp;									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
//	e->nSamplesLUT = NSAMPLESLUT;
//		
//	// Inicializamos contadores
//	e->contSamplesExp = 0;
//	e->contSamplesLUT = 0;
//	e->contSamplesPer = 0;
	
	
	
}


static void load_DPA_data(){

}

static void load_MSA_data(){


}


/************************************************/
/*										APIs											*/
/************************************************/

// Función para cargar datos recibidos desde el PC en la estructura del experimento correspondiente
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
	
	
	/* en función de la técnica guardamos los datos del experimento en la estructura correspondiente */
	switch(eConfig->exp){
		case 0:													// CV
			//load_CV_data(e, buff);
			break;
		
		case 1:													// LSV
			//load_LSV_data(e, buff);
			break;
		
		case 2:													// SCV
			load_SCV_data(e, buff);
			break;
		
		case 3:													// DPV
			load_DPV_data(e, buff);
			break;
			
		case 4:													// NPV
			load_NPV_data(e, buff);
			break;
		
		case 5:													// DNPV
			load_DNPV_data(e, buff);
			break;
		
		case 6:													// SWV
			load_SWV_data(e, buff);
			break;
		
		case 7:													// ACV
			load_ACV_data(e, buff);
			break;
	
	
	
	}

}
	

void generate_data(exp_param_values_t* e,	exp_config_t* eConfig, uint16_t* lut){

	/* En función de la técnica guardamos los datos del experimento en la estructura correspondiente */
	switch(eConfig->exp){
		case 0:													// CV
			// TODO
			break;

		case 1:													// LSV
			// TODO
			break;
		
		case 2:													// SCV
			generateSCV(e, lut);
			break;
		
		case 3:													// DPV
			generateDPV(e, lut);
			break;
			
		case 4:													// NPV
			generateNPV(e, lut);
			break;
		
		case 5:													// DNPV
			generateDNPV(e, lut);
			break;
		
		case 6:													// SWV
			generateSWV(e, lut);
			break;
		
		case 7:													// ACV
			generateACV(e, lut);
			break;



	}
}




