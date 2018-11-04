/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

//#include "cooperative_os_scheduler_03_seos_Pont2014.h"   // <= own header (optional)
#include "sapi.h"        // <= sAPI header
#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header
#include "apiTimbre.h"
/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/
static estadoTimbres_t estadoTimbreTeoria;
static estadoTimbres_t estadoTimbreTaller;
// Teclado
keypad_t keypad;
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

void tarea1( void* ptr );
void tarea2( void* ptr );
void tarea3( void* ptr );
void tarea4( void* ptr );
static void apiConfigKeypad(void);
int8_t teclaToChar(uint16_t tecla);

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){

	//Inicializar el hardware, perifericos, puertos, clock, etc.
	apiConfigHardware();
	//Carga las configuraciones iniciales de la estacion metereologica
	//EjecutarBootloader();

	leerDatosSD ();
	apiConfigKeypad();

    // FUNCION que inicializa el planificador de tareas
    schedulerInit();


    // Se agrega la tarea tarea4 al planificador
    schedulerAddTask( tarea1, 0, 50 );


    // FUNCION que inicializa la interrupcion que ejecuta el planificador de
    // tareas con tick cada 1ms.
    schedulerStart( 1 );

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
       // Se despachan (ejecutan) las tareas marcadas para su ejecucion.
       // Luego se pone el sistema en bajo consumo hasta que ocurra la
       // proxima interrupcion, en este caso la de Tick.
       // Al ocurrir la interrupcion de Tick se ejecutara el planificador
       // que revisa cuales son las tareas a marcar para su ejecucion.
       schedulerDispatchTasks();
    }

    // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
    // directamenteno sobre un microcontroladore y no es llamado por ningun
    // Sistema Operativo, como en el caso de un programa para PC.
    return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/


void tarea1( void* ptr ){
	actualizarEstadoMEF_Sistema();
}


void tarea2( void* ptr ){

	static bool_t inicializar = 0;

	if( inicializar == 0 ) {
		estadoTimbreTeoria.nroCircuito = TIMBRE_TEORIA;
		inicializarEstadoMEF_Timbre( &estadoTimbreTeoria );
		inicializar = 1;
	}


	gpioToggle( LEDB );
    actualizarEstadoMEF_Timbre(&estadoTimbreTeoria);


}

void tarea3( void* ptr ){

	static bool_t inicializar = 0;

	if( inicializar == 0 ) {
		estadoTimbreTaller.nroCircuito = TIMBRE_TALLER;
		inicializarEstadoMEF_Timbre( &estadoTimbreTaller );
		inicializar = 1;
	}


	gpioToggle( LEDR );
    actualizarEstadoMEF_Timbre(&estadoTimbreTaller);


}

void tarea4( void* ptr ){
   apiActualizarInfoLCD(&estadoTimbreTeoria, &estadoTimbreTaller);
}

