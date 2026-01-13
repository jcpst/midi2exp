# Makefile for MIDI to Expression Pedal Converter
# Target: ATtiny1614

# Microcontroller settings
MCU = attiny1614
F_CPU = 3333333UL

# Toolchain
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

# Project files
TARGET = midi2exp
SRC = src/main.c
OBJ = $(SRC:.c=.o)

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CFLAGS += -Os -Wall -Wextra
CFLAGS += -std=gnu99
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections

# Linker flags
LDFLAGS = -mmcu=$(MCU)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(TARGET).map

# Output formats
HEX = $(TARGET).hex
ELF = $(TARGET).elf
LSS = $(TARGET).lss

# Optional: Enable CC/channel filtering
# Uncomment to filter for specific CC number and channel
# CFLAGS += -DFILTER_CC

# Build targets
.PHONY: all clean size flash fuses

all: $(HEX) $(LSS) size

$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(LSS): $(ELF)
	$(OBJDUMP) -h -S $< > $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

size: $(ELF)
	@echo
	@echo "Size of sections:"
	$(SIZE) -C --mcu=$(MCU) $(ELF)

clean:
	rm -f $(OBJ) $(ELF) $(HEX) $(LSS) $(TARGET).map

# Programming targets (requires UPDI programmer)
# Adjust programmer settings for your setup

# Using pymcuprog (recommended for ATtiny)
flash: $(HEX)
	pymcuprog write -t uart -u /dev/ttyUSB0 -d $(MCU) -f $(HEX)

# Alternative: using SerialUPDI
flash-serialupdi: $(HEX)
	python3 -m serialupdi -d $(MCU) -c /dev/ttyUSB0 -b 57600 -f $(HEX)

# Set fuses (configure for external clock if needed)
# Default: internal 20MHz/6 = 3.33MHz
fuses:
	@echo "Using default internal oscillator (20MHz/6)"
	@echo "If external clock needed, modify FUSE2 (OSCCFG)"
