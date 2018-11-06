#include <apiTimbre.h>
#define FILENAME_TIMBRES  "timbres.dat"
#define FILENAME_FERIADOS "feriados.dat"

static estructuraHorarios_t timbresEscuela[50];

static estructuraFeriados_t feriadosEscuela[20];

// Caracter personalizado carita feliz :)
const char smile[8] = {
   0b00000000,
   0b00001010,
   0b00001010,
   0b00001010,
   0b00000000,
   0b00010001,
   0b00001110,
   0b00000000,
};

// Caracter personalizado letra e
const char e_char[8] = {
   0b01110,
   0b10000,
   0b10000,
   0b01100,
   0b01000,
   0b10000,
   0b10001,
   0b01110
};

// Caracter personalizado letra r
const char r_char[8] = {
   0b00000,
   0b00000,
   0b00000,
   0b01110,
   0b01000,
   0b01000,
   0b01000,
   0b01000
};

// Caracter personalizado letra i
const char i_char[8] = {
   0b00000,
   0b00100,
   0b00000,
   0b00100,
   0b00100,
   0b00100,
   0b00100,
   0b00110
};

// Caracter personalizado letra c
const char c_char[8] = {
   0b00000,
   0b00000,
   0b00000,
   0b00110,
   0b01000,
   0b01000,
   0b01001,
   0b00110
};

//Temperatura - Termometro
const char tempChar[8] = {
   0b01110,
   0b01010,
   0b01010,
   0b01110,
   0b01110,
   0b10111,
   0b11111,
   0b01110
};

// Humedad - Gota
const char humChar[8] = {
   0b00100,
   0b00100,
   0b01110,
   0b10111,
   0b10111,
   0b10011,
   0b01110,
   0b00000
};

// Viento
const char vieChar[8] = {
   0b00111,
   0b11100,
   0b00000,
   0b00111,
   0b11100,
   0b00000,
   0b00111,
   0b11100
};

// Simbolo grados
const char graChar[8] = {
   0b01110,
   0b01010,
   0b01110,
   0b00000,
   0b00000,
   0b00000,
   0b00000,
   0b00000
};


static void apiConfigLCD(void);



void inicializarEstadoMEF_Timbre ( estadoTimbres_t *estadoTimbre )
{
	uint8_t  nroTimbre = 0;
	uint16_t horaActualFM;

	estadoTimbre->cantidadTotalTimbres = sizeof( timbresEscuela ) / sizeof( estructuraHorarios_t );

	horaActualFM = leerHoraActualFM ();

	while ( horaActualFM > timbresEscuela[nroTimbre].horaTimbreFM && nroTimbre++ < estadoTimbre->cantidadTotalTimbres );
	if ( estadoTimbre->nroCircuito !=  timbresEscuela[nroTimbre].zonaCircuito)
		nroTimbre++;
	if ( nroTimbre > estadoTimbre->cantidadTotalTimbres )
		nroTimbre = 0;

	estadoTimbre->nroTimbre = nroTimbre;
	estadoTimbre->tiempoTimbre = 0;
	estadoTimbre->estadoTimbre = DESACTIVADO;
}



void actualizarEstadoMEF_Timbre ( estadoTimbres_t *estadoTimbre )
{
	uint16_t horaActualFM;

	horaActualFM = leerHoraActualFM ();

	switch ( estadoTimbre->estadoTimbre )
	{
	case DESACTIVADO: if ( !esFinDeSemana () && !esFeriado () )
						  if ( timbresEscuela[estadoTimbre->nroTimbre].horaTimbreFM == horaActualFM ) {
							  if (timbresEscuela[estadoTimbre->nroTimbre].zonaCircuito == estadoTimbre->nroCircuito )
							  {
								  estadoTimbre->estadoTimbre = ACTIVADO;
								  actualizarSalidas (estadoTimbre);
							  }
						  }
	break;
	case ACTIVADO:    if (estadoTimbre->tiempoTimbre++ == timbresEscuela[estadoTimbre->nroTimbre].tiempoDuracion )
					  {
						  estadoTimbre->tiempoTimbre = 0;
						  buscarProximoTimbre(estadoTimbre);
						  estadoTimbre->estadoTimbre = DESACTIVADO;
						  actualizarSalidas (estadoTimbre);
		  	  	  	  }
	break;
	}
}



