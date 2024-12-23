EXE_NAME=LED_CUBE
VER_MAJ = 1
VER_MIN = 0
VER_PATCH = 1
MAKE_BINARY=yes

TCHAIN = arm-none-eabi-
MCPU += -mcpu=cortex-m3 -mthumb
CDIALECT = gnu99
OPT_LVL = 0
DBG_OPTS = -gdwarf-2 -ggdb -g

CFLAGS   += -fvisibility=hidden -funsafe-math-optimizations -fdata-sections -ffunction-sections -fno-move-loop-invariants
CFLAGS   += -fmessage-length=0 -fno-exceptions -fno-common -fno-builtin -ffreestanding
CFLAGS   += -fsingle-precision-constant
CFLAGS   += $(C_FULL_FLAGS)
CFLAGS   += -Werror

CXXFLAGS += -fvisibility=hidden -funsafe-math-optimizations -fdata-sections -ffunction-sections -fno-move-loop-invariants
CXXFLAGS += -fmessage-length=0 -fno-exceptions -fno-common -fno-builtin -ffreestanding
CXXFLAGS += -fvisibility-inlines-hidden -fuse-cxa-atexit -felide-constructors -fno-rtti
CXXFLAGS += -fsingle-precision-constant
CXXFLAGS += $(CXX_FULL_FLAGS)
CXXFLAGS += -Werror

LDFLAGS  += -specs=nano.specs
LDFLAGS  += -Wl,--gc-sections
LDFLAGS  += -Wl,--print-memory-usage

PPDEFS += STM32F103C8 USE_STDPERIPH_DRIVER STM32F10X_MD USE_DMA

INCDIR += inc
INCDIR += cmsis
INCDIR += cmsis_boot
INCDIR += STM32F10x_StdPeriph_Driver/inc
INCDIR += STM32_USB-FS-Device_Driver/inc

SOURCES += startup_stm32f103x8.s
SOURCES += cmsis_boot/system_stm32f10x.s
SOURCES += $(wildcard src/*.c)
SOURCES += $(wildcard STM32F10x_StdPeriph_Driver/src/*.c)
SOURCES += $(wildcard STM32_USB-FS-Device_Driver/src/*.c)

LDSCRIPT += stm32f103c8.ld

include core.mk

#####################
### FLASH & DEBUG ###
#####################

flash: $(BINARY)
	@openocd -d0 -f ../target/stm32_f103.cfg -c "program $< 0x08000000 verify reset exit" 

ds:
	@openocd -d0 -f ../target/stm32_f103.cfg

debug:
	@set _NO_DEBUG_HEAP=1
	@echo "file $(EXECUTABLE)" > .gdbinit
	@echo "set auto-load safe-path /" >> .gdbinit
	@echo "set confirm off" >> .gdbinit
	@echo "target extended-remote :3333" >> .gdbinit
	@arm-none-eabi-gdb -q -x .gdbinit