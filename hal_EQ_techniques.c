#include "hal_EQ_techniques.h"
#include "led.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++
// Formulas para el cálculo de los valores DAC:
// 	+ DAC bipotentiostat
//		DatoDAC = ((Vanalog + 5) * (2^20 - 1)) / 10;
//
//
// 	+ DAC galvanostat
//		1. FS +/- 500nA
//			DatoDAC = (((Ianalog * 10^7) + 5) * (2^20 - 1)) / 10
//
//		2. FS +/- 500uA
//			DatoDAC = (((Ianalog * 10^4) + 5) * (2^20 - 1)) / 10
//
//		3. FS +/- 100mA 
//			DatoDAC = (((Ianalog * 10) + 5) * (2^20 - 1)) / 10
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++


/************************************************/
/*				Helper functions				*/
/************************************************/

/* #######################################################
   #	VOLTAMMETRIES
   ####################################################### */


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the CV
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
* @retval	None
*/
static void generateCV(exp_param_values_t* e, uint16_t* LUT){

	// TODO :  se tiene que llevar cuenta de los scans ya que debe de enviarsele
	// al PC el número de scan al que pertenece cada dato.
	
	// =================================================================================
	// Este algorítmo divide la generación de la CV en 2 partes y a su vez
	// la segunda parte se divide en A y B.
	// PRIMERA PARTE
	//
	// Va desde eStart hasta eVtx1. Se comprueba si esta es de subida o de bajada y 
	// se calculan los steps que hay entre eStart y eVtx1.
	//
	//
	// SEGUNDA PARTE
	//
	// A) Va desde eVtx1 a eVtx2. Se comprueba si ésta es de subida o de bajada y se
	// calculan los steps de nuevo. 
	// B) Va desde eVtx2 a eVtx1. Similar a la parte A.
	// ==================================================================================
	
	uint16_t i;

	
	for(i = 0; i < NSAMPLESLUT; i++){
			
		if(!e->flagPrimeraParte){														// La primera parte solo se genera al principio... (start => vtx1)
		// Generamos la primeraparte de la onda
		
			if(e->runTime.vtx1 > e->runTime.start){																														// Primera parte sube...
				if(e->contStep > e->nSteps1){										// Hemos llegado al vertex 1...?
					
					e->flagPrimeraParte = 1;											// Hemos terminado de sacar la primera parte de la señal
					e->contStep = 1;															// Inicializamos contStep para comenzar la segunda parte
					if(e->runTime.vtx1 < e->runTime.vtx2){				// activamos el flag para saber si comienza subiendo o no la segunda parte
						e->flagSube = 1;
					}
					else{
						e->flagSube = 0;
					}
					e->parteA = 1;
				}
				
				else{
					LUT[i] = e->runTime.start + (e->contStep * e->runTime.step);
					if(LUT[i] > e->runTime.vtx1){																		// Hemos sobrepasado el valor de vtx1?
						LUT[i] = e->runTime.vtx1;																			// Lo ajustamos
					}
				}	
			}
			
			else{																																															// Primera parte baja...
				if(e->contStep > e->nSteps1){										// Hemos llegado al vertex 1...?
					e->flagPrimeraParte = 1;											// Hemos terminado de sacar la primera parte de la señal
					e->contStep = 1;															// Inicializamos contStep para segunda parte señal
					if(e->runTime.vtx1 < e->runTime.vtx2){				// activamos el flag para saber si comienza subiendo o no la segunda parte
						e->flagSube = 1;
					}
					else{
						e->flagSube = 0;
					}
					e->parteA = 1;
				}

				else{
					LUT[i] = e->runTime.start - (e->contStep * e->runTime.step);
					if(LUT[i] < e->runTime.vtx1){									// Hemos sobrepasado valor de vtx2?
						LUT[i] = e->runTime.vtx1;										// Lo ajustamos
					}
				}
			}
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer == e->nSamplesPer){					// Hemos llegado al final del período (step)
				e->contStep++;
				e->contSamplesPer = 0;
			}			
		}
			
		else{						// vtx1 => vtx2 => vtx1 => vtx2...
			// Generamos escaneos completos y aumentamos contador cada vez que completemos uno
			if(e->parteA){																																				// PARTE A : vtx1 => vtx2
				if(e->flagSube){																												// Steps suben...
					
					if(e->contStep > e->nSteps2){													// Llegamos a vtx2, tenemos que empezar a bajar
						e->flagSube = 0;
						e->contStep = 1;
						e->parteA = 0;
					}
					else{
						LUT[i] = e->runTime.vtx1 + (e->contStep * e->runTime.step);			
						if(LUT[i] > e->runTime.vtx2){							// Hemos sobrepasado valor de vtx2?
							LUT[i] = e->runTime.vtx2;								// Lo ajustamos
						}
					}
				}
				else{																																		// Steps bajan...
					if(e->contStep > e->nSteps2){													// Llegamos a vtx2, tenemos que empezar a subir...
						e->flagSube = 1;
						e->contStep = 1;
						e->parteA = 0;
					}
					else{
						LUT[i] = e->runTime.vtx1 - (e->contStep * e->runTime.step);
						if(LUT[i] < e->runTime.vtx2){							// Hemos sobrepasado valor de vtx2?
							LUT[i] = e->runTime.vtx2;   						// Lo ajustamos
						}
					}
				}
			}
			
			else{																																								// PARTE B : vtx2 => vtx1
				if(e->flagSube){														// Steps suben
					if(e->contStep > e->nSteps2){							// Llegamos a vtx1, tenemos que empezar a bajar
						e->flagSube = 0;
						e->contStep = 1;
						e->parteA = 1;
					}
					
					else{
						LUT[i] = e->runTime.vtx2 + (e->contStep * e->runTime.step);
						if(LUT[i] > e->runTime.vtx1){						// Hemos sobreapsado el valor de vtx1
							LUT[i] = e->runTime.vtx1;							// Lo ajustamos
						}
					}
				}
				
				else{																				// Steps bajan...
					
					if(e->contStep > e->nSteps2){							// Llegamos a vtx2, ahora empezamos a subir 
						e->flagSube = 1;
						e->contStep = 1;
						e->parteA = 1;
					}
					
					else{
						LUT[i] = e->runTime.vtx2 - (e->contStep * e->runTime.step);
						if(LUT[i] < e->runTime.vtx1){						// Hemos sobrepasado valor vtx2?
							LUT[i] = e->runTime.vtx1;							// Ajustamos
						}
					}
				}
			}
	
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer == e->nSamplesPer){				// Hemos llegado al final del período (step)
				e->contStep++;
				e->contSamplesPer = 0;
			}
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the LSV
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
* @retval	None
*/
static void generateLSV(exp_param_values_t* e, uint16_t* LUT){

	uint16_t i;
	
	for(i = 0; i < NSAMPLESLUT; i++){
	
		if(e->contStep <= e->nSteps){																					// Si no hemos llegado al último step...
			
			if(e->upwardsStep){																									// Steps suben...?
				LUT[i] = e->runTime.start + (e->runTime.step * e->contStep);
			}
			else {																															// Steps bajan...?
				LUT[i] = e->runTime.start + (e->runTime.step * e->contStep);
			}
		
			e->contSamplesPer++;																								// Aumentamos contador del período
			
			if(e->contSamplesPer == e->nSamplesPer){														// Hemos terminado el step?
				e->contStep++;																										// Subimos o bajamos otro step
				e->contSamplesPer = 0;																						// Reseteamos contador 
			}
		}
		
	}
	
}




/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the SCV
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
* @retval	None
*/

// El algorítmo de generación de la señal SCV está basado en el mismo
// utilizado para generar la CV.

static void generateSCV(exp_param_values_t* e, uint16_t* LUT){
	
	uint16_t i;
	
	for(i = 0; i < NSAMPLESLUT; i++){
		
		if(e->parteA){																	// PARTE 1: start => stop
			if(e->upwardsStep){												// Steps suben...
				
				if(e->contStep > e->nSteps){						// Hemos llegado a stop, empezamos a bajar...
					e->upwardsStep = 0;
					e->contStep = 1;
					e->parteA = 0;
				}
				
				else{
					LUT[i] = e->runTime.start + (e->contStep * e->runTime.step);
					if(LUT[i] > e->runTime.stop){																		// Hemos sobrepasado valor de stop?
						LUT[i] = e->runTime.stop;																			// Ajustamos
					}
				}
			}
			
			else{																			// Steps bajan...
				
				if(e->contStep > e->nSteps){						// Hemos llegado a stop, empezamos a subir...
					e->upwardsStep = 1;
					e->contStep = 1;
					e->parteA = 0;
				}
				
				else{
					LUT[i] = e->runTime.start - (e->contStep * e->runTime.step);
					if(LUT[i] < e->runTime.stop){																		// Hemos sobrepasado valor de top?
						LUT[i] = e->runTime.stop;																			// Ajustamos
					}
				}
			}
		}
	
		else{																									// PARTE 2 : stop => start
			if(e->upwardsStep){												// Steps suben...
				
				if(e->contStep > e->nSteps){						// Hemos llegado a start, empezamos a bajar...
					e->upwardsStep = 0;
					e->contStep = 1;
					e->parteA = 1;
				
				}
				
				else{
					LUT[i] = e->runTime.stop + (e->contStep * e->runTime.step);
					if(LUT[i] > e->runTime.start){																	// Hemos sobrepasado el valor de start?
						LUT[i] = e->runTime.start;																		// Ajustamos
					}
				}	
			}
			
			else{																			// Steps bajan...
				
				if(e->contStep > e->nSteps){					// Hemos llegado a start, empezamos a subir...
					e->upwardsStep = 1;
					e->contStep = 1;
					e->parteA = 1;
				
				}
				
				else{
					LUT[i] = e->runTime.stop - (e->contStep * e->runTime.step);
					if(LUT[i] < e->runTime.start){																	// Hemos sobrepasado valor de start?
						LUT[i] = e->runTime.start;																		// Ajustamos
					}
				}	
			}
		}
		
		e->contSamplesPer++;
		
		if(e->contSamplesPer == e->nSamplesPer){												// Hemos finalizado un período (step) ?
			e->contStep++;																								// Subimos al siguiente step 
			e->contSamplesPer = 0;																				// Reseteamos contador de samples periodo
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the DPV
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
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
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
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
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
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
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
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

/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the ACV
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
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

/**
* @brief	loads the data for CV technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_CV_data(exp_param_values_t* e, uint8_t* cmd){

	
	// Parámetros
	// + start
	// + vtx1
	// + vtx2
	// + step
	// + sr
	// + scans
	
	// TODO
	float start, step, vtx1, vtx2, sr, tSampling, tInt;
	
	// BORRAR :  va a depender del filtro seleccionado
	e->fSampling = 10000;
	

	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.vtx1 = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.vtx2 = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.step = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.sr = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.scans = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	
	
	// Guardamos los parámetros convertidos a valores DAC
	//e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.step = ceil((((e->Init.step / 1000.0) * 32768.0) / VREF));
	e->runTime.start = ceil((((e->Init.start / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.vtx1 = ceil((((e->Init.vtx1 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.vtx2 = ceil((((e->Init.vtx2 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.step = ceil((((e->Init.step / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	
		// Recupero todos los decimales para los cálculos...
	step = e->Init.step / 1000.0;
	sr = e->Init.sr / 1000.0;
	
	vtx1 = ((int16_t)(e->Init.vtx1)) / 1000.0;
	vtx2 = ((int16_t)(e->Init.vtx2)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
		
	tSampling = 1 / (float)(e->fSampling);
	tInt = step / sr;
	
	// Nº de samples
	e->nSamples1 = ceil(tInt / tSampling);
	
	// Nº de steps
	e->nSteps1 = ceil(fabs(start - vtx2) / step);		// nSteps1 : no. steps in start - vtx1 part
	e->nSteps2 = ceil(fabs(vtx1 - vtx2) / step);		// nSteps2 : no. steps in vtx1 - vtx2 part
	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1;																												// Samples de cada período de LUT (suma samples cada tramo)
	e->nSamplesExp = e->Init.scans * (e->nSamplesPer * (e->nSteps1 + (2*e->nSteps2)));		// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	e->contScansCV = 0;
	

	// Inicializamos flags auxiliares
	e->parteA = 0;
	e->flagPrimeraParte = 0;
}


/**
* @brief	loads the data for LSV technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_LSV_data(exp_param_values_t* e, uint8_t* cmd){
	
	// Parámetros
	// + start
	// + stop
	// + step
	// + sr
	
	float start, stop, step, sr, tSampling, tInt;
	
	// BORRAR :  va a depender del filtro seleccionado
	e->fSampling = 10000;
		
	/* Experiment values */
	e->Init.start = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.stop = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.step = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.sr = ((cmd[28] << 8) | (cmd[29] & 0xFF));

	
	// Guardamos los parámetros convertidos a valores DAC
	//e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.step = ceil((((e->Init.step / 1000.0) * 32768.0) / VREF));
	e->runTime.start = ceil((((e->Init.start / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.step = ceil((((e->Init.step / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	
		// Recupero todos los decimales para los cálculos...
	step = e->Init.step / 1000.0;
	sr = e->Init.sr / 1000.0;
	
	stop = ((int16_t)(e->Init.stop)) / 1000.0;
	start = ((int16_t)(e->Init.start)) / 1000.0;
		
	tSampling = 1 / (float)(e->fSampling);
	tInt = step / sr;
	
	// Nº de samples
	e->nSamples1 = ceil(tInt / tSampling);
	
	// Nº de steps
	e->nSteps = ceil(fabs(start - stop) / step);	

	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesPer = e->nSamples1;																												// Samples de cada período de LUT (suma samples cada tramo)
	e->nSamplesExp = e->nSamplesPer * e->nSteps ;		// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
}


/**
* @brief	loads the data for SCV technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_SCV_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + start
	// + stop
	// + step
	// + tHold		TODO :  tener en cuenta la gestión de este tHold 
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
	//e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.step = ceil((((e->Init.step / 1000.0) * 32768.0) / VREF));
	e->runTime.start = ceil((((e->Init.start / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.step = ceil((((e->Init.step / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
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
	e->nSamplesExp = 2 * e->nSteps * e->nSamplesPer;						// Samples totales del experimento
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesPer = 0;
	e->contStep = 0;
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	
	// Inicializamos flags
	e->parteA = 1;
	
}


/**
* @brief	loads the data for DPV technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
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
	//e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	//e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.start = ceil((((e->Init.start / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.step = ceil((((e->Init.step / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	
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


/**
* @brief	loads the data for NPV technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
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
	//e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	e->runTime.start = ceil((((e->Init.start / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.step = ceil((((e->Init.step / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
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


/**
* @brief	loads the data for DNPV technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
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
	//e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	//e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.start = ceil((((e->Init.start / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.step = ceil((((e->Init.step / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	
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


/**
* @brief	loads the data for SWV technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
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
	//e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	//e->runTime.amplitude = ceil(((e->Init.amplitude / 1000.0) * 32768.0) / VREF);
	e->runTime.start = ceil((((e->Init.start / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
  e->runTime.stop = ceil((((e->Init.stop / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.step = ceil((((e->Init.step / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.amplitude = ceil((((e->Init.amplitude / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
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


/**
* @brief	loads the data for ACV technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
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
	//e->runTime.start = ceil((((e->Init.start / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.stop = ceil((((e->Init.stop / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.step = ceil(((e->Init.step / 1000.0) * 32768.0) / VREF);
	//e->runTime.ACamplitude = ceil(((e->Init.ACamplitude / 1000.0) * 32768.0) / VREF);
	e->runTime.start = ceil((((e->Init.start / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.stop = ceil((((e->Init.stop / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.step = ceil((((e->Init.step / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.ACamplitude = ceil((((e->Init.ACamplitude / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	
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


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the DSCA
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
static void generateDSCA(exp_param_values_t* e, uint16_t* LUT){

	uint32_t i;
	
	for(i = 0; i < NSAMPLESLUT; i++){
		
		if(e->contSamplesPer < e->nSamplesPer){													// Primer pulso de la onda
			LUT[i] = e->runTime.ePulse1;
		}
		
		else if((e->contSamplesPer - e->nSamples1) < e->nSamples2){			// Segundo pulso de la onda
			LUT[i] = e->runTime.ePulse2;
		}
		
		e->contSamplesPer++;
		
		if(e->contSamplesPer == (e->nSamplesPer)){											// Hemos finalizado un período...?
			e->contSamplesPer = 0;
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the ACV
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
static void generateAD(exp_param_values_t* e, uint16_t* LUT){

	// TODO
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the ACV
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
static void generateFA(exp_param_values_t* e, uint16_t* LUT){

	// TODO
}

/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the PAD
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
static void generatePAD(exp_param_values_t* e, uint16_t* LUT){

	uint16_t i;
	
	for(i = 0; i < NSAMPLESLUT; i++){
		
		if(e->contSamplesPer < e->nSamples1){																						// Primera parte de la onda
			LUT[i] = e->runTime.eDC;
		}
		
		else if((e->contSamplesPer - e->nSamples1) < e->nSamples2){											// Segunda parte de la onda
			LUT[i] = e->runTime.ePulse1;
		}
		
		else if((e->contSamplesPer - (e->nSamples1 + e->nSamples2)) < e->nSamples3){		// Tercera parte de la onda
			LUT[i] = e->runTime.ePulse2;
		}
		
		e->contSamplesPer++;
		
		if(e->contSamplesPer == e->nSamplesPer){																				// Hemos finalizado un período...?
			e->contSamplesPer = 0;
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the ACV
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
static void generateDPA(exp_param_values_t* e, uint16_t* LUT){

	uint16_t i;
	
	for(i = 0; i < NSAMPLESLUT; i++){
	
		if(e->contSamplesPer < e->nSamples1){																			// Primera parte de la onda
			LUT[i] = e->runTime.eDC;
		}
		
		else if((e->contSamplesPer -  e->nSamples1) < e->nSamples2){							// Segunda parte de la onda
			LUT[i] = e->runTime.ePulse1;
		}
		
		e->contSamplesPer++;
		
		if(e->contSamplesPer == e->nSamplesPer){																	// Hemos finalizado un período...?
			e->contSamplesPer = 0;
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the MSA
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
// Para generar esta señal mejor enviar un primer mensaje con el nº de steps que 
// se van a lanzar y posteriormente preparar la recepción en el MCU para poder
// coger todos los parámetros de los steps en un segundo mensaje.
// potential : variable para ir guardando el valor de potencial de cada step
// nSamplesx : variable para ir guardando el n de samples de cada step
// potentials[] : array para guardar todos los potenciales de cada step
// samples[] : array para guardar el n de samples de cada step
static void generateMSA(exp_param_values_t* e, uint16_t* LUT){

	uint16_t i;
	
	for(i = 0; i < NSAMPLESLUT; i++){
	
		if(e->contStep < sizeof(e->runTime.potentials)){						// Si no es el último step de la lista...
			LUT[i] = e->runTime.potentials[e->contStep];
			
			e->contSamplesPer++;
			
			if(e->contSamplesPer == e->runTime.samples[e->contStep]){				// Hemos acabado de leer el step actual...?
				e->contStep++;																								// Cargamos el siguiente step
				e->contSamplesPer = 0;																				// Reseteamos el contador de samples de cada step
				
				if(e->contStep == sizeof(e->runTime.potentials)){						// Hemos llegado al último step de la lista?
					e->contStep = 0;																					// Volvemos a repetir el ciclo
				}
			}
		}
	}
}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the ZRA
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
static void generateZRA(exp_param_values_t* e, uint16_t* LUT){


}


/**
* @brief	Each iteration prepare values to fill the 1024 LUT with the HFP
* @param	e : parameters for configuring the waveform
* @param	LUT : array to save the values
*/
static void generateHFP(exp_param_values_t* e, uint16_t* LUT){

	
	
}


/* ************************************************** */
/* 	FUNCIONES PARA PRECARGA DE DATOS  DEL EXPERIMENTO */
/* ************************************************** */

/**
* @brief	loads the data for DSCA technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
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
	//e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.ePulse2 = ceil((((e->Init.ePulse2 / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.ePulse2 = ceil((((e->Init.ePulse2 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;																				// Valor de tiempo de cada medición ADC
	
	tSampling = 1 / (float)e->fSampling;
	
	tPulse1 = e->Init.tPulse1 / 1000.0;
	tPulse2 = e->Init.tPulse2 / 1000.0;

	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamples1 = ceil(tPulse1 / tSampling);
	e->nSamples2 = ceil(tPulse2 / tSampling);
	e->nSamplesPer = e->nSamples1 + e->nSamples2; 
	e->nSamplesExp = e->nSamplesPer;									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	e->contSamplesPer = 0;
	
}

/**
* @brief	loads the data for AD technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_AD_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + eDC
	// + tInterval
	// + tRun
	
	// TODO 
	// OJO tRun puede ser hasta 1 segundo, por lo que al venir multiplicado por 1000 desde el PC puede salirse del rango de un uint8_t!!!
	// Hay que revisar esto más adelante para darle solución: aumentar a 32 bits? 
	
	float tSampling;
	
	tSampling = 1 / (float)e->fSampling;
	
	// BORRAR
	e->fSampling = 10000;
	
	/* Experiment values */
	e->Init.eDC = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.tInterval = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tRun = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	
	// Pasamos los parámetros a valores DAC
	//e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;
	e->runTime.tRun = e->Init.tRun / 1000.0;
	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
}


/**
* @brief	loads the data for FA technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_FA_data(exp_param_values_t* e, uint8_t* cmd){
	
	// Parámetros
	// + eDC
	// + tInterval
	// + tRun
	
	// TODO 
	// OJO tRun puede ser hasta 1 segundo, por lo que al venir multiplicado por 1000 desde el PC puede salirse del rango de un uint8_t!!!
	// Hay que revisar esto más adelante para darle solución: aumentar a 32 bits? 
	
	float tSampling;
	
	tSampling = 1 / (float)e->fSampling;
	
	// BORRAR
	e->fSampling = 10000;
	
	/* Experiment values */
	e->Init.eDC = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.tInterval = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tRun = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	
	// Pasamos los parámetros a valores DAC
	//e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;
	e->runTime.tRun = e->Init.tRun / 1000.0;
	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	
}


/**
* @brief	loads the data for PAD technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_PAD_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + eDC
	// + ePulse1
	// + ePulse2
	// + tPulse1
	// + tPulse2
	// + tInterval
	// + tRun
	
	float tPulse1, tPulse2, tSampling;

	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado

	/* Experiment values */
	e->Init.eDC = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.ePulse1 = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tPulse1 = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.ePulse2 = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.tPulse2 = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.tInterval = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	e->Init.tRun = ((cmd[34] << 8) | (cmd[35] & 0xFF));
	
	// Pasamos los parámetros a valores DAC
	//e->runTime.eDC = ceil((((e->Init.eDC / 1000) * 32768.0) / VREF) + 32768.0);
	//e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.ePulse2 = ceil((((e->Init.ePulse2 / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.ePulse2 = ceil((((e->Init.ePulse2 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;																				// Valor de tiempo de cada medición ADC
	e->runTime.tRun = e->Init.tRun / 1000.0;

	tPulse1 = e->Init.tPulse1 / 1000.0;
	tPulse2 = e->Init.tPulse2 / 1000.0;

	tSampling = 1 / (float)e->fSampling;
	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamples1 = ceil((e->runTime.tInterval - (e->Init.tPulse1 + e->Init.tPulse2)) / tSampling);
	e->nSamples2 = ceil(tPulse1 / tSampling);
	e->nSamples3 = ceil(tPulse2 / tSampling);
	e->nSamplesPer = e->nSamples1 + e->nSamples2 + e->nSamples3; 
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	e->contSamplesPer = 0;
	
	
	
}



/**
* @brief	loads the data for DPA technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_DPA_data(exp_param_values_t* e, uint8_t* cmd){

	// PARAMETROS
	// + eDC
	// + ePulse
	// + tPulse
	// + tInterval
	// + tRun
	
	float tPulse1, tSampling;
	
	// BORRAR
	e->fSampling = 10000;
	
	/* Experiment values */
	e->Init.eDC = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.ePulse1 = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tPulse1 = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.tInterval = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.tRun = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	
	// Pasamos parámetros a valores DAC
	//e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) * 32768.0) / VREF) + 32768.0);
	//e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	e->runTime.ePulse1 = ceil((((e->Init.ePulse1 / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
	
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;
	e->runTime.tRun = e->Init.tRun / 1000.0;
	
	tPulse1 = e->Init.tPulse1 / 1000.0;
	
	tSampling = 1 / (float)e->fSampling;
	
		/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamples1 = ceil((e->runTime.tInterval - tPulse1) / tSampling);
	e->nSamples2 = ceil(tPulse1 / tSampling);
	e->nSamplesPer = e->nSamples1 + e->nSamples2;
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);
	e->nSamplesLUT = NSAMPLESLUT;
	
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	e->contSamplesPer = 0;
	
}


/**
* @brief	loads the data for MSA technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_MSA_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + frSize : frame size => tamaño de la trama que se va a recibir posteiormente con todos los datos de tensiones y tiempos para cada level
	// + Cycles
	// + Levels/steps
	// + tInterval
	// + potentials[]
	// + samples[]
	
	// Hay que definir este flag en el main y ponerlo a 1 donde corresponda.
	if(e->msa_second_frame == 0){					// ¿es el segundo data frame para el MSA con los valores de todos los levels?
		float tInterval;
		
		// BORRAR
		e->fSampling = 10000;
		
		/* Experiment values */
		e->Init.frSize = ((cmd[22] << 8) | (cmd[23] & 0xFF));
		e->Init.levels = ((cmd[24] << 8) | (cmd[25] & 0xFF));
		e->Init.cycles = ((cmd[26] << 8) | (cmd[27] & 0xFF));
		e->Init.tInterval = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	}
	
	else{																// SI, aquí vienen todos los valores para rellenar los arrays
		
		float tSampling;
		uint16_t i;
		
		tSampling = 1 / (float)e->fSampling;
		
		e->runTime.tInterval = e->Init.tInterval / 1000.0;
		
		e->nSamplesExp = 0;
		
		// Loop para generar el los arrays de valores para todos los levels
		for (i = 3; i < e->Init.levels; i++){
			e->Init.potentials[i] = ((cmd[i] << 8) | (cmd[i+1] & 0xFF));
			e->Init.time[i] = ((cmd[i+2] << 8) | (cmd[i+3] & 0xFF));
			
			// Vamos convirtendo a la vez los potenciales a valores DAC
			//e->runTime.potentials[i-3] = ceil((((e->Init.potentials[i-3] / 1000.0) * 32768.0) / VREF) + 32768.0);
			e->runTime.potentials[i-3] = ceil((((e->Init.potentials[i-3] / 1000.0) + 5) * (pow(2,20) - 1)) / 10);
			
			// ... y calculando el nº de samples de cada level
			e->runTime.samples[i-3] = ceil((e->Init.time[i-3] / 1000.0) / tSampling);
			
			// ... y además vamos a inicializar el nº de samples del experimento
			e->nSamplesExp = e->nSamplesExp + e->runTime.samples[i];
		}
		
		/* Inicializamos las variables para llevar la cuenta
		de los samples que vamos leyendo de la LUT y de los 
		samples totales del exp	en la ISR */
		e->nSamplesLUT = NSAMPLESLUT;
		
		// Inicializamos contadores
		e->contSamplesExp = 0;
		e->contSamplesLUT = 0;
		e->contStep = 0;
		
	}
}


/**
* @brief	loads the data for ZRA technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_ZRA_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + OCV => los parámetros de la OCV se van a determinar empíricamente por lo que no se van a introducir desde la interface
	// tRun
	// tInterval
	
	float tSampling;
	
	tSampling = 1 / (float)e->fSampling;
	
	// BORRAR
	e->fSampling = 10000;
	
	/* Experiment values */
	e->Init.tInterval = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.tRun = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;
	e->runTime.tRun = e->Init.tRun / 1000.0;
	
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);
	e->nSamplesLUT = NSAMPLESLUT;
	
	
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	
}


/**
* @brief	loads the data for HCP technique
* @param	e : parameters for configuring the waveform
* @param	cmd : buffer with data from PC
*/
static void load_HFP_data(exp_param_values_t* e, uint8_t* cmd){
	// Parámetros
	// + eDC
	
	e->Init.eDC = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	
	//e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) * 32768.0) / VREF) + 32768.0);
	e->runTime.eDC = ceil((((e->Init.eDC / 1000.0) + 5) * (pow(2,20) - 1)) / 10);

}


/* #######################################################
   #	POTENTIOMETRIES
   ####################################################### */


static void generateDSCP(exp_param_values_t* e, uint16_t* LUT){

	// TODO

}

static void generatePD(exp_param_values_t* e, uint16_t* LUT){
	// TODO

}

static void generateFP(exp_param_values_t* e, uint16_t* LUT){

	// TODO

}

static void generateCRCP(exp_param_values_t* e, uint16_t* LUT){

	// TODO

}

static void generateMSP(exp_param_values_t* e, uint16_t* LUT){

	// TODO
	
}

static void generatePSA(exp_param_values_t* e, uint16_t* LUT){

	// TODO

}

static void generateZCP(exp_param_values_t* e, uint16_t* LUT){

	// TODO

}



/* ************************************************** */
/* 	FUNCIONES PARA PRECARGA DE DATOS  DEL EXPERIMENTO */
/* ************************************************** */	

/**
* @brief	loads the data for PD technique
* @param	e : parameters for configuring the waveform
* @param	e_config : configuration parameters for the experiment
* @param	cmd : buffer with data from PC
*/
static void load_PD_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){

	// Parámetros
	// + iDC
	// + tInterval
	// + tRun
	
	// OJO tRun puede ser hasta 1 segundo, por lo que al venir multiplicado por 1000 desde el PC puede salirse del rango de un uint8_t!!!
	// Hay que revisar esto más adelante para darle solución: aumentar a 32 bits? 
	float tSampling;
	e->fSampling = 10000;
	
	tSampling = 1 / (float)e->fSampling;
	
	// BORRAR
	e->fSampling = 10000;
	
	/* Experiment values */
	e->Init.iDC = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.tInterval = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tRun = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	
	// Pasamos los parámetros a valores DAC según el FS seleccionado
	switch(e_config->rango){
	
		case FS_G_500nA:	
			e->runTime.iDC = ceil(((((e->Init.iDC * pow(10,7)) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		case FS_G_500uA:
			e->runTime.iDC = ceil(((((e->Init.iDC * pow(10,4)) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		case FS_G_100mA:
			e->runTime.iDC = ceil(((((e->Init.iDC * 10) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		default:
			break;
	}
	
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;
	e->runTime.tRun = e->Init.tRun / 1000.0;
	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;

}
	

/**
* @brief	loads the data for FP technique
* @param	e : parameters for configuring the waveform
* @param	e_config : configuration parameters for the experiment
* @param	cmd : buffer with data from PC
*/
static void load_FP_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){

	// Parámetros
	// + iDC
	// + tInterval
	// + tRun
	
	// TODO 
	// OJO tRun puede ser hasta 1 segundo, por lo que al venir multiplicado por 1000 desde el PC puede salirse del rango de un uint8_t!!!
	// Hay que revisar esto más adelante para darle solución: aumentar a 32 bits? 
	
	float tSampling;
	
	tSampling = 1 / (float)e->fSampling;
	
	// BORRAR
	e->fSampling = 10000;
	
	/* Experiment values */
	e->Init.iDC = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.tInterval = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tRun = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	
	// Pasamos los parámetros a valores DAC según el FS seleccionado
		switch(e_config->rango){
	
		case FS_G_500nA:	
			e->runTime.iDC = ceil(((((e->Init.iDC * pow(10,7)) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		case FS_G_500uA:
			e->runTime.iDC = ceil(((((e->Init.iDC * pow(10,4)) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		case FS_G_100mA:
			e->runTime.iDC = ceil(((((e->Init.iDC * 10) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		default:
			break;
	}
	
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;
	e->runTime.tRun = e->Init.tRun / 1000.0;
	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	
}


/**
* @brief	loads the data for DSCP technique
* @param	e : parameters for configuring the waveform
* @param	e_config : configuration parameters for the experiment
* @param	cmd : buffer with data from PC
*/
static void load_DSCP_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){

	// Parámetros
	// + iPulse1
	// + tPulse1
	// + iPulse2
	// + tPulse2
	
	float tPulse1, tPulse2, tSampling;

	// BORRAR
	e->fSampling = 10000;		// Va a depender del filtro seleccionado

	/* Experiment values */
	e->Init.iPulse1 = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tPulse1 = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.iPulse2 = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.tPulse2 = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.tInterval = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	
	// Pasamos los parámetros a valores DAC según el FS seleccionado
	switch(e_config->rango){
	
		case FS_G_500nA:	
			e->runTime.iPulse1 = ceil(((((e->Init.iPulse1 * pow(10,7)) + 5)) * (pow(2,20) - 1)) / 10);
			e->runTime.iPulse2 = ceil(((((e->Init.iPulse2 * pow(10,7)) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		case FS_G_500uA:
			e->runTime.iPulse1 = ceil(((((e->Init.iPulse1 * pow(10,4)) + 5)) * (pow(2,20) - 1)) / 10);
			e->runTime.iPulse2 = ceil(((((e->Init.iPulse2 * pow(10,4)) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		case FS_G_100mA:
			e->runTime.iPulse1 = ceil(((((e->Init.iPulse1 * 10) + 5)) * (pow(2,20) - 1)) / 10);
			e->runTime.iPulse2 = ceil(((((e->Init.iPulse2 * 10) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		default:
			break;
	}
	
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;																				// Valor de tiempo de cada medición ADC
	
	tSampling = 1 / (float)e->fSampling;
	
	tPulse1 = e->Init.tPulse1 / 1000.0;
	tPulse2 = e->Init.tPulse2 / 1000.0;

	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamples1 = ceil(tPulse1 / tSampling);
	e->nSamples2 = ceil(tPulse2 / tSampling);
	e->nSamplesPer = e->nSamples1 + e->nSamples2; 
	e->nSamplesExp = e->nSamplesPer;									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	e->contSamplesPer = 0;

	

}


/**
* @brief	loads the data for CRCP technique
* @param	e : parameters for configuring the waveform
* @param	e_config : configuration parameters for the experiment
* @param	cmd : buffer with data from PC
*/
static void load_CRCP_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){

	// TODO

}
	

/**
* @brief	loads the data for MSP technique
* @param	e : parameters for configuring the waveform
* @param	e_config : configuration parameters for the experiment
* @param	cmd : buffer with data from PC
*/
static void load_MSP_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){

	// TODO
	// Parámetros
	// + frSize : frame size => tamaño de la trama que se va a recibir posteiormente con todos los datos de tensiones y tiempos para cada level
	// + Cycles
	// + Levels/steps
	// + tInterval
	// + currents[]
	// + samples[]
	
	// TODO: Hay que definir este flag en el main y ponerlo a 1 donde corresponda.
	if(e->msa_second_frame == 0){					// ¿es el segundo data frame para el MSA con los valores de todos los levels?
		float tInterval, tSampling;
		
		// BORRAR
		e->fSampling = 10000;
		
		/* Experiment values */
		e->Init.frSize = ((cmd[22] << 8) | (cmd[23] & 0xFF));
		e->Init.levels = ((cmd[24] << 8) | (cmd[25] & 0xFF));
		e->Init.cycles = ((cmd[26] << 8) | (cmd[27] & 0xFF));
		e->Init.tInterval = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	}
	
	else{																// SI, aquí vienen todos los valores para rellenar los arrays
		
		float tSampling;
		uint16_t i;
		
		tSampling = 1 / (float)e->fSampling;
		
		e->runTime.tInterval = e->Init.tInterval / 1000.0;
		
		e->nSamplesExp = 0;
		
		// Loop para generar el los arrays de valores para todos los levels
		for (i = 3; i < e->Init.levels; i++){
			e->Init.currents[i] = ((cmd[i] << 8) | (cmd[i+1] & 0xFF));
			e->Init.time[i] = ((cmd[i+2] << 8) | (cmd[i+3] & 0xFF));
			
			// Vamos convirtendo a la vez los potenciales a valores DAC según el FS seleccionado
			switch(e_config->rango){
	
				case FS_G_500nA:	
					e->runTime.currents[i-3] = ceil(((((e->Init.currents[i-3] * pow(10,7)) + 5)) * (pow(2,20) - 1)) / 10);
					break;
				
				case FS_G_500uA:
					e->runTime.currents[i-3] = ceil(((((e->Init.currents[i-3] * pow(10,4)) + 5)) * (pow(2,20) - 1)) / 10);
					break;
				
				case FS_G_100mA:
					e->runTime.currents[i-3] = ceil(((((e->Init.currents[i-3] * 10) + 5)) * (pow(2,20) - 1)) / 10);
					break;
				
				default:
					break;
			}
			
			// ... y calculando el nº de samples de cada level
			e->runTime.samples[i-3] = ceil((e->Init.time[i-3] / 1000.0) / tSampling);
			
			
			// ... y además vamos a inicializar el nº de samples del experimento
			e->nSamplesExp = e->nSamplesExp + e->runTime.samples[i];
		}
		
		/* Inicializamos las variables para llevar la cuenta
		de los samples que vamos leyendo de la LUT y de los 
		samples totales del exp	en la ISR */
		e->nSamplesLUT = NSAMPLESLUT;
		
		// Inicializamos contadores
		e->contSamplesExp = 0;
		e->contSamplesLUT = 0;
		e->contStep = 0;
		
	}
	
}
	
/**
* @brief	loads the data for PSA technique
* @param	e : parameters for configuring the waveform
* @param	e_config : configuration parameters for the experiment
* @param	cmd : buffer with data from PC
*/	
static void load_PSA_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){

	
	// TODO: todavía hay que revisar la manera en que se va a realizar la finalización 
	// de la técnica ya que esta acaba cuando se llegue a eEnd o cuando tRun finalice,
	// lo que primero ocurra.
	
	// Parámetros
	// + iStrip
	// + eEnd
	// + tInterval
	// + tRun
	
	// TODO 
	// OJO tRun puede ser hasta 1 segundo, por lo que al venir multiplicado por 1000 desde el PC puede salirse del rango de un uint8_t!!!
	// Hay que revisar esto más adelante para darle solución: aumentar a 32 bits? 
	
	float tSampling;
	
	tSampling = 1 / (float)e->fSampling;
	
	// BORRAR
	e->fSampling = 10000;
	
	/* Experiment values */
	e->Init.iStrip = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.eEnd = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	e->Init.tInterval = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.tRun = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	
	// Pasamos los parámetros a valores DAC según el FS seleccionado 
	switch(e_config->rango){
	
		case FS_G_500nA:	
			e->runTime.iStrip = ceil(((((e->Init.iStrip * pow(10,7)) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		case FS_G_500uA:
			e->runTime.iStrip = ceil(((((e->Init.iStrip * pow(10,4)) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		case FS_G_100mA:
			e->runTime.iStrip = ceil(((((e->Init.iStrip * 10) + 5)) * (pow(2,20) - 1)) / 10);
			break;
		
		default:
			break;
	}
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;
	e->runTime.tRun = e->Init.tRun / 1000.0;
	
	/* Inicializamos las variables para llevar la cuenta
	de los samples que vamos leyendo de la LUT y de los 
	samples totales del exp	en la ISR */
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);									// En este caso los samples del "período" son los mismos que los del experimento (ver forma de onda)
	e->nSamplesLUT = NSAMPLESLUT;
		
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
	
}
	

/**
* @brief	loads the data for ZCP technique
* @param	e : parameters for configuring the waveform
* @param	e_config : configuration parameters for the experiment
* @param	cmd : buffer with data from PC
*/
static void load_ZCP_data(exp_param_values_t* e, uint8_t* cmd){

	// Parámetros
	// + OCV => los parámetros de la OCV se van a determinar empíricamente por lo que no se van a introducir desde la interface
	// + tRun
	// + tInterval
	
	float tSampling;
	
	tSampling = 1 / (float)e->fSampling;
	
	// BORRAR
	e->fSampling = 10000;
	
	/* Experiment values */
	e->Init.tInterval = ((cmd[22] << 8) | (cmd[23] & 0xFF));
	e->Init.tRun = ((cmd[24] << 8) | (cmd[25] & 0xFF));
	
	e->runTime.tInterval = e->Init.tInterval / 1000.0;
	e->runTime.tRun = e->Init.tRun / 1000.0;
	
	e->nSamplesExp = ceil(e->runTime.tRun / tSampling);
	e->nSamplesLUT = NSAMPLESLUT;
	
	
	// Inicializamos contadores
	e->contSamplesExp = 0;
	e->contSamplesLUT = 0;
}
	
	

/* #######################################################
   #	EIS
   ####################################################### */
	
	
// TODO

// Carga en un array los 4096 valores de la señal a generar
// Este array se pasa luego en forma de puntero a las APIs de CEMITEC
// para que el DDS genere la frecuencia deseada
static void generatePEIS_FP(exp_param_values_t* e, exp_config_t* e_config, uint16_t* LUT){

	
	uint16_t i = 0;
		
	if(e_config->eis.waveType == MULTISINE){
		// TODO
		
	}
		
	else{
		for(i = 0; i < 4096; i++){							// Rellenamos el array con los valores correspondientes
			//e->Init.eis.amplitude * sin((2 * PI / e->nSamplesAC) * e->contSin)
		}
		
	}
}
	



static void generatePEIS_SP(exp_param_values_t* e, exp_config_t* e_config, uint16_t* LUT){


}


static void generatePEIS_TS(exp_param_values_t* e, exp_config_t* e_config, uint16_t* LUT){

}


static void generateGEIS_FC(exp_param_values_t* e, exp_config_t* e_config, uint16_t* LUT){



}


static void generateGEIS_SC(exp_param_values_t* e, exp_config_t* e_config, uint16_t* LUT){



}

static void generateGEIS_TS(exp_param_values_t* e, exp_config_t* e_config, uint16_t* LUT){



}


/* ************************************************** */
/* 	FUNCIONES PARA PRECARGA DE DATOS  DEL EXPERIMENTO */
/* ************************************************** */	

static void load_PEIS_FP_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){

	// Parámetros
	//		+ DC potential
	//		+ Amplitude
	// 1) Escaneo en frecuencia
	//		+ First applied frequency
	//		+ Last applied frequency
	//		+ Number of frequencies
	//		+ Amplitude
	//		+ Sine/Multisine
	//		+ noSines
	//		+ freq step (linear, log, decade)


	e->Init.eis.DCpotential = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.eis.amplitude = ((cmd[34] << 8) | (cmd[35] & 0xFF));					// Amplitud máxima (tiene que llegar ya convertida desde el PC a este valor)
	
	if(e_config->eis.isScan == 1){			// Si es un escaneo en frecuencias recogeme estos parámetros también
		e->Init.eis.firstFreq = ((cmd[28] << 8) | (cmd[29] & 0xFF));
		e->Init.eis.lastFreq = ((cmd[30] << 8) | (cmd[31] & 0xFF));
		e->Init.eis.nFreq = ((cmd[32] << 8) | (cmd[33] & 0xFF));
	}
	
	
	
	
}


static void load_PEIS_SP_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){

	// Parámetros
	//	+ eStart
	//	+ eStop
	//	+ eStep
	// 1) Escaneo en frecuencia
	//		+ First applied frequency
	//		+ Last applied frequency
	//		+ Number of frequencies
	//		+ Amplitude
	//		+ Sine/Multisine
	//		+ noSines
	//		+ freq step (linear, log, decade)
	
	e->Init.eis.DCstart = ((cmd[26] << 8) | (cmd[27] & 0xFF));
	e->Init.eis.DCstop = ((cmd[28] << 8) | (cmd[29] & 0xFF));
	e->Init.eis.DCstep = ((cmd[30] << 8) | (cmd[31] & 0xFF));
	e->Init.eis.amplitude = ((cmd[38] << 8) | (cmd[39] & 0xFF));
	
	if(e_config->eis.isScan == 1){																					// Es un escaneo de frecuencias?
		e->Init.eis.firstFreq = ((cmd[32] << 8) | (cmd[33] & 0xFF));					// Recogemos además estos otros parametros
		e->Init.eis.lastFreq = ((cmd[34] << 8) | (cmd[35] & 0xFF));
		e->Init.eis.nFreq = ((cmd[36] << 8) | (cmd[37] & 0xFF));
	}

}

static void load_PEIS_TS_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){


}


static void load_GEIS_FC_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){


}

static void load_GEIS_SC_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){


}


static void load_GEIS_TS_data(exp_param_values_t* e, uint8_t* cmd, exp_config_t* e_config){


}








/* ============================================== */
/************************************************/
/*										APIs											*/
/************************************************/
/**
* @brief	function for load the data from PC inside the needed structure
* @param	e : parameters for configuring the waveform
* @param	p : parameters for configuring the pretreatment
* @param	e_config : configuration parameters for the experiment
* @param	cmd : buffer with data from PC
*/
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
	
	// Si nos llega técnica EIS cargamos además estos parámetros de configuración...
	if(eConfig->exp == 23 | eConfig->exp == 24 | eConfig->exp == 25 | eConfig->exp == 26 | eConfig->exp == 27 | eConfig->exp == 28){
		
		if(buff[25] == 1){				// isScan == 1??; es un escaneo?
			
			eConfig->eis.isScan = 1;								// Se lo indicamos a la estructura 
			
			if(buff[22] == 1){											// Vemos si es multisine o no y ponemos la variable al valor...
				eConfig->eis.waveType = MULTISINE;

				eConfig->eis.noSines = buff[24];			// Guardamos el número de sines que se utilizan para formar la multisine
			}
			else{
				eConfig->eis.waveType = SINE;
			}
			
			if(buff[23] == 0){											// Vemos el tipo de step que se aplica a las frecuencias
				eConfig->eis.freqStep = LINEAR;
			}
			else if(buff[23] == 1){
				eConfig->eis.freqStep = LOG;
			}
			else if (buff[23] == 2){
				eConfig->eis.freqStep = DECADE;
			}
		
		}
	}
	
	
	// Configuramos el FS según vayamos a utilizar potenciostato o galvanostato
	switch(eConfig->exp){																
		case 0:												// Si vamos a utilizar potenciostato (voltametrías, amperometrías y PEIS)...
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			if(eConfig->rango == 0){
				eConfig->rango = FS_B_5mA;
			}
			else if(eConfig->rango == 1){
				eConfig->rango = FS_B_50uA;
			}
			else if(eConfig->rango == 2){
				eConfig->rango = FS_B_500nA;
			}
			else if(eConfig->rango == 3){
				eConfig->rango = FS_B_5_100_mA;
			}
			break;
			
		case 16:										// Si vamos a utilizar galvanostato (potenciometrías y GEIS)...
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			if(eConfig->rango == 0){
				eConfig->rango = FS_G_500nA;
			}
			else if(eConfig->rango == 1){
				eConfig->rango = FS_G_500uA;
			}
			else if(eConfig->rango == 2){
				eConfig->rango = FS_G_100mA;
			}
			
			// TODO: falta añadir las técnicas de EIS a estos cases
			
	}
	
	
	/* en función de la técnica guardamos los datos del experimento en la estructura correspondiente */
	switch(eConfig->exp){
		/* **********************************
		* VOLTAMMETRIES
		************************************ */
		case 0:													// CV
			load_CV_data(e, buff);
			break;
		
		case 1:													// LSV
			load_LSV_data(e, buff);
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
		
		/* **********************************
		* AMPEROMETRIES
		************************************ */
		case 8:													// AD
			load_AD_data(e, buff);
			break;
		
		case 9:													// FA
			load_FA_data(e, buff);
			break;
		
		case 10:												// DSCA
			load_DSCA_data(e, buff);
			break;
		
		case 11:												// PAD
			load_PAD_data(e, buff);
			break;
		
		case 12:												// DPA
			load_DPA_data(e, buff);
			break;
		
		case 13:												// MSA
			load_MSA_data(e, buff);
			break;
		
		case 14:												// ZRA
			load_ZRA_data(e, buff);
			break;
		
		case 15:												// HFP
			load_HFP_data(e, buff);
			break;
		
		/* **********************************
		* POTENTIOMETRIES
		************************************ */
		case 16:												// PD
			load_PD_data(e, buff, eConfig);
			break;
		
		case 17:												// FP
			load_FP_data(e, buff, eConfig);
			break;
		
		case 18:												// DSCP
			load_DSCP_data(e, buff, eConfig);
			break;
		
		case 19: 												// CRCP
			// TODO
			break;
		
		case 20:												// MSP
			load_MSP_data(e, buff, eConfig);
			break;
		
		case 21:												// PSA
			load_PSA_data(e, buff, eConfig);
			break;
		
		case 22:												// ZCP
			load_ZCP_data(e, buff);
			break;
	
		/* **********************************
		* EIS
		************************************ */
		case 23:																	// PEIS-FixedPotential
			load_PEIS_FP_data(e, buff, eConfig);
			break;
		
		case 24:																	// PEIS-ScanPotential
			load_PEIS_SP_data(e, buff, eConfig);
			break;
		
		case 25:																	// PEIS-TimeScan
			load_PEIS_TS_data(e, buff, eConfig);
			break;
		
		case 26:																	// GEIS-FixedPotential
			load_GEIS_FC_data(e, buff, eConfig);
			break;
		
		case 27:																	// GEIS-ScanPotential
			load_GEIS_SC_data(e, buff, eConfig);
			break;
		
		case 28:																	// GEIS-TimeScan
			load_GEIS_TS_data(e, buff, eConfig);
			break;
	}
}
	

/**
* @brief	function for generate the data from loaded parameters
* @param	e : parameters for configuring the waveform
* @param	e_config : configuration parameters for the experiment
* @param	lut : pointer to an array where wave is loaded
*/
void generate_data(exp_param_values_t* e,	exp_config_t* eConfig, uint16_t* lut){

	/* En función de la técnica guardamos los datos del experimento en la estructura correspondiente */
	switch(eConfig->exp){
		
		/* **********************************
		* VOLTAMMETRIES
		************************************ */
		case 0:													// CV
			generateCV(e, lut);
			break;

		case 1:													// LSV
			generateLSV(e, lut);
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
		
		/* **********************************
		* AMPEROMETRIES
		************************************ */
		case 8:													// AD
			generateAD(e, lut);
			break;
		
		case 9:													// FA
			generateFA(e, lut);
			break;
		
		case 10:												// DSCA
			generateDSCA(e, lut);
			break;
		
		case 11:												// PAD
			generatePAD(e, lut);
			break;
		
		case 12:												// DPA
			generateDPA(e, lut);
			break;
		
		case 13:												// MSA
			generateMSA(e, lut);
			break;
		
		case 14:												// ZRA
			generateZRA(e, lut);
			break;
		
		case 15:												// HFP
			generateHFP(e, lut);
			break;
		
		/* **********************************
		* POTENTIOMETRIES
		************************************ */
		case 16:												// PD
			generatePD(e, lut);
			break;
		
		case 17:												// FP
			generateFP(e, lut);
			break;
		
		case 18:												// DSCP
			generateDSCP(e, lut);
			break;
		
		case 19: 												// CRCP
			// TODO
			break;
		
		case 20:												// MSP
			generateMSP(e, lut);
			break;
		
		case 21:												// PSA
			generatePSA(e, lut);
			break;
		
		case 22:												// ZCP
			generateZCP(e, lut);
			break;
		
		
		/* **********************************
		* EIS
		************************************ */
		case 23:																	// PEIS-FixedPotential
			generatePEIS_FP(e, eConfig, lut);
			break;
		
		case 24:																	// PEIS-ScanPotential
			generatePEIS_SP(e, eConfig, lut);
			break;
		
		case 25:																	// PEIS-TimeScan
			generatePEIS_TS(e, eConfig, lut);
			break;
		
		case 26:																	// GEIS-FixedPotential
			generateGEIS_FC(e, eConfig, lut);
			break;
		
		case 27:																	// GEIS-ScanPotential
			generateGEIS_SC(e, eConfig, lut);
			break;
		
		case 28:																	// GEIS-TimeScan
			generateGEIS_TS(e, eConfig, lut);
			break;
		

	}
}


