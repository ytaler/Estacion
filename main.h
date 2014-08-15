#ifndef MAIN_H
#define MAIN_H
#include "lpc2114.h"
//	31:28	27:24	23:20	19:16	15:12	11:08	07:04	03:00	<-- Bits 1 salida, 0 entrada, x no importa
//	xxx0 	xxx0	0000	x111	xxxx	xxxx	xxxx	xxxx	<-- Bits 1 salida, 0 entrada, x no importa
#define anemometro		IOPIN0&0x00001000	// P0.12
#define boton_menu		IOPIN0&0x00100000	// P0.20
#define boton_ok		IOPIN0&0x00000200	// P0.09
#define boton_bajar		IOPIN1&0x00800000	// P1.23
#define boton_subir		IOPIN1&0x01000000	// P1.24

#define prende_luz()	IOSET1 = (0x01) << 28; // Enciende la luz del lcd
#define	 apaga_luz()	IOCLR1 = (0x01) << 28; // Apaga la luz del lcd

#define M_PI		3.14159265358979323846
#define radio_km	0.00025
#define radio_m		0.25
#define radio_cm	25
#define producto	11309.7335529 //Radio_km * M_PI * 3600 * 4000, 3600 para pasarlo de s a hora, 4000 para pasarlo de us a s
#endif
