#include "adc.h"
#define AD_ADCR        (*(REG32 (0xE0034000)))
#define AD_ADDR        (*(REG32 (0xE0034004)))

void adc_init(unsigned char canal) {

  unsigned int configura=0x0000;
  switch (canal)
  {	
	    case 1:
				canal=1;
		break;
	    case 2:
				canal=2;
		break;	
	    case 3:
				canal=4;
		break;	
	    case 4:
				canal=8;
		break;		
  }

  configura |= (16 <<8 );   // CLKDIV = 16 configuro divisor externo --> 0000 0000 0000 0000 1111 0000 0000
  configura |= (1 << 21 ); // PDN = 1 ADC en modo operacional --> 0000 0001 0000 0000 0000 0000 0000
  configura |= canal;      // canal a realizar la conversión --> 0000 0000 0000 0000 1111 0000 0000
  AD_ADCR = configura; // 0000 0000 0000 0000 1111 0000 0001

}
int adc_read(void){
  unsigned int salida;
   AD_ADCR |= (1 << 24);  /* START = 001 comenzar la conversión inmediatamente  */
    do {
       salida = AD_ADDR;
    } while (( salida & (1<<31) ) == 0); /* DONE = 0 termino de convertir */
    
    salida = (salida >>6) & 0x03FF; /* tomar el resultado del bit 6 a 15*/
  
    return salida; 
}
