@ Startup para aplicaciones en C
@ -----------------------------------------------------------------------------

	.global main
	.global _start 
	/* Symbols defined by the linker script */
	.global _etext
	.global _data
	.global _edata
	.global _bss
	.global _ebss

	.text
	.arm
_start:
	/* Vectors (8 total)  */
	b reset   /* reset */
	b loop   /* undefined instruction  */
	b loop   /* software interrupt */
	b loop   /* prefetch abort */
	b loop   /* data abort */
	nop      /* reserved for the bootloader checksum */
	b loop   /* IRQ */
	b loop   /* FIQ */

/* Tareas a realizar para C 
 - Establecer el PLL 
 - Copiar la seccion .data ( variables inicializadas ) a la SRAM
 - Borrar .bss (variables no inicializadas)
 - Establecer el stack pointer
 - Saltar al main del C
*/
reset:
/* 
* Establecer PLL 
* -----------------------------------------------------------------------------
*/
        @ Use r0 for indirect addressing 
        ldr r0, PLLBASE

        @ PLLCFG = PLLCFG_VALUE 
        mov r3, #PLLCFG_VALUE
        str r3, [r0, #PLLCFG_OFFSET]

        @ PLLCON = PLLCON_PLLE 
        mov r3, #PLLCON_PLLE
        str r3, [r0, #PLLCON_OFFSET]

        @ PLLFEED = PLLFEED1, PLLFEED2 
        mov r1, #PLLFEED1
        mov r2, #PLLFEED2
        str r1, [r0, #PLLFEED_OFFSET]
        str r2, [r0, #PLLFEED_OFFSET]

        @ repetir mientras ((PLLSTAT & PLLSTAT_PLOCK) == 0)

pll_loop:
        ldr r3, [r0, #PLLSTAT_OFFSET]
        tst r3, #PLLSTAT_PLOCK
        beq pll_loop

        @ PLLCON = PLLCON_PLLC|PLLCON_PLLE 
        mov r3, #PLLCON_PLLC|PLLCON_PLLE
        str r3, [r0, #PLLCON_OFFSET]

        @ PLLFEED = PLLFEED1, PLLFEED2 ( 
        str r1, [r0, #PLLFEED_OFFSET]
        str r2, [r0, #PLLFEED_OFFSET]	

/*
* Copiar .data 
* -----------------------------------------------------------------------------
*/
	ldr r0, data_source @ indica la posicion donde estan guardados los valores de inicializacion 
	ldr r1, data_start  @ indica la pos.de la RAM donde comienza la zona de variables inicializadas
	ldr r2, data_end    @ indica la pos.de la RAM donde finaliza la zona de variables inicializadas

copy_data:
	cmp   r1, r2
	ldrne r3, [r0], #4
	strne r3, [r1], #4
	bne   copy_data
	
	/* Borrar el sector de variables no inicializadas .bss */
	ldr r0, =0
	ldr r1, bss_start  @ indica la pos.de la RAM donde comienza la zona de variables no inicializadas  
	ldr r2, bss_end    @ indica la pos.de la RAM donde finaliza la zona de variables no inicializadas
clear_bss:
	cmp   r1, r2
	strne r0, [r1], #4
	bne   clear_bss
seguir:	
	/* Stack pointer */
	ldr sp, stack_addr
	bl  main

/* si retorna del main entonces entrar en un bucle infinito*/
loop:   b   loop

/* Constantes (calculo de stack, si la SRAM del LPC comienza en 0x40000000, y mide 16Kb = 4000h) */
stack_addr:	.word   0x40004000

PLLBASE:        .word   0xE01FC080
MAMBASE:        .word   0xE01FC000
STACK_START:	.word	0x40004000


/* Simbolos del linker script */

data_source:    .word   _etext
data_start:     .word   _data
data_end:       .word   _edata
bss_start:      .word   _bss
bss_end:        .word   _ebss

/*
* ----------------------------------------------------------------
* Constantes de 8 bits
* ----------------------------------------------------------------
*
* las constantes de 8 bits son usadas como valores inmediatos y offset
*/

@ PLL configuration 
        .equ PLLCON_OFFSET,   0x0
        .equ PLLCFG_OFFSET,   0x4
        .equ PLLSTAT_OFFSET,  0x8
        .equ PLLFEED_OFFSET,  0xC

        .equ PLLCON_PLLE,    (1 << 0)
        .equ PLLCON_PLLC,    (1 << 1)
        .equ PLLSTAT_PLOCK,  (1 << 10)
        .equ PLLFEED1,        0xAA
        .equ PLLFEED2,        0x55

        .equ PLLCFG_VALUE,    0x23
@ configuracíon del PLL para cristal de 14,7456 Mhz



@ MAM configuration 
        .equ MAMCR_OFFSET,   0x0
        .equ MAMTIM_OFFSET,  0x4

        .equ MAMCR_VALUE,    0x2  /* fully enabled */
        .equ MAMTIM_VALUE,   0x4  /* fetch cycles  */
	
@ stack configuration 
        .equ FIQ_MODE,	0x11
	.equ FIQ_STACK_SIZE, 0x8
	.equ FIQ_DISABLE, (1 << 6)
	.equ IRQ_DISABLE, (1 << 7)
	.equ IRQ_STACK_SIZE, 0x100
	.equ IRQ_MODE, 0x12
	.equ SYS_MODE, 0x1F

	.end
