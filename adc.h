#ifndef ADC_H
#define ADC_H
#include "lpc2114.h"

void adc_init(unsigned char canal) ;
/**
 * \fn void adc_init().
 * \brief Esta función configura el ADC.
 * \param 1)Canal
 * \return No retorna ningún valor.
 * \warning No configura el canal de medición.
 * \author Bruno Agresta
 */
int adc_read();
/**
 * \fn int adc_read(unsigned char canal).
 * \brief Esta función lee el Adc.
 * \param 1) ninguno
 * \return Retorna la salida del ADC leido.
 * \author Bruno Agresta.
 */

#endif
