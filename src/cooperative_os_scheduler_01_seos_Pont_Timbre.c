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
	static uint8_t estadoSistema = ESTADO_DETENIDO;
	// Variable para guardar la tecla leida
	static uint16_t tecla = 0;
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

	switch ( estadoSistema )
	{
		case ESTADO_INICIAL:
		break;
		case ESTADO_DETENIDO: if ( keypadRead(&keypad, &tecla) ) {
								if(tecla == 2) {
									schedulerAddTask( tarea2, // funcion de tarea a a�adir
												                     100,      // offset de ejecucion en ticks
												                     1000     // periodicidad de ejecucion en ticks
												                   );

												   // Se agrega la tarea tarea2 al planificador
												   schedulerAddTask( tarea3, 200, 1000 );

												   // Se agrega la tarea tarea3 al planificador
												   schedulerAddTask( tarea4, 300, 1000 );

									estadoSistema = ESTADO_FUNCIONAMIENTO;
								}
		}
		break;
		case ESTADO_FUNCIONAMIENTO:  // Se agrega la tarea tarea1 al planificador
			if ( keypadRead(&keypad, &tecla) ) {
											if(tecla == 15){
												schedulerDeleteTask( 1);
																								   // Se agrega la tarea tarea2 al planificador
											   schedulerDeleteTask( 2 );

																								   // Se agrega la tarea tarea3 al planificador
												schedulerDeleteTask( 3 );


												estadoSistema = ESTADO_DETENIDO;
											}
			}
		break;
	//	case ESTADO_CONFIGURACION:
	}
}

static void apiConfigKeypad(void){
	/* Configuracion de pines para el Teclado Matricial*/





}

/*==================[fin del archivo]========================================*/