void buscarProximoTimbre (estadoTimbres_t *estadoTimbre)
{
	uint8_t  nroTimbre = estadoTimbre->nroTimbre;

	nroTimbre++;
	while ( estadoTimbre->nroCircuito !=  timbresEscuela[nroTimbre].zonaCircuito)
		nroTimbre++;
	if ( nroTimbre > estadoTimbre->cantidadTotalTimbres )
		nroTimbre = 0;

	estadoTimbre->nroTimbre = nroTimbre;
}


void actualizarSalidas(estadoTimbres_t *estadoTimbre)
{
	switch( estadoTimbre->nroCircuito )
	{
	case TIMBRE_TEORIA:	if ( estadoTimbre->estadoTimbre == ACTIVADO)
							gpioWrite (SALIDA_TIMBRE_TEORIA, HIGH);
						if ( estadoTimbre->estadoTimbre == DESACTIVADO)
							gpioWrite (SALIDA_TIMBRE_TEORIA, LOW);
	break;
	case TIMBRE_TALLER:	if ( estadoTimbre->estadoTimbre == ACTIVADO)
							gpioWrite (SALIDA_TIMBRE_TALLER, HIGH);
						if ( estadoTimbre->estadoTimbre == DESACTIVADO)
							gpioWrite (SALIDA_TIMBRE_TALLER, LOW);
	break;
	}
}


uint16_t leerHoraActualFM ( void )
{
	rtc_t rtc;
	uint16_t horaActualFM;

	rtcRead ( &rtc );
	horaActualFM = rtc.hour * 100 + rtc.min;

	return horaActualFM;
}


uint8_t leerDiaSemana ( void )
{
	rtc_t rtc;
	uint8_t diaSemana;

	rtcRead ( &rtc );
	diaSemana = rtc.wday;

	return diaSemana;
}

bool_t esFinDeSemana ( void )
{
	uint8_t diaSemana;
	bool_t  esFinDeSemana;

	diaSemana = leerDiaSemana ();

	if ( diaSemana == SABADO || diaSemana == DOMINGO)
		esFinDeSemana = TRUE;
	else
		esFinDeSemana = FALSE;

	return esFinDeSemana;
}


bool_t esFeriado ( void )
{
	uint8_t dia;
	uint8_t mes;
	uint16_t indFer;
	uint16_t cantFer;
	bool_t esFeriado = FALSE;
	rtc_t rtc;

	rtcRead ( &rtc );
	dia = rtc.mday;
	mes = rtc.month;

	cantFer = sizeof (feriadosEscuela) / sizeof ( estructuraFeriados_t);

	for ( indFer = 0 ; indFer < cantFer ; indFer++ )
		if ( dia == feriadosEscuela[indFer].fecha.dia && mes == feriadosEscuela[indFer].fecha.mes )
			esFeriado = TRUE;

	return esFeriado;
}


uint8_t apiConfigHardware( void ) {
	rtc_t rtc;

	boardConfig();
	apiConfigLCD();
	uartConfig(UART_USB, UART_BAUDRATE);
	// Configura la UART GPIO a 9600 Baudios.
	uartConfig(UART_GPIO, 9600);	// UART para BT
	// Configura la UART RS232 a 9600 Baudios.
	uartConfig(UART_232, 9600);		// UART para WiFi
	//adcConfig(ADC_ENABLE);
	spiConfig(SPI0);
	//dht11Config();


	rtc.year  = 2018;
	rtc.month = 11;
	rtc.mday  = 5;
	rtc.wday  = 1;
	rtc.hour  = 7;
	rtc.min   = 44;
	rtc.sec   = 0;
	rtcConfig (&rtc);


//	apiConfigServer();


	return _API_STATE_OK;
}





