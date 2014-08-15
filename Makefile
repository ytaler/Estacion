# -----------------------------------------------------------------
# Makefile for ex1.elf 
# -----------------------------------------------------------------
#path herramientas
#compilador

LOADER  = lpc21isp
AS      = arm-elf-as
CC      = arm-elf-gcc
LD      = arm-elf-ld
OBJCOPY = arm-elf-objcopy
OBJDUMP = arm-elf-objdump

AFLAGS  = -mcpu=arm7tdmi -mapcs-32 -g
CFLAGS  = -Wall -O0 -mcpu=arm7tdmi -g
LDFLAGS = -Tlpc2114_flash.ld -nostartfiles

# fuentes del programa 
# ===========================================================================
SOURCES = head.s main.c delay_v2.c lcd_v2.c adc.c rtc.c

# nombre del archivo de salida
# ===========================================================================
TARGET  = main.hex

OBJS1=$(SOURCES:.c=.o) 
OBJS=$(OBJS1:.s=.o)
ELF=$(TARGET:.hex=.elf)
LST=$(TARGET:.hex=.lst)
MAP=$(TARGET:.hex=.map)

all: $(TARGET) $(LST)

depend.lst: $(SOURCES)
	$(CC) -MM $^ > depend.lst

#cargar dependencias 

include depend.lst

#opcion de compilaci�n para .c
%.o:%.s
	$(AS) $(AFLAGS) $< -o $@ 

%.o:%.c
	$(CC) $(CFLAGS) -c $< 

$(TARGET): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(LST): $(ELF)
	$(OBJDUMP) -S $(ELF) > $(LST)
clean:
	rm *.o *.elf *.hex *.lst
grabar:
	$(LOADER) -wipe -hex $(TARGET) /dev/ttyUSB0 115200 14745