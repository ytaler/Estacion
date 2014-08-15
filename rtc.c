#include "lpc2114.h"
#include "rtc.h"

/*--------------------------------------------------------------------------*/
/*	Inicializacion del RTC													*/
/*--------------------------------------------------------------------------*/
int rtc_initialize (void)
{
	//Es como conectar el PCLK al RTC. Le mando el clock de periferico
	SCB_PCONP |= 0x00000200;
	RTC_CCR = 0x1;					//Comienza el conteo del contador del RTC
	RTC_PREINT = 449;				//PREINT = int (pclk / 32768) - 1
									//PREINT = int (58982400 / 32768) - 1 = 1799
									//PREINT = int (147456 / 32768) - 1 = 449
	RTC_PREFRAC = 0;
//------------AGREGADOS DE ALARMA E INTERRUPCIONES-----------------------	
	RTC_ILR=0x3;		//Limpia las interrupciones
/* Este registro configura cuando se quiere que interrumpa el RTC. Puede ser por cada incremento de
 * segundo, por cada incremento de minuto, de año, etc.
 * Ver la pagina 202, Tabla 146 de la hoja de datos del  mirco
 * 
 * Registro de 8 bits: IMYEAR, IMMON, IMDOY, IMDOW, IMDOM, IMHOUR, IMMIN, IMSEC
 * interru al minuto:	0		0		0		0	  0		0		1		0
 * */
	RTC_CIIR = 0x2;    	//Seteo que interrumpa a cada minuto 00000010
/* Este registro configura cuando se quiere que salte la alarma del RTC. Puede ser a un cierto
 * segundo, minuto, año, etc.
 * Ver la pagina 203, Tabla 147 de la hoja de datos del  mirco
 * 
 * Registro de 8 bits: AMRYEAR, AMRMON, AMRDOY, AMRDOW, AMRDOM, AMRHOUR, AMRMIN, AMRSEC
 * Alarm segundos:			1		1		1		1	  1			1		1		0
 */ 				
	RTC_AMR = 0xFF;		//Deshabilito las alarmas
	RTC_ALMIN = 5;		//Alarma a los 5 minutos
//------------FIN AGREGADOS DE ALARMA E INTERRUPCIONES-------------------
	// Detiene el clock de PCLK al RTC para reducir consumo. La idea es activarlo nomas
	SCB_PCONP &= 0xFFFFFDFF;
	return 1;
}
/*--------------------------------------------------------------------------*/
/*	Obtener datos actuales del RTC											*/
/*--------------------------------------------------------------------------*/
int rtc_gettime (RTC *rtc)
{
	DWORD d, t;
	do {
		t = RTC_CTIME0;
		d = RTC_CTIME1;
	} while (t != RTC_CTIME0 || d != RTC_CTIME1);
	//Lee los registros del rtc
	rtc->sec = t & 63;				// and con 0011 1111 --> 6 bits, del 00 al 05 del registro ctime0
	rtc->min = (t >> 8) & 63;		// and con 0011 1111 --> 6 bits, del 08 al 13 del registro ctime0
	rtc->hour = (t >> 16) & 31;		// and con 0001 1111 --> 5 bits, del 16 al 20 del registro ctime0
	rtc->wday = (t >> 24) & 7;		// and con 0111 --> 3 bits, del 24 al 26 del registro ctime0
	rtc->mday = d & 31;				// and con 0001 1111 --> 5 bits, del 00 al 04 del registro ctime1
	rtc->month = (d >> 8) & 15;		// and con 1111 --> 4 bits, del 8 al 11 del registro ctime1
	rtc->year = (d >> 16) & 4095;	// and con 1111 1111 1111 --> 12 bits, del 16 al 27 del registro ctime1
	return 1;
}
/*--------------------------------------------------------------------------*/
/*	Escribir datos actuales del RTC											*/
/*--------------------------------------------------------------------------*/
int rtc_settime (const RTC *rtc)
{
	/* Supply PCLK to the RTC */
	SCB_PCONP |= 0x00000200;
	//resetea el clock del rtc y deshabilita el rtc
	RTC_CCR = 0x2;
	//Actualiza los registros del rtc
	RTC_SEC = rtc->sec;
	RTC_MIN = rtc->min;
	RTC_HOUR = rtc->hour;
	RTC_DOW = rtc->wday;
	RTC_DOM = rtc->mday;
	RTC_MONTH = rtc->month;
	RTC_YEAR = rtc->year;
	// Reiniciar el rtc y usar clock externo
	RTC_CCR = 0x1;
	return 1;
}
