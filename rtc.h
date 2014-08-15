#include "integer.h"

typedef struct {
	BYTE	hour;	/* 0..23 */
	BYTE	min;	/* 0..59 */
	BYTE	sec;	/* 0..59 */
	BYTE	wday;	/* 1..7 */
	BYTE	mday;	/* 1.. 31 */
	BYTE	month;	/* 1..12 */
	WORD	year;	/* 1..4095 */
} RTC;
//Funcion que inicializa el RTC
int rtc_initialize (void);
/** \fn int rtc_gettime (RTC*)
 * \brief Esta función Obtiene la FECHA y HORA del RTC.
 * \param 1) "RTC *", es un puntero a la estructura tipo RTC creada previamente.
 * \return   Devuelve 1 si fue exitoso.
 * \author   ChaN
 * \modificada:  ytaler
 */
int rtc_gettime (RTC*);					/* Get time */
//Esta funcion setea los valores del RTC (fehca y hora)
int rtc_settime (const RTC*);			/* Set time */