static void apiConfigLCD(void){

	// Inicializar LCD de 20x4 (caracteres x lineas) con cada caracter de 5x2 pixeles
	inicializarLCD( 20, 4, 5, 8 );


	// Cargar el caracter a CGRAM
	// El primer parámetro es el código del caracter (0 a 7).
	// El segundo es el puntero donde se guarda el bitmap (el array declarado anteriormente)

	lcdCreateChar( 0, tempChar );
	lcdCreateChar( 1, humChar );
	lcdCreateChar( 2, vieChar );

	lcdCreateChar( 3, e_char );
    lcdCreateChar( 4, r_char );
    lcdCreateChar( 5, i_char );
    lcdCreateChar( 6, c_char );

    lcdCreateChar( 7, graChar );



	lcdClear(); 	   // Borrar la pantalla
	lcdGoToXY( 1, 1 ); // Poner cursor en 1, 1
	lcdSendStringRaw( "Bienvenido!!!       " );
	lcdGoToXY( 1, 2 ); // Poner cursor en 1, 2
	lcdSendStringRaw( "Timbre Escolar      " );
	lcdGoToXY( 1, 3 ); // Poner cursor en 1, 3
	lcdSendStringRaw( "ET N" );
	lcdData(7);
	lcdSendStringRaw( "1 OTTO KRAUSE " );
	lcdGoToXY( 1, 4 ); // Poner cursor en 1, 4
	lcdSendStringRaw( "Curso CIAA - INET   " );
	delay(2000);

	lcdClear(); 	   // Borrar la pantalla
	lcdGoToXY( 1, 1 ); // Poner cursor en 1, 1
	lcdSendStringRaw( "Inicializando...    " );
	lcdGoToXY( 1, 2 ); // Poner cursor en 1, 2
	lcdSendStringRaw( "Espere un momento   " );
	delay(2000);

}

void apiActualizarInfoLCD_Detenido( void ){
	lcdClear(); 	   // Borrar la pantalla
	lcdGoToXY( 1, 1 ); // Poner cursor en 1, 3
	lcdSendStringRaw( "Sistema detenido   " );
	lcdGoToXY( 1, 3 ); // Poner cursor en 1, 3
	lcdSendStringRaw( "A  Activar         " );
	lcdGoToXY( 1, 4 ); // Poner cursor en 1, 4
	lcdSendStringRaw( "C  Configurar      " );
}

