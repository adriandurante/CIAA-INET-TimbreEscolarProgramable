/*
 * apiTimbre.h
 *
 *  Created on: 20/10/2018
 *      Author: aadur
 */

#ifndef APITIMBRE_H_
#define APITIMBRE_H_

#define SALIDA_TIMBRE_TEORIA LED1	//GPIO4
#define SALIDA_TIMBRE_TALLER LED3	//GPIO6

#define CLAVE_ALMACENADA "1234"

#define ACTIVADO 	TRUE
#define DESACTIVADO FALSE

#define UART_BAUDRATE					(115200)

#define _API_STATE_OK			(0)
#define _API_STATE_ERROR		(1)

#include "stdint.h"
#include "ff.h"
#include "sapi_datatypes.h"
//#include "sapi_rtc.h"
#include "sapi_lcd.h"
#include "apiLCD.h"
#include "sapi.h"
//#include "sapi_spi.h"
#include "apiTimbre.h"
#include "stdlib.h"
#include "string.h"
#include "debounce.h"

enum {
	TIMBRE_TEORIA = 1,
	TIMBRE_TALLER = 2
};

enum {
	TIEMPO_DURACION_TIMBRE_CORTO = 5,
	TIEMPO_DURACION_TIMBRE_MEDIO = 8,
	TIEMPO_DURACION_TIMBRE_LARGO = 12
};

typedef struct {
	uint16_t horaTimbreFM;
	uint8_t  zonaCircuito;
	uint8_t  tiempoDuracion;
	uint8_t	 textoTimbre[30];
} estructuraHorarios_t;

typedef struct {
	bool_t 	 estadoTimbre;
	uint16_t cantidadTotalTimbres;
	uint16_t tiempoTimbre;
	uint8_t  nroTimbre;
	uint8_t  nroCircuito;
} estadoTimbres_t;

typedef struct {
	uint8_t dia;
	uint8_t mes;
} fecha_t;

typedef struct {
	fecha_t fecha;
	int8_t descripcion[40];
}estructuraFeriados_t;

typedef enum {
	ESTADO_INICIAL,
	ESTADO_DETENIDO,
	ESTADO_FUNCIONAMIENTO,
	ESTADO_CONFIGURACION
};

typedef enum {
	ESPERANDO_CLAVE,
	INGRESANDO_CLAVE
};

typedef enum {
	HORA_FECHA,
	PROXIMO_TIMBRE,
	ESTADO_TIMBRE
} estadoLCD_t;

typedef enum {
	LUNES = 1,
	MARTES,
	MIERCOLES,
	JUEVES,
	VIERNES,
	SABADO,
	DOMINGO
}diaSemana_t;


uint16_t leerHoraActualFM ( void );
void inicializarEstadoMEF_Timbre ( estadoTimbres_t* );
void actualizarEstadoMEF_Timbre ( estadoTimbres_t* );
void actualizarEstadoMEF_Sistema ( void );
void actualizarSalidas(estadoTimbres_t*);
void buscarProximoTimbre (estadoTimbres_t*);
uint8_t leerDiaSemana ( void );
bool_t esFinDeSemana ( void );
bool_t esFeriado ( void );
uint8_t apiConfigHardware ( void );
void EjecutarBootloader ( void );
void apiActualizarInfoLCD( estadoTimbres_t*, estadoTimbres_t* );
void apiActualizarInfoLCD_Detenido( void );
void leerDatosSD ( void );






#endif /* APITIMBRE_H_ */
