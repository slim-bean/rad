MCU      = atmega169
F_CPU    = 1000000UL
PROG     = avrisp2

CC       = avr-gcc
OBJCOPY  = avr-objcopy
OBJDUMP  = avr-objdump
SIZE     = avr-size
AVRDUDE  = avrdude

TARGET   = geiger

CFLAGS   = -mmcu=$(MCU) -DF_CPU=$(F_CPU) \
           -Os -std=gnu99 -Wall -Wextra -Wno-unused-parameter \
           -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
           -ffunction-sections -fdata-sections

LDFLAGS  = -mmcu=$(MCU) -Wl,--gc-sections

SRC      = main.c lcd_driver.c lcd.c button.c timer0.c click.c geiger.c
OBJ      = $(SRC:.c=.o)

all: $(TARGET).hex size

$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

size: $(TARGET).elf
	$(SIZE) --mcu=$(MCU) --format=avr $<

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROG) -p $(MCU) -U flash:w:$<:i

disasm: $(TARGET).elf
	$(OBJDUMP) -d -S $< > $(TARGET).lss

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex $(TARGET).lss

.PHONY: all flash size disasm clean
