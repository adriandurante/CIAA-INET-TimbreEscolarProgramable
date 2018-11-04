/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "debounce.h"        // <= sAPI header
#include "sapi.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void    debounceActualizarMefParametric (EstadoMefDebounce_t * estado, uint16_t* tecla){
	switch(*estado) {
		case BUTTON_UP:
			if( leerKeypad(&keypad, tecla) ){
				*estado = BUTTON_FALLING;
			}
			break;
		case BUTTON_FALLING:
			if( leerKeypad(&keypad, tecla) ){
				*estado = BUTTON_DOWN;
			} else{
				*estado = BUTTON_UP;
			}
			break;
		case BUTTON_RISING:  // Mientras esta ocurriendo el flanco ascendente,
			if( !leerKeypad(&keypad, tecla) ){
				*estado = BUTTON_UP;
			} else{
				*estado = BUTTON_DOWN;
			}
			break;
		case BUTTON_DOWN:
			if( !leerKeypad(&keypad, tecla) ){
				*estado = BUTTON_RISING;
			}
			break;
		default:
			*estado = BUTTON_UP;
			break;
	}
}



/* Return TRUE if any key is pressed or FALSE (0) in other cases.
 * If exist key pressed write pressed key on key variable */
bool_t leerKeypad( keypad_t* keypad, uint16_t* key ){

   bool_t retVal = FALSE;

   uint8_t r = 0; // Rows
   uint8_t c = 0; // Columns

   // Put all Rows in LOW state
   for( r=0; r<keypad->keypadRowSize; r++ ){
      gpioWrite( keypad->keypadRowPins[r], LOW );
   }

   // Check all Columns to search if any key is pressed
   for( c=0; c<keypad->keypadColSize; c++ ){

      // If reads a LOW state in a column then that key may be pressed
      if( !gpioRead( keypad->keypadColPins[c] ) ){

        // delay( 50 ); // Debounce 50 ms

         // Put all Rows in HIGH state except first one
         for( r=1; r<keypad->keypadRowSize; r++ ){
            gpioWrite( keypad->keypadRowPins[r], HIGH );
         }

         // Search what key are pressed
         for( r=0; r<keypad->keypadRowSize; r++ ){

            // Put the Row[r-1] in HIGH state and the Row[r] in LOW state
            if( r>0 ){ // Prevents negative index in array
               gpioWrite( keypad->keypadRowPins[r-1], HIGH );
            }
            gpioWrite( keypad->keypadRowPins[r], LOW );

            // Check Columns[c] at Row[r] to search if the key is pressed
            // if that key is pressed (LOW state) then retuns the key
            if( !gpioRead( keypad->keypadColPins[c] ) ){
               *key = (uint16_t)r * (uint16_t)(keypad->keypadColSize) + (uint16_t)c;
               retVal = TRUE;
               return retVal;
            }
         }

      }
   }

   /*
      4 rows * 5 columns Keypad

         c0 c1 c2 c3 c4
      r0  0  1  2  3  4
      r1  5  6  7  8  9    Press r[i] c[j] => (i) * amountOfColumns + (j)
      r2 10 11 12 13 14
      r3 15 16 17 18 19
   */

   // if no key are pressed then retun FALSE
   return retVal;
}

/*==================[fin del archivo]========================================*/
