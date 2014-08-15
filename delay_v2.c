/*
 * delay.c
 *
 *  Created on: 14/11/2012
 *      Author: ytaler
 */

#include "lpc2114.h"
#include "delay.h"

// Frecuencia del micro: 14,7456 Mhz
// Tiempo de cada ciclo de reloj: 67,81684028 ns

void delay_ms(unsigned long int tiempo)
{
	int i;
	//for(i=0;i<(tiempo*14745);i++);
	for(i=0;i<(tiempo*1000);i++);
}

void delay_us(unsigned long int tiempo)
{
	int i;
	//for(i=0;i<(tiempo*14);i++);
	for(i=0;i<(tiempo*10);i++);
}
