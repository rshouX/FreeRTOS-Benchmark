###############################################################################
#Filename    : FreeRTOS
#Author      : hrs
#Date        : 07/08/2024
#Licence     : LGPL v3+; see COPYING for details.
#Description : Generic Makefile (based on gcc). This file is intended
#              to be used with STM32L071CB, and the GNU toolchain.
###############################################################################

# Config ######################################################################
TARGET=FreeRTOS
CPU=-mcpu=cortex-m0 -mthumb-interwork
CDEFS=-DSTM32L071xx -DUSE_HAL_DRIVER

CFLAGS=-O3 -specs=nano.specs -fsigned-char -fno-common -fno-strict-aliasing -fdata-sections -ffunction-sections -ffreestanding
AFLAGS=-fdata-sections -ffunction-sections
WFLAGS=-Wall -Wno-strict-aliasing
DFLAGS=-g3
LFLAGS=-specs=nano.specs -specs=nosys.specs -Wl,--gc-sections,--cref

OBJDIR=Object
PREFIX=arm-none-eabi-
# End Config ##################################################################

# Source ######################################################################
INCS+=-I../FreeRTOS/include
INCS+=-I../FreeRTOS/portable/GCC/ARM_CM0
INCS+=-I../RVMDK-STM32L071/User
INCS+=-I../RVMDK-STM32L071/Drivers/CMSIS/Include
INCS+=-I../RVMDK-STM32L071/Drivers/CMSIS/Device/ST/STM32L0xx/Include
INCS+=-I../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Inc
INCS+=-I../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Inc/Conf

# FreeRTOS Source
CSRCS+=../FreeRTOS/croutine.c
CSRCS+=../FreeRTOS/event_groups.c
CSRCS+=../FreeRTOS/list.c
CSRCS+=../FreeRTOS/queue.c
CSRCS+=../FreeRTOS/tasks.c
CSRCS+=../FreeRTOS/timers.c
CSRCS+=../FreeRTOS/portable/MemMang/heap_4.c
CSRCS+=../FreeRTOS/portable/GCC/ARM_CM0/port.c
# HAL Source
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim_ex.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c
CSRCS+=../RVMDK-STM32L071/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c
CSRCS+=../RVMDK-STM32L071/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c
# User Source
CSRCS+=../RVMDK-STM32L071/User/main.c

ASRCS+=../RVMDK-STM32L071/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/gcc/startup_stm32l071xx.s

LDSCRIPT=stm32l071cb.ld
LIBS=-lc -lnosys
# End Source ##################################################################

# Toolchain ###################################################################
ifdef GCC
CC=$(GCC)/$(PREFIX)gcc
AS=$(GCC)/$(PREFIX)gcc -x assembler-with-cpp
CP=$(GCC)/$(PREFIX)objcopy
LD=$(GCC)/$(PREFIX)gcc
SZ=$(GCC)/$(PREFIX)size
else
CC=$(PREFIX)gcc
AS=$(PREFIX)gcc -x assembler-with-cpp
CP=$(PREFIX)objcopy
LD=$(PREFIX)gcc
SZ=$(PREFIX)size
endif

HEX=$(CP) -O ihex
BIN=$(CP) -O binary -S
# End Toolchain ###############################################################

# User ########################################################################
-include user
# End User ####################################################################

# Build #######################################################################
COBJS=$(CSRCS:%.c=%.o)
CDEPS=$(CSRCS:%.c=%.d)
AOBJS=$(ASRCS:%.s=%.o)
ADEPS=$(ASRCS:%.s=%.d)

DEP=$(OBJDIR)/$(notdir $(@:%.o=%.d))
LST=$(OBJDIR)/$(notdir $(@:%.o=%.lst))
OBJ=$(OBJDIR)/$(notdir $@)
MAP=$(OBJDIR)/$(TARGET).map

# Build all
all: mkdir $(COBJS) $(AOBJS) $(TARGET).elf $(TARGET).hex $(TARGET).bin

# Create output folder
mkdir:
	$(shell if [ ! -e $(OBJDIR) ];then mkdir -p $(OBJDIR); fi)

# Compile C sources
%.o:%.c
	@echo "    CC      $(notdir $<)"
	@$(CC) -c $(CPU) $(CDEFS) $(INCS) $(CFLAGS) $(DFLAGS) -MMD -MP -MF "$(DEP)" -Wa,-a,-ad,-alms="$(LST)" "$<" -o "$(OBJ)"

# Assemble ASM sources
%.o:%.s
	@echo "    AS      $(notdir $<)"
	@$(AS) -c $(CPU) $(INCS) $(AFLAGS) $(DFLAGS) "$<" -o "$(OBJ)"

# Link ELF target file and print size
$(TARGET).elf:$(COBJS) $(AOBJS)
	@echo "    LD [P]  $(notdir $@)"
	@$(LD) $(OBJDIR)/*.o $(CPU) $(LFLAGS) $(DFLAGS) -T $(LDSCRIPT) -Wl,-Map=$(MAP) $(LIBS) -o $(OBJ)
	@$(SZ) $(OBJ)

# Create hex/bin programming files
$(TARGET).hex:$(TARGET).elf
	@echo "    HEX     $(notdir $@)"
	@$(HEX) "$(OBJDIR)/$<" "$(OBJDIR)/$@"

$(TARGET).bin:$(TARGET).elf
	@echo "    BIN     $(notdir $@)"
	@$(BIN) "$(OBJDIR)/$<" "$(OBJDIR)/$@"

# Clean up
clean:
	-rm -rf $(OBJDIR)

# Dependencies
-include $(wildcard $(OBJDIR)/*.d)
# End Build ###################################################################

# End Of File #################################################################

# Copyright (C) Evo-Devo Instrum. All rights reserved #########################