void apiActualizarInfoLCD(estadoTimbres_t *estadoTimbreTeoria, estadoTimbres_t *estadoTimbreTaller )
{
	uint8_t cadenaHora[20];
	uint8_t cadenaHora1[20];
	uint8_t cadenaHora2[20];
	uint8_t cadenaFecha[20];
	uint8_t pos, hora, min;
	rtc_t   rtc;
	//static bool_t alternarMensaje = 0;
	//static uint8_t contMensaje = 0;
	static uint16_t tiempo = 0;
	static bool_t estadoSegundero = FALSE;
	static estadoLCD_t estadoLCD = HORA_FECHA;

	rtcRead( &rtc );

	if ( !esFinDeSemana () && !esFeriado () ) {

	switch ( estadoLCD ) {
	case HORA_FECHA: tiempo++;
 					 if ( tiempo == 1 ) {

					pos=0;
					cadenaFecha[pos++]  = ( rtc.mday / 10 ) 		+ '0';
					cadenaFecha[pos++]  = ( rtc.mday % 10 )			+ '0';
					cadenaFecha[pos++]  = '/';
					cadenaFecha[pos++]  = ( rtc.month / 10 ) 		+ '0';
					cadenaFecha[pos++]  = ( rtc.month % 10 ) 		+ '0';
					cadenaFecha[pos++]  = '/';
					cadenaFecha[pos++]  = ( rtc.year / 1000 ) 		+ '0';
					cadenaFecha[pos++]  = ( rtc.year % 1000 ) / 100 + '0';
					cadenaFecha[pos++]  = (( rtc.year % 100) / 10) 	+ '0';
					cadenaFecha[pos++]  = ( rtc.year % 10 ) 		+ '0';
					cadenaFecha[pos]   	= '\0';
					pos=0;
					cadenaHora[pos++] 	= ( rtc.hour / 10 ) 		+ '0';
					cadenaHora[pos++] 	= ( rtc.hour % 10 ) 		+ '0';
					cadenaHora[pos++] 	= ':';
					cadenaHora[pos++] 	= ( rtc.min / 10 ) 			+ '0';
					cadenaHora[pos++] 	= ( rtc.min % 10 ) 			+ '0';
				/*	cadenaHora[pos++] 	= ':';
					cadenaHora[pos++] 	= ( rtc.sec / 10 ) 			+ '0';
					cadenaHora[pos++] 	= ( rtc.sec % 10 ) 			+ '0'; */
					cadenaHora[pos]   	= '\0';

					lcdClear(); 	   // Borrar la pantalla
					lcdGoToXY( 1, 1 ); // Poner cursor en 1, 3
					lcdSendStringRaw( "Hora y fecha actual " );
					lcdGoToXY( 1, 3 ); // Poner cursor en 1, 3
					lcdSendStringRaw( "Hora:  " );
					lcdSendStringRaw( cadenaHora );
					lcdGoToXY( 1, 4 ); // Poner cursor en 1, 4
					lcdSendStringRaw( "Fecha: " );
					lcdSendStringRaw( cadenaFecha );
 					 }
 					if ( estadoSegundero == TRUE )
					 {
						lcdGoToXY( 10, 3); // Poner cursor en 1, 3
						lcdSendStringRaw( ":" );
					 }
					 else
					 {
						lcdGoToXY( 10, 3); // Poner cursor en 1, 3
						lcdSendStringRaw( " " );
					 }
					 estadoSegundero ^= TRUE;

					 if ( tiempo > 6) {
						 estadoLCD = PROXIMO_TIMBRE;
						 tiempo = 0;
					 }
	break;

	case PROXIMO_TIMBRE : tiempo++;

						  if ( tiempo == 1 ) {

							  hora = timbresEscuela[estadoTimbreTeoria->nroTimbre].horaTimbreFM / 100;
							  min  = timbresEscuela[estadoTimbreTeoria->nroTimbre].horaTimbreFM % 100;

							  pos = 0;
							  cadenaHora1[pos++] = ( hora / 10 ) + '0';
							  cadenaHora1[pos++] = ( hora % 10 ) + '0';
							  cadenaHora1[pos++] = ':';
							  cadenaHora1[pos++] = ( min / 10 ) 	+ '0';
							  cadenaHora1[pos++] = ( min % 10 ) 	+ '0';
							  cadenaHora1[pos]   = '\0';

							  hora = timbresEscuela[estadoTimbreTaller->nroTimbre].horaTimbreFM / 100;
							  min  = timbresEscuela[estadoTimbreTaller->nroTimbre].horaTimbreFM % 100;

							  pos = 0;
							  cadenaHora2[pos++] = ( hora / 10 ) + '0';
							  cadenaHora2[pos++] = ( hora % 10 ) + '0';
							  cadenaHora2[pos++] = ':';
							  cadenaHora2[pos++] = ( min / 10 ) 	+ '0';
							  cadenaHora2[pos++] = ( min % 10 ) 	+ '0';
							  cadenaHora2[pos]   = '\0';


							  lcdClear(); 	   // Borrar la pantalla
							  lcdGoToXY( 1, 1 ); // Poner cursor en 1, 3
							  lcdSendStringRaw( "Proximos timbres   " );
							  lcdGoToXY( 1, 3 ); // Poner cursor en 1, 3
							  lcdSendStringRaw( "Teoria:  " );
							  lcdSendStringRaw( cadenaHora1 );
							  lcdGoToXY( 1, 4 ); // Poner cursor en 1, 4
							  lcdSendStringRaw( "Taller:  " );
							  lcdSendStringRaw( cadenaHora2 );
						  }

						  if ( tiempo > 6) {
							 estadoLCD = ESTADO_TIMBRE;
							 tiempo = 0;
						  }
	break;

	case ESTADO_TIMBRE  : tiempo++;

						  if ( tiempo == 1 ) {

							  if ( estadoTimbreTeoria->estadoTimbre == ACTIVADO )
							  	  strcpy ( cadenaHora1, "ACTIVADO" );
							  else
								  strcpy ( cadenaHora1, "DESACTIVADO" );

							  if ( estadoTimbreTaller->estadoTimbre == ACTIVADO )
							  	  strcpy ( cadenaHora2, "ACTIVADO" );
							  else
								  strcpy ( cadenaHora2, "DESACTIVADO" );

							  lcdClear(); 	   // Borrar la pantalla
							  lcdGoToXY( 1, 1 ); // Poner cursor en 1, 3
							  lcdSendStringRaw( "Estado timbres   " );
							  lcdGoToXY( 1, 3 ); // Poner cursor en 1, 3
							  lcdSendStringRaw( "Teoria: " );
							  lcdSendStringRaw( cadenaHora1 );
							  lcdGoToXY( 1, 4 ); // Poner cursor en 1, 4
							  lcdSendStringRaw( "Taller: " );
							  lcdSendStringRaw( cadenaHora2 );
						  }

						  if ( tiempo > 6) {
							  estadoLCD = HORA_FECHA;
							  tiempo = 0;
						  }
	}

	}
	else if ( esFinDeSemana () )
	{
		lcdClear(); 	   // Borrar la pantalla
		lcdGoToXY( 1, 1 ); // Poner cursor en 1, 3
		lcdSendStringRaw( "Fin de semana   " );
	}
	else {
		lcdClear(); 	   // Borrar la pantalla
		lcdGoToXY( 1, 1 ); // Poner cursor en 1, 3
		lcdSendStringRaw( "Feriado         " );
	}

}

