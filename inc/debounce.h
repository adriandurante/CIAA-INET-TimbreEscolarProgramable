/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/

#ifndef _DEBOUNCE_H_
#define _DEBOUNCE_H_

/*==================[inclusiones]============================================*/

#include "sapi.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

// Nuevo tipo de datos enumerado llamado estadoMEF
typedef enum{
	BUTTON_UP,      // Mientras el botón está liberado.
	BUTTON_FALLING, // Mientras esta ocurriendo el flanco descendente, hace el anti-rebote.
	BUTTON_RISING,  // Mientras esta ocurriendo el flanco ascendente, hace el anti-rebote.
	BUTTON_DOWN     // Mientras eNl botón está presionado.
} EstadoMefDebounce_t;

/*==================[declaraciones de datos externos]========================*/
extern keypad_t keypad;
/*==================[declaraciones de funciones externas]====================*/

void    debounceActualizarMefParametric (EstadoMefDebounce_t * estado, uint16_t* tecla);
bool_t  leerKeypad( keypad_t* keypad, uint16_t* key );

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _DEBOUNCE_H_ */
