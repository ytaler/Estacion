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
	T0TCR = 2;		// reset timer counter
	T0CTCR = 0;		// Habilita el modo timer, con flanco ascendente
	T0PR = 14704;  	// cuenta a llegar (1ms)
	T0TCR = 1;    	// Habilita el contador
	while(T0TC < tiempo);
	T0TCR = 0;		// Apaga el timer0
}

void delay_us(unsigned long int tiempo)
{
	T0TCR = 2;		// reset timer counter
	T0CTCR = 0;		// Habilita el modo timer, con flanco ascendente
	T0PR = 14;  	// cuenta a llegar (1us)
	T0TCR = 1;    	// Habilita el contador
	T0TC = 0;		// Inicializa el contador
	while(T0TC < tiempo);
	T0TCR = 0;		// Apaga el timer0
}
