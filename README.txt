***********************
NOTAS sobre el programa		
***********************

AUTOR : Omar Su�rez
FECHA : 2017.04.20
ESTADO: PENDIENTE
TARJETA: STM32F769I-DISC1



El objetivo del proyecto es preparar varios drivers para los siguientes perif�ricos:

	=> GPIOs
	=> SPIs
	=> TIM6

Al final necesitamos preparar la placa DISC1 para su conexi�n al Prototipo 5 del bioptenciostato.
Para ello este proyecto pretende hacer pruebas con los DACs de que se disponen en dicho prototipo,
que utilizan un bus SPI.

Los pasos ser�an:
	=> Iniciar la t�cnica 
	=> Configurar perif�ricos
	=> Configurar interrupciones
	=> Generar la LUT
	=> Comenzar el env�o de datos al SPI

	=> Lectura de datos de los ADCs (MAS ADELANTE...)
		=> Gesti�n del cambio de escalas en saturaci�n

