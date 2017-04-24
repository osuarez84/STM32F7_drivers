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
	=> USART (bluetooth)

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


---------------------------------
USART
---------------------------------
El c�lculo para el baudrate utilizando el HSE de 25MHz como clk del sistema:
	=> Revisar p�gina 1223 del reference manual del STM32F7.

* Para obtener 9600 baudios a 25MHz y con OVER8 = 1
	USARTDIV = 25 000 000 / 9600 = A2Ch
* Para obtener 115200 baudios a 25 MHz con OVER 1 = 
	USARTDIV = 25 000 000 / 115200 = D9h

APIs
	=> UART initialization
	=> UART Transmission
	=> UART Reception
	=> Interrupt handling

