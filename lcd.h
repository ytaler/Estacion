/*
 * lcd.h
 *
 *  Created on: 15/11/2012
 *      Author: ytaler
 */

#ifndef _LCD_H_
#define _LCD_H_

#define LCD_ROWS	2		// Numero de filas
#define LCD_COLS	16		// Numero de columnas
//Definicion de comandos del LCD, RS=0 y RW=0. --> DB7 DB6 DB5 DB4 - DB3 DB2 DB1 DB0 <--
// BIT 0: Clear display --> 0 0 0 0 - 0 0 0 1 <--
#define LCD_CLEAR_DISPLAY	0x01	// Borrar display --> 0000 0001

// BIT 1: Return Home
#define LCD_RETURN_HOME		0x02	// 0, Retorna el cursor a la direccion 0 --> 0000 001x

// BIT 2: Entry mode set --> 0 0 0 0 - 0 1 I/D S <--
#define LCD_CURSOR_LEFT		0x04	// 00, Mueve el cursor a la izquierda --> 0000 0100
#define LCD_CURSOR_RIGHT	0x06	// 10, Mueve el cursor a la derecha ----> 0000 0110
#define LCD_SHIFT_RIGHT		0x05	// 01, Mueve todos los datos del display a la derecha ----> 0000 0101
#define LCD_SHIFT_LEFT		0x07	// 11, Mueve todos los datos del display a la izquierda --> 0000 0111

// BIT 3: Display/Cursor on/off --> 0 0 0 0 - 1 D C B <--
#define LCD_DISPLAY_OFF		0x08	// D: 0, Se apaga el display, pero los datos quedan en memoria --> 0000 10xx
#define LCD_DISPLAY_ON		0x0C	// D: 1, Se enciende el display ---------------------------------> 0000 11xx
#define LCD_CURSOR_OFF		0x08	// C: 0, Se apaga el cursor, pero se mantiene su posicion en memoria --> 0000 1x0x
#define LCD_CURSOR_ON		0x0A	// C: 1, Se enciende el cursor ----------------------------------------> 0000 1x1x
#define LCD_BLINK_OFF		0x08	// B: 0, No titila el cursor --> 0000 1xx0
#define LCD_BLINK_ON		0x09	// B: 1, El cursor titila -----> 0000 1xx1

// BIT 4: Cursor o display shift. Se usa para busqueda o correcion de caracteres --> 0 0 0 1 -  S/C R/L x x <--
#define LCD_CURSOR_LEFT_2	0x10	// 00, Mueve el cursor a la izquierda --> 0001 00xx
#define LCD_CURSOR_RIGHT_2	0x14	// 01, Mueve el cursor a la derecha ----> 0001 01xx
#define LCD_SHIFT_RIGHT_2	0x18	// 10, Mueve todos los datos del display a la derecha ----> 0001 10xx
#define LCD_SHIFT_LEFT_2	0x1C	// 11, Mueve todos los datos del display a la izquierda --> 0001 11xx
// BIT 5: Function Set --> 0 0 1 DL -  N F x x <--
#define LCD_INTERFACE_4		0x20	// DL: 0, Establece interfaz de comunicacion de 4 bits --> 0010 xxxx
#define LCD_INTERFACE_8		0x30	// DL: 1, Establece interfaz de comunicacion de 8 bits --> 0011 xxxx
#define LCD_1_LINE			0x20	// N: 0, Establece modo de 1 linea ---> 001x 0xxx
#define LCD_2_LINE			0x28	// N: 1, Establece modo de 2 lineas --> 001x 1xxx
#define LCD_5x8				0x20	// F: 0, Tamaño de la fuente. Para una y dos lineas  --> 001x 00xx
#define LCD_5x11			0x24	// F: 1, Tamaño de la fuente. Solamente para 1 linea --> 001x 01xx
// BIT 6: Set CGRAM Addres --> DB7=0, DB6=1
// BIT 7: Set DDRAM Addres --> DB7=1, DB6-DB0=Addres. Para 2 lineas, 0x00 a 0x27 (1ra) y 0x40 a 0x67 (2da). Para una linea 0x00 a 0x4FH
#define LCD_PRIMERA_LINEA	0x80	// 1000 0000
#define LCD_SEGUNDA_LINEA	0xC0	// 1100 0000

// Read Busy Flag and Address: RS=0, RW=1, DB7=BF, DB6-DB0=Addres
// Write Data to CGRAM o DDRAM: RS=1, RW=0, DB7-DB0=Addres
// Read Data from CGRAM or DDRAM: RS=1, RW=1, DB7-DB0=Addres

// Inicializa el LCD - (Llamar al principio)
void lcd_init(void);
// Escribe un byte en el LCD en modo de 4bits
void lcd_write(unsigned char);
// Escribe comandos de 8bits
void lcd_command_8bit(unsigned const char c);
// Escribe un caracter
void lcd_putc(unsigned char);
// Escribe un string
void lcd_puts(const char * s);
// Habilita y deshabilita la señal de enable
void lcd_strobe(void);

#endif /* _LCD_H_ */
