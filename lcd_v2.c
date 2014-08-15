/*
 * lcd.c
 *
 *  Created on: 15/11/2012
 *      Author: ytaler
 */

#include "lcd.h"
#include "delay.h"
#include "lpc2114.h"
// Definiciones de hardware
// P0  31:28  27:24  23:20   19:16  15:12  11:8	   7:4    3:0
// 0000 - 0000 - 0000 - 1111 - 0000 - 0000 - 0000 - 0000
// P1  31:28  27:24  23:20   19:16  15:12  11:8	   7:4    3:0
// 0000 - 0000 - 0000 - 0111 - 0000 - 0000 - 0000 - 0000
#define		E_HIGH()	IOSET1 = (0x01) << 16;
#define		E_LOW()		IOCLR1 = (0x01) << 16;
#define		RW_HIGH()	IOSET1 = (0x01) << 17;
#define		RW_LOW()	IOCLR1 = (0x01) << 17;
#define		RS_HIGH()	IOSET1 = (0x01) << 18;
#define		RS_LOW()	IOCLR1 = (0x01) << 18;

#define		LCD_ETIME_1	1520	// Tiempo que demora la ejecucion de un Clear Display y Return Home [us]
#define		LCD_ETIME_2	40		// Tiempo que demoran el resto de las intrucciones [us]

/*--------------------------------------------------------------------------*/
/*	Habilitar y deshabilitar señal E										*/
/*--------------------------------------------------------------------------*/
void lcd_strobe(void){
	E_HIGH();
	;
	;
	;
	E_LOW();
	;
	;
	;
}
/*--------------------------------------------------------------------------*/
/*	Escribir datos de 4 bits												*/
/*--------------------------------------------------------------------------*/
void lcd_write(unsigned char c){
	unsigned char lsn; //Less significant nibble
	unsigned char msn; //Most significant nibble
	delay_us(LCD_ETIME_2);	// Retardo de 42 us que demora en completar una instruccion
	msn = ( ( c >> 4 ) & 0x0F );
	lsn = ( c & 0x0F );
	IOPIN0 = ((IOPIN0 & 0xFFF0FFFF) | (msn << 16));
	lcd_strobe();
	IOPIN0 = ((IOPIN0 & 0xFFF0FFFF) | (lsn << 16));
	lcd_strobe();
}
/*--------------------------------------------------------------------------*/
/*	Escribir comandos de 8 bits												*/
/*--------------------------------------------------------------------------*/
void lcd_command_8bit (unsigned const char c){
	RS_LOW();			// Pongo en cero RS
	RW_LOW();			// Pongo en cero RW
	lcd_write(c);
	if((c==LCD_CLEAR_DISPLAY)||(c==LCD_RETURN_HOME))
		delay_us(LCD_ETIME_1);
}
/*--------------------------------------------------------------------------*/
/*	Escribir un caracter													*/
/*--------------------------------------------------------------------------*/
void lcd_putc(unsigned char c) {
	RS_HIGH();			// Pongo en uno RS
	RW_LOW();			// Pongo en cero RW
	lcd_write(c);
}
/*--------------------------------------------------------------------------*/
/*	Escribir un string													*/
/*--------------------------------------------------------------------------*/
void lcd_puts(const char * s) {
	RS_HIGH();			// Pongo en uno RS
	while (*s)
        lcd_write(*s++);
}
/*--------------------------------------------------------------------------*/
/*	Inicializar Modulo LCD para interfaz de 4 bits							*/
/*--------------------------------------------------------------------------*/
void lcd_init(void) {
	delay_ms(16);			// Debo esperar un tiempo mayor a 15ms
	IOPIN0 = ((IOPIN0 & 0xFFF0FFFF) | (0x03 << 16));	// Le escribo el valor 0011 --> Datos primer envio
	lcd_strobe();			// Habilito y deshabilito la señal enable --> Primer envio
	delay_ms(5);			// Debo esperar un tiempo superior a 4.1 ms
	lcd_strobe();			// Habilito y deshabilito la señal enable --> Segundo envio. Datos iguales
	delay_us(200);			// Debo esperar un tiempo superior a 100 us
	lcd_strobe();			// Habilito y deshabilito la señal enable --> Tercer envio. Datos iguales
	IOPIN0 = ((IOPIN0 & 0xFFF0FFFF) | (0x02 << 16));// Le escribo el valor 0010
	lcd_strobe();									// Habilito y deshabilito la señal E
	lcd_write(LCD_INTERFACE_4|LCD_2_LINE|LCD_5x8);	// Function set: le escribo el valor 0010 1000 (4-bits, 2 lineas, 5x8) --> 0x28
	lcd_write(LCD_DISPLAY_ON);						// Display on/off: le escribo el valor 0000 1000 (display on, cursor y blink off) --> 0x0C
	lcd_write(LCD_CLEAR_DISPLAY); 					// Le escribo el valor 0000 0001 (Display clear) --> 0x01
	delay_us(LCD_ETIME_1);							// Espero un tiempo de 1500 us que es lo que demora un clear display
	lcd_write(LCD_CURSOR_RIGHT);					// Muevo una posicion a la derecha el cursor 0000 0110 --> 0x06
}
