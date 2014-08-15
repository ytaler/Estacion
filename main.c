/*------------------------------------------------------------------------
						Archivos de cabecera
-------------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "main.h"
#include "delay.h"
#include "lcd.h"
#include "adc.h"
#include "rtc.h"
/*------------------------------------------------------------------------
			Declaración de funciones e interrupciones.
-------------------------------------------------------------------------*/
void configurar_fecha_hora (void);
void gpio_init(void);
short int configurar_parametros_lcd(short cifras,short v_inicial,short v_final,const char *parametro);
unsigned int leer_temperatura(void);
unsigned int leer_humedad(void);
unsigned int leer_presion(void);
void leer_dirviento(void);
unsigned int leer_velviento(void);
void mostrar_datos(void);
void mostrar_fecha(void);
/*------------------------------------------------------------------------
			  Declaración de variables globales.
-------------------------------------------------------------------------*/
RTC rtc_global = {	 //Creo variable de Hora inicial.
		0,0, 0,	// HH:MM:SS
		1, 1,  	// DíaSem, DíaMes,
		1, 2012    // mes, anio
	};
char dir_viento[2];
unsigned int velocidad;
/*------------------------------------------------------------------------
						Comienzo del Main.
-------------------------------------------------------------------------*/
int main (void){
	unsigned int i;
	gpio_init();
	//rtc_initialize ();	//Inicializa el Reloj de tiempo Real.
	lcd_init();			//Inicializa el display lcd
	prende_luz();
	/*char debug[32];
	while(1){
		lcd_command_8bit(LCD_CLEAR_DISPLAY);
		sprintf(debug,"Vel: %d",leer_velviento());
		lcd_puts(debug);
		delay_ms(500);
	}*/
	// Mensaje de bienvenida
	lcd_puts("** Bienvenido **");
	for(i=0;i<4;i++){
		lcd_command_8bit(LCD_DISPLAY_OFF);
		delay_ms(500);
		lcd_command_8bit(LCD_DISPLAY_ON);
		delay_ms(500);
	}
	while(1){
	lcd_command_8bit(LCD_CLEAR_DISPLAY);
	lcd_puts("E. Meteorologica");
	lcd_command_8bit(LCD_SEGUNDA_LINEA);
	lcd_puts("v 2.00");
	delay_ms(3000);
	}
	configurar_fecha_hora();
	while(1){
		//Mostrar datos reloj
		while(~boton_menu){
			mostrar_fecha();
			delay_ms(400);
		}
		while(boton_menu);
		//Mostrar datos meteorologicos
		while(~boton_menu){
			mostrar_datos();
			if(boton_menu)
				break;
			delay_ms(2000);
			for(i=16;i>0;i--){
				lcd_command_8bit(LCD_SHIFT_RIGHT_2);
				if(boton_menu)
					break;
				delay_ms(300);
			}
			if(boton_menu)
				break;
			delay_ms(2000);
			for(i=16;i>0;i--){
				lcd_command_8bit(LCD_SHIFT_LEFT_2);
				if(boton_menu)
					break;
				delay_ms(300);
			}
		}
		while(boton_menu);
		//Apagar display y borrar datos
		apaga_luz();
		lcd_command_8bit(LCD_CLEAR_DISPLAY);
		lcd_command_8bit(LCD_DISPLAY_OFF);
		while(~boton_menu){
			delay_ms(100);
		}
		while(boton_menu);
		lcd_command_8bit(LCD_DISPLAY_ON);
		prende_luz();
	}
	return 0;
}
/*------------------------------------------------------------------------
			Funcion que inicializa los puertos.
-------------------------------------------------------------------------*/
void gpio_init(void){
	/*------------------------------------------------------------------------
				  Definicion de botones.
	-------------------------------------------------------------------------*/
	// 	Configuracion P0
	//	31:28	27:24	23:20	19:16	15:12	11:08	07:04	03:00	<-- Bits 1 salida, 0 entrada, x no importa
	//	x000 	0xxx	xxx0	1111	xxxx	xx0x	xxxx	xx01	<-- Bits 1 salida, 0 entrada, x no importa
	//	0		0		0		F		0		0		0		1		<-- OR
	//	8		7		E		F		F		D		F		D		<-- AND

	// P0.00 --> Salida UART0
	// P0.01 --> Entrada UART0
	// P0.04 --> Serial Clock SPI
	// P0.05 --> Data Input SPI
	// P0.06 --> Data Output SPI
	// P0.07 --> Select Slave
	// P0.09 --> Boton Ok
	// P0.16 --> Db4 lcd --> Out
	// P0.17 --> Db5 lcd --> Out
	// P0.18 --> Db6 lcd --> Out
	// P0.19 --> Db7 lcd --> Out
	// P0.20 --> Interrupcion externa. Menu --> In
	// P0.27 --> Temperatura --> In
	// P0.28 --> Presion	 --> In
	// P0.29 --> Humedad	 --> In
	// P0.30 --> Entrada señal anemometro
	PCB_PINSEL0=(PCB_PINSEL0 & 0xFFFF00F0); //Bits del 1 al 15
	PCB_PINSEL0=(PCB_PINSEL0 | 0x00005505); //Bits del 1 al 15
	PCB_PINSEL1=(PCB_PINSEL1 & 0x000FFF00); //Bits del 16 al 31
	PCB_PINSEL1=(PCB_PINSEL1 | 0x05400000); //Bits del 16 al 31
	IODIR0=(IODIR0 & 0x87EFFDFD);
	IODIR0=(IODIR0 | 0x000F0001);
	IOCLR0=0x000F0001;

	// 	Configuracion P1
	//	31:28	27:24	23:20	19:16	15:12	11:08	07:04	03:00	<-- Bits 1 salida, 0 entrada, x no importa
	//	xxx1 	xxx0	0000	x111	xxxx	xxxx	xxxx	xxxx	<-- Bits 1 salida, 0 entrada, x no importa
	//	1		0		0		7		0		0		0		0		<-- OR
	//	F		E		0		F		F		F		F		F		<-- AND
	// P1.16 --> Enable del lcd --> Out
	// P1.17 --> Read/Write lcd --> Out
	// P1.18 --> Ready/Set lcd  --> Out
	// P1.20 --> Entrada 1 veleta (lsb) --> In
	// P1.21 --> Entrada 2 veleta	 	--> In
	// P1.22 --> Entrada 3 veleta (msb) --> In
	// P1.23 --> Boton arriba	--> In
	// P1.24 --> Boton abajo	--> In
	// P1.28 --> Control del transistor que comanda la luz del LCD

	PCB_PINSEL2=(PCB_PINSEL2 & 0x0);
	IODIR1=(IODIR1 & 0xFE0FFFFF);
	IODIR1=(IODIR1 | 0x10070000);
	IOCLR1=0x10070000;
}
/*------------------------------------------------------------------------
			Funcion que lee la temperatura.
-------------------------------------------------------------------------*/
unsigned int leer_temperatura(void){
	unsigned int temperatura;
	adc_init(1);
	temperatura=(adc_read()*70/1023)-20;
	// 70: Rango de temperatura posible, de -20°C a 50°C
	// 1023: Valor máximo posible del conversor (10 bits, 1024 posiciones)
	// -20: Ajuste de valor inicial de temperatura. Se hace porque comienza en -20°C
	return temperatura;
}
/*------------------------------------------------------------------------
			Funcion que lee la presion.
-------------------------------------------------------------------------*/
unsigned int leer_presion(void){
unsigned int presion;
	adc_init(2);
	presion=(adc_read()*200/1023)+900;		// Resultado
	// 200: Rango de presion en hpa (de 900 hpa a 1100 hpa). Si fuera en kpa seria 20 (90 a 110 kpa)
	// 1023: Valor máximo posible del conversor (10 bits, 1024 posiciones)
	// +900: Ajuste de valor inicial de presion. Se hace porque comienza en 900 hpa. Si fuera en kpa la medicion seria +90
	return presion;
}
/*------------------------------------------------------------------------
			Funcion que lee la humedad.
-------------------------------------------------------------------------*/
unsigned int leer_humedad(void){
	unsigned int humedad;
	adc_init(3);
	humedad=(adc_read()*100/1023);		// Resultado
	// 100: Rango de humedad relativa, de 0% a 100%.
	// 1023: Valor máximo posible del conversor (10 bits, 1024 posiciones)
	return humedad;
}
/*------------------------------------------------------------------------
			Funcion que lee la direccion del viento.
-------------------------------------------------------------------------*/
void leer_dirviento(void){
	short int valor;
	valor = (IOPIN1 >> 20) & 0x00000007;
    switch(valor){
                  case 0:
                       strcpy(dir_viento,"N ");
                       break;
                  case 1:
                       strcpy(dir_viento,"NE");
                       break;
                  case 2:
                       strcpy(dir_viento,"E ");
                       break;
                  case 3:
                       strcpy(dir_viento,"SE");
                       break;
                  case 4:
                       strcpy(dir_viento,"S ");
                       break;
                  case 5:
                       strcpy(dir_viento,"SO");
                       break;
                  case 6:
                       strcpy(dir_viento,"O ");
                       break;
                  case 7:
                       strcpy(dir_viento,"NO");
                       break;
                  default:
                          strcpy(dir_viento,"ER");
                          break;
    }
    return;
}
/*------------------------------------------------------------------------
			Funcion que lee la direccion del viento.
-------------------------------------------------------------------------*/
unsigned int leer_velviento(void){
	unsigned int velocidad=0, contador_us=1, clave=1;
	if(anemometro){
		while(anemometro){
			delay_us(250);
			contador_us++;
			if(contador_us > 4000){ //Maximo valor de espera, 2 s
				contador_us=0;
				clave=0;
				break;
			}
		}
		while((~anemometro)&(clave)){
			delay_us(250);
			contador_us++;
			if(contador_us > 4000){ //Maximo valor de espera, 2 s
				contador_us=0;
				break;
			}
		}
	}
	else{
		while(~anemometro){
			delay_us(250);
			contador_us++;
			if(contador_us > 4000){ //Maximo valor de espera, 2 s
				contador_us=0;
				clave=0;
				break;
			}
		}
		while((anemometro)&(clave)){
			delay_us(250);
			contador_us++;
			if(contador_us > 4000){ //Maximo valor de espera, 2 s
				contador_us=0;
				clave=0;
				break;
			}
		}
		while((~anemometro)&(clave)){
			delay_us(250);
			contador_us++;
			if(contador_us > 4000){ //Maximo valor de espera, 2 s
				contador_us=0;
				break;
			}
		}
	}

	if(contador_us == 0)
		velocidad=0;
	else{
		velocidad = producto / contador_us;
		if(velocidad >= 200)
			velocidad = 0;
	}
	return velocidad;
}
/*------------------------------------------------------------------------
			Funcion que muestra los datos en el lcd.
-------------------------------------------------------------------------*/
void mostrar_datos(void){
	char linea[40];
	lcd_command_8bit(LCD_CLEAR_DISPLAY);
	//sprintf(linea,"Temp: %d%cC, Humedad: %d%%",leer_temperatura(),210,leer_humedad());
	sprintf(linea,"T: %d%cC, H: %d%%",leer_temperatura(),210,leer_humedad());
	lcd_puts(linea);
	lcd_command_8bit(LCD_SEGUNDA_LINEA); // 1100 0000, 0xC0
	leer_dirviento();
	//sprintf(linea,"Pre: %d hpa, Viento: %s %d km/h",leer_presion(),dir_viento, leer_velviento());
	sprintf(linea,"P: %d hpa, %s %d km/h",leer_presion(),dir_viento, leer_velviento());
	lcd_puts(linea);

}
/*------------------------------------------------------------------------
			Funcion que muestra la hora y fecha.
-------------------------------------------------------------------------*/
void mostrar_fecha(void){
	char linea[32];
	rtc_gettime(&rtc_global);
	lcd_command_8bit(LCD_CLEAR_DISPLAY);
	sprintf(linea,"Fecha: %d-%d-%d",rtc_global.mday, rtc_global.month, rtc_global.year);
	lcd_puts
	(linea);
	lcd_command_8bit(LCD_SEGUNDA_LINEA); // 1100 0000, 0xC0
	sprintf(linea,"Hora: %d:%d:%d",rtc_global.hour, rtc_global.min, rtc_global.sec);
	lcd_puts(linea);
}
/*------------------------------------------------------------------------
			Funcion que configura la hora y fecha.
-------------------------------------------------------------------------*/
void configurar_fecha_hora(void){
	char temp[16];
	// 	Configuracion P1
	//	31:28	27:24	23:20	19:16	15:12	11:08	07:04	03:00 <-- Bits 1 salida, 0 entrada
	//	xxxx	xxxx	xx00	0111	xxxx	xxxx	xxxx	xxxx
	/*---------------------- INICIO CONFIGURACION ---------------------------*/
	lcd_command_8bit(LCD_DISPLAY_ON|LCD_BLINK_ON);
	/*------------ Configuracion del valor dia ------------------------------*/
	rtc_global.mday=configurar_parametros_lcd(2,1,31,"Dia: 01");
	delay_ms(500);
	/*------------ Configuracion del valor mes ------------------------------*/
	rtc_global.month=configurar_parametros_lcd(2,1,12,"Mes: 01");
	delay_ms(500);
	/*------------ Configuracion del valor año ------------------------------*/
	sprintf(temp,"A%co: 2012",167);
	rtc_global.year=configurar_parametros_lcd(4,2012,3000,temp);
	delay_ms(500);
	/*------------ Configuracion del valor hora -----------------------------*/
	rtc_global.hour=configurar_parametros_lcd(2,0,23,"Hora: 00");
	delay_ms(500);
	/*------------ Configuracion del valor minuto ---------------------------*/
	rtc_global.min=configurar_parametros_lcd(2,0,59,"Minuto: 00");
	delay_ms(500);
	/*------------ Configuracion del valor segundo --------------------------*/
	rtc_global.sec=configurar_parametros_lcd(2,0,59,"Segundo: 00");
	delay_ms(500);
	/*------------------------ FIN CONFIGURACION ----------------------------*/
	lcd_command_8bit(LCD_DISPLAY_ON|LCD_BLINK_OFF);
	rtc_settime(&rtc_global);
	delay_ms(1000);
}
/*------------------------------------------------------------------------
			Funcion que controla los datos a configurar en el lcd.
-------------------------------------------------------------------------*/
short configurar_parametros_lcd(short cifras,short v_inicial,short v_final,const char *parametro){
	short valor=v_inicial, umil, centena, decena, unidad, temp=v_inicial;
	lcd_command_8bit(LCD_CLEAR_DISPLAY);
	lcd_puts("Config. Inicial:");
	lcd_command_8bit(LCD_SEGUNDA_LINEA); // 1100 0000, 0xC0
	while(*parametro)
		lcd_putc(*parametro++);
	lcd_command_8bit(LCD_CURSOR_LEFT_2);
	while(~boton_ok){
		if(boton_subir){
			valor++;
			delay_ms(250);
		}
		if(boton_bajar){
			valor--;
			delay_ms(250);
		}
		if(valor >= (v_final+1))
			valor=v_inicial;
		if(valor <= (v_inicial-1))
			valor=v_final;
		if(valor != temp){
			temp=valor;
			lcd_command_8bit(LCD_CURSOR_RIGHT_2);
			switch(cifras){
				case 1:
					unidad=valor+48;
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_putc(unidad);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					break;
				case 2:
					decena=(valor/10)+48;
					unidad=(valor%10)+48;
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_putc(decena);
					lcd_putc(unidad);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					break;
				case 3:
					centena=(valor/100)+48;
					decena=((valor%100)/10)+48;
					unidad=(valor%10)+48;
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_putc(centena);
					lcd_putc(decena);
					lcd_putc(unidad);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					break;
				case 4:
					umil=(valor/1000)+48;
					centena=((valor%1000)/100)+48;
					decena=((valor%100)/10)+48;
					unidad=(valor%10)+48;
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					lcd_putc(umil);
					lcd_putc(centena);
					lcd_putc(decena);
					lcd_putc(unidad);
					lcd_command_8bit(LCD_CURSOR_LEFT_2);
					break;
			}
		}
	}
	return valor;
}