void actualizarEstadoMEF_Sistema ( void )
{
	static uint8_t estadoActualSistema = ESTADO_DETENIDO, estadoAnteriorSistema;
	static uint8_t claveIngresada[5];
	// Variable para guardar la tecla leida
	static uint16_t tecla = 0, contadorClave = 0;
	static EstadoMefDebounce_t * estadoTecla;
	// Teclado
	// Filas --> Salidas
			uint8_t keypadRowPins1[4] = { T_FIL0, // Row 0
					T_FIL1,    // Row 1
					T_FIL2,    // Row 2
					T_FIL3     // Row 3
					};

			// Columnas --> Entradas con pull-up (MODO = GPIO_INPUT_PULLUP)
			uint8_t keypadColPins1[4] = { T_COL0,    // Column 0
					T_COL1,    // Column 1
					T_COL2,    // Column 2
					CAN_TD     // Column 3
					};

			keypadConfig(&keypad, keypadRowPins1, 4, keypadColPins1, 4);

	switch ( estadoActualSistema )
	{
		case ESTADO_INICIAL:
		break;
		case ESTADO_DETENIDO: //leerKeypad(&keypad, &tecla);
								debounceActualizarMefParametric(&estadoTecla, &tecla);
								if (estadoTecla == BUTTON_RISING)
									if(tecla == 12 ) {
										estadoAnteriorSistema = estadoActualSistema;
										estadoActualSistema = INGRESO_CLAVE;
									}
							// }
		break;
		case ESTADO_FUNCIONAMIENTO:  //leerKeypad(&keypad, &tecla) ;
										debounceActualizarMefParametric(&estadoTecla, &tecla);
										if (estadoTecla == BUTTON_RISING) {
											if(tecla == 12 ) {
												estadoAnteriorSistema = estadoActualSistema;
												estadoActualSistema = INGRESO_CLAVE;
											}
											if(tecla == 14 ) {
												estadoAnteriorSistema = estadoActualSistema;
												estadoActualSistema = INGRESO_CLAVE;
											}
									   }

		break;
		case INGRESO_CLAVE:
			//leerKeypad(&keypad, &tecla);
				debounceActualizarMefParametric(&estadoTecla, &tecla);
				if (estadoTecla == BUTTON_RISING){
					claveIngresada[contadorClave++] = teclaToChar( tecla );

				if ( contadorClave == 4 ) {
					contadorClave = 0;
					if ( !strcmp ( claveIngresada, CLAVE_ALMACENADA ) ) {

						if ( estadoAnteriorSistema == ESTADO_DETENIDO) {

							schedulerAddTask( tarea2, // funcion de tarea a a�adir
									100,      // offset de ejecucion en ticks
									1000     // periodicidad de ejecucion en ticks
							);

							// Se agrega la tarea tarea2 al planificador
							schedulerAddTask( tarea3, 200, 1000 );

							// Se agrega la tarea tarea3 al planificador
							schedulerAddTask( tarea4, 300, 1000 );

							estadoActualSistema = ESTADO_FUNCIONAMIENTO;
						}
						if ( estadoAnteriorSistema == ESTADO_FUNCIONAMIENTO) {
							schedulerDeleteTask( 1 );
							// Se agrega la tarea tarea2 al planificador
							schedulerDeleteTask( 2 );
							// Se agrega la tarea tarea3 al planificador
							schedulerDeleteTask( 3 );


							estadoActualSistema = ESTADO_DETENIDO;
						}
						if ( estadoAnteriorSistema == CONFIGURACION) {
							schedulerDeleteTask( 1 );
							// Se agrega la tarea tarea2 al planificador
							schedulerDeleteTask( 2 );
							// Se agrega la tarea tarea3 al planificador
							schedulerDeleteTask( 3 );


							estadoActualSistema = ESTADO_DETENIDO;
						}

					}
					else
						estadoActualSistema = estadoAnteriorSistema;
			}

	}

	}
}

int8_t teclaToChar(uint16_t tecla) {

	int8_t keypadToDesplayKeys[16] = { '1', '2', '3', 0x0a, '4', '5', '6', 0x0b, '7', '8', '9',
				0x0c, 0x0e, '0', 0x0f, 0x0d };

	return keypadToDesplayKeys[tecla];
/*	switch (tecla) {
	case 0:
		return '1';
		break;
	case 1:
		return '2';
		break;
	case 2:
		return '3';
		break;
	case 3:
		return 'A';
		break;
	case 4:
		return '4';
		break;
	case 5:
		return '5';
		break;
	case 6:
		return '6';
		break;
	case 7:
		return 'B';
		break;
	case 8:
		return '7';
		break;
	case 9:
		return '8';
		break;
	case 10:
		return '9';
		break;
	case 11:
		return 'C';
		break;
	case 12:
		return '*';
		break;
	case 13:
		return '0';
		break;
	case 14:
		return '#';
		break;
	case 15:
		return 'D';
		break;

	}*/
}

static void apiConfigKeypad(void){
	/* Configuracion de pines para el Teclado Matricial*/





}

/*==================[fin del archivo]========================================*/