void leerDatosSD ( void )
{

	static FATFS FileSystem;  	// <-- FatFs work area needed for each volume
	static FIL   File;          // <-- File object needed for each open file
	UINT bytesRead = 0, coma = 0, i, j=0, f;

	BYTE z1[48], z[1024];


		// Monta el sistema de archivos
		if( f_mount( &FileSystem, "", 0 ) != FR_OK ){
			// Se podria poner un mensaje de error si no lo puede montar.
		}


		// FA_CREATE_ALWAYS se sobreescribe el archivo. FA_OPEN_APPEND el nuevo contenido se escribe al final.
		if( f_open( &File, FILENAME_TIMBRES, FA_OPEN_EXISTING | FA_READ ) == FR_OK ){
			// Escribe en el archivo abierto el texto "Hola mundo\n\r".
			f_read( &File, z, 1024, &bytesRead );
			// Una vez que escribio cierra el archivo.
			f_close(&File);
			// Chequea si los bytes escritos son iguales a los que se pidio escribir.
			if( bytesRead == 1024 ){
				// Si salio todo bien prende el LED verde
				gpioWrite( LEDG, ON );
			}
		} else{
			// Si la operacion fallo prende el LED rojo.
			gpioWrite( LEDR, ON );
		}

		i=0;
		f=0;

		while ( z[i] != EOF && i < 516 )
		{
			z1[j] = z[i];
			j++;

			if( z[i] == ',' || z[i] == ';')
			{
				z1[j-1] = 0;
				j=0;
				coma++;

				if ( 1 == coma )
				{
					timbresEscuela[f].horaTimbreFM = strtol (z1, NULL, 10);
				}
				if (coma == 2)
				{
					timbresEscuela[f].zonaCircuito = strtol (z1, NULL, 10);
				}
				if (coma == 3)
				{
					timbresEscuela[f].tiempoDuracion = strtol (z1, NULL, 10);
					coma=0;
				}
			}

			if ( z[i] == 13 || z[i] == 10)
			{
				z1[j-1] = 0;
				j = 0;
				strcpy( timbresEscuela[f].textoTimbre, z1);
				f++;
				if(z[i] == 10) i++;
				if(z[i] == 13) i++;
			}

			i++;
		}


		// FA_CREATE_ALWAYS se sobreescribe el archivo. FA_OPEN_APPEND el nuevo contenido se escribe al final.
				if( f_open( &File, FILENAME_FERIADOS, FA_OPEN_EXISTING | FA_READ ) == FR_OK ){
					// Escribe en el archivo abierto el texto "Hola mundo\n\r".
					f_read( &File, z, 1024, &bytesRead );
					// Una vez que escribio cierra el archivo.
					f_close(&File);
					// Chequea si los bytes escritos son iguales a los que se pidio escribir.
					if( bytesRead == 1024 ){
						// Si salio todo bien prende el LED verde
						gpioWrite( LEDG, ON );
					}
				} else{
					// Si la operacion fallo prende el LED rojo.
					gpioWrite( LEDR, ON );
				}

				i=0;
				f=0;
				j=0;
				coma = 0;

				while ( z[i] != EOF && i < 516 )
				{
					z1[j] = z[i];
					j++;

					if( z[i] == ',' || z[i] == ';')
					{
						z1[j-1] = 0;
						j=0;
						coma++;

						if ( 1 == coma )
						{
							feriadosEscuela[f].fecha.dia = strtol (z1, NULL, 10);
						}
						if (coma == 2)
						{
							feriadosEscuela[f].fecha.mes = strtol (z1, NULL, 10);
						}
					}

					if ( z[i] == 13 || z[i] == 10)
					{
						z1[j-1] = 0;
						j = 0;
						strcpy( feriadosEscuela[f].descripcion, z1 );
						f++;
						if(z[i] == 10) i++;
						if(z[i] == 13) i++;
					}

					i++;
				}

}
