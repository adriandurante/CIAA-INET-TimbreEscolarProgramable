/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

//#include "program.h"   // <= su propio archivo de cabecera (opcional)
#include "sapi.h"        // <= Biblioteca sAPI
#include "sapi_lcd.h"

//#include "c_i18n_es.h" // <= para traducir el codigo C al espaï¿½ol (opcional)
//#include "c_i18n_es.h" // <= para traducir la sAPI al espaï¿½ol (opcional)

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

//CONSOLE_PRINT_ENABLE

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/



void inicializarLCD( uint16_t lineWidth, uint16_t amountOfLines,
              uint16_t charWidth, uint16_t charHeight ){
   
	// Configure LCD Pins as Outputs

	   lcdConfigPinAsOutput( LCD_HD44780_RS );
	   lcdConfigPinAsOutput( LCD_HD44780_RW );
	   lcdConfigPinAsOutput( LCD_HD44780_EN );

	   lcdConfigPinAsOutput( LCD_HD44780_D4 );
	   lcdConfigPinAsOutput( LCD_HD44780_D5 );
	   lcdConfigPinAsOutput( LCD_HD44780_D6 );
	   lcdConfigPinAsOutput( LCD_HD44780_D7 );


	   // Configure LCD for 4-bit mode

	   lcdPinWrite( LCD_HD44780_RW, OFF );   // RW = 0
	   lcdPinWrite( LCD_HD44780_RS, OFF );   // RS = 0

	   lcdPinWrite( LCD_HD44780_EN, OFF );   // EN = 0

	   lcdDelay_ms( LCD_STARTUP_WAIT_MS );   // Wait for stable power

	   lcdCommand( 0x33 );                   // Command 0x33 for 4-bit mode
	   lcdCommandDelay();                    // Wait

	   lcdCommand( 0x32 );                   // Command 0x32 for 4-bit mode
	   lcdCommandDelay();                    // Wait

	   lcdCommand( 0x28 );                   // Command 0x28 for 4-bit mode
	   lcdCommandDelay();                    // Wait


	   // Initialize LCD

	   lcdCommand( 0x0C );                   // Command 0x0E for display on, cursor on
	   lcdCommandDelay();                    // Wait

	   lcdClear();                           // Command for clear LCD

	   lcdCommand( 0x06 );                   // Command 0x06 for Shift cursor right
	   lcdCommandDelay();                    // Wait

	   lcdDelay_ms( 1 );                     // Wait
	}


/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
