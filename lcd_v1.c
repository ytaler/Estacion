/*
 * lcd.c
 *
 *  Created on: 15/11/2012
 *      Author: ytaler
 *  E: P1.16
 * RW: P1.17
 * RS: P1.18
 * Se usa modo de 4 bits:
 * D0: P0.16
 * D1: P0.17
 * D2: P0.18
 * D3: P0.19
 */

#include "lcd.h"
#include "delay.h"
#include "lpc2114.h"

// Definiciones de hardware
// P0  31:28  27:24  23:20   19:16  15:12  11:8	   7:4    3:0
// 0000 - 0000 - 0000 - 1111 - 0000 - 0000 - 0000 - 0000
// P1  31:28  27:24  23:20   19:16  15:12  11:8	   7:4    3:0
// 0000 - 0000 - 0000 - 0111 - 0000 - 0000 - 0000 - 0000
#define		CTRL_INIT() {IODIR0=0x000F0000;IOCLR0=0x000F0000;IODIR1=0x00070000;IOCLR1=0x00070000;}
#define		E_HIGH()	IOSET1 = (0x01) << 16;
#define		E_LOW()		IOCLR1 = (0x01) << 16;
#define		RW_HIGH()	IOSET1 = (0x01) << 17;
#define		RW_LOW()	IOCLR1 = (0x01) << 17;
#define		RS_HIGH()	IOSET1 = (0x01) << 18;
#define		RS_LOW()	IOCLR1 = (0x01) << 18;

#define		LCD_ETIME_1	1520	// Tiempo que demora la ejecucion de un Clear Display y Return Home [us]
#define		LCD_ETIME_2	42		// Tiempo que demoran el resto de las intrucciones [us]

/*--------------------------------------------------------------------------*/
/*	Habilitar y deshabilitar señal E										*/
/*--------------------------------------------------------------------------*/

void lcd_strobe(){
	E_HIGH();
	;
	;
	E_LOW();
	;
	;
}

/*--------------------------------------------------------------------------*/
/*	Escribir datos de 4 bits												*/
/*--------------------------------------------------------------------------*/

void lcd_write(unsigned char c){
	unsigned char lsn; //Less significant nibble
	unsigned char msn; //Most significant nibble
	msn = ( ( c >> 4 ) & 0x0F );
	lsn = ( c & 0x0F );
	//IOCLR0 = (~msn) << 16;
	//IOSET0 = msn << 16;
	IOPIN0 = ((IOPIN0 & 0xFFF0FFFF) | (msn << 16));
	lcd_strobe();
	//IOCLR0 = (~lsn) << 16;
	//IOSET0 = lsn << 16;
	IOPIN0 = ((IOPIN0 & 0xFFF0FFFF) | (lsn << 16));
	lcd_strobe();
	delay_us(LCD_ETIME_2);	// Retardo de 42 us que demora en completar una instruccion
}

/*--------------------------------------------------------------------------*/
/*	Escribir comandos de 4 bits												*/
/*--------------------------------------------------------------------------*/

void lcd_command_4bit(unsigned char c){
	RS_LOW();			// Pongo en cero RS
	RW_LOW();			// Pongo en cero RW
	IOPIN0 = ((IOPIN0 & 0xFFF0FFFF) | (c << 16));
	lcd_strobe();
}

/*--------------------------------------------------------------------------*/
/*	Escribir comandos de 8 bits												*/
/*--------------------------------------------------------------------------*/

void lcd_command_8bit (unsigned char c){
	RS_LOW();			// Pongo en cero RS
	RW_LOW();			// Pongo en cero RW
	lcd_write(c);
}

/*--------------------------------------------------------------------------*/
/*	Escribir un caracter													*/
/*--------------------------------------------------------------------------*/

void lcd_putch(unsigned char c) {
	RS_HIGH();			// Pongo en uno RS
	RW_LOW();			// Pongo en cero RW
	lcd_write(c);
}

/*--------------------------------------------------------------------------*/
/*	Inicializar Modulo LCD para interfaz de 4 bits							*/
/*--------------------------------------------------------------------------*/
void lcd_init() {

	CTRL_INIT();		// Inicializa los pines de I/O
	delay_ms(100);		// Espero 100 ms por seguridad
	E_LOW();			// Pongo en cero E
	RS_LOW();			// Pongo en cero RS
	RW_LOW();			// Pongo en cero RW

	delay_ms(16);			// Debo esperar un tiempo mayor a 15ms
	lcd_command_4bit(0x3);	// Le escribo el valor 0011 0000
	delay_ms(5);			// Debo esperar un tiempo superior a 4.1 ms
	lcd_command_4bit(0x3);	// Le escribo el valor 0011 0000
	delay_us(100);			// Debo esperar un tiempo superior a 100 us
	lcd_command_4bit(0x3);	// Le escribo el valor 0011
	delay_us(LCD_ETIME_2);		// Espero el tiempo normal de procesamiento de cada instruccion
	lcd_command_4bit(0x2);	// Le escribo el valor 0010
	delay_us(LCD_ETIME_2);		// Espero el tiempo normal de procesamiento de cada instruccion

	lcd_write(LCD_INTERFACE_4|LCD_2_LINE|LCD_5x8);	// Function set: le escribo el valor 0010 1000 (4-bits, 2 lineas, 5x8) --> 0x28
	lcd_write(LCD_DISPLAY_ON);						// Display on/off: le escribo el valor 0000 1000 (display on, cursor y blink off) --> 0x0C
	lcd_write(LCD_CLEAR_DISPLAY); 					// Le escribo el valor 0000 0001 (Display clear) --> 0x01
	delay_us(LCD_ETIME_1);								// Espero un tiempo de 1500 us que es lo que demora un clear display
	lcd_write(LCD_CURSOR_RIGHT);					// Muevo una posicion a la derecha el cursor 0000 0110 --> 0x06
}
