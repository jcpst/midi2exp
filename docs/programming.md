# Programming Guide

This guide covers programming the ATtiny1614 using UPDI (Unified Program and Debug Interface).

## UPDI Overview

The ATtiny1614 uses a single-wire UPDI interface for programming and debugging, replacing the older ISP/HVPP methods used in classic AVR chips.

### UPDI Pin

- **Pin 13** (PA0) on the ATtiny1614 SOIC-14 package
- Shares the PA0/RESET pin
- Requires special UPDI programmer or USB-serial adapter

## Hardware Setup

### Required Connections

```
Programmer          ATtiny1614 (SOIC-14)
──────────────      ────────────────────
UPDI    ────────────── Pin 13 (PA0/UPDI)
GND     ────────────── Pin 14 (GND)
VCC     ────────────── Pin 1  (VDD) - Optional, can power from circuit
```

**Important:**
- Ensure ATtiny1614 is powered (5V on VDD, GND connected)
- UPDI line requires 4.7kΩ - 10kΩ pull-up to VCC on some programmers
- Keep UPDI wire short to minimize noise

## Programming Methods

### Method 1: Dedicated UPDI Programmer (Recommended)

Use a dedicated UPDI programmer like:
- **Microchip MPLAB Snap**
- **Atmel-ICE**
- **JTAG2UPDI** (Arduino-based)

#### Using pymcuprog (Python-based tool)

Install pymcuprog:
```bash
pip3 install pymcuprog
```

Flash the firmware:
```bash
make flash
```

Or manually:
```bash
pymcuprog write -t uart -u /dev/ttyUSB0 -d attiny1614 -f midi2exp.hex
```

### Method 2: SerialUPDI (USB-Serial Adapter)

Convert any USB-serial adapter (CH340, FT232, etc.) into a UPDI programmer.

#### Wiring

```
USB-Serial          ATtiny1614
──────────          ──────────
TX  ───┬─────────── PA0 (UPDI)
       │
       └── 4.7kΩ ── RX
GND ────────────── GND
VCC ────────────── VDD (optional)
```

The 4.7kΩ resistor between TX and RX is critical for SerialUPDI to work.

#### Using SerialUPDI Python tool

Install:
```bash
pip3 install pyserial intelhex pyupdi
```

Flash:
```bash
make flash-serialupdi
```

Or manually:
```bash
python3 -m serialupdi -d attiny1614 -c /dev/ttyUSB0 -b 57600 -f midi2exp.hex
```

#### Troubleshooting SerialUPDI

- Try lower baud rates: `-b 19200` or `-b 9600`
- Ensure correct resistor value (4.7kΩ works best)
- Verify USB-serial adapter is 5V tolerant or use level shifter
- Check serial port permissions: `sudo usermod -a -G dialout $USER`

### Method 3: Arduino as UPDI Programmer (JTAG2UPDI)

Turn an Arduino Uno/Nano into a UPDI programmer.

1. Flash JTAG2UPDI firmware to Arduino
2. Connect Arduino to ATtiny1614:
   ```
   Arduino D6 ───── ATtiny PA0 (UPDI)
   Arduino GND ──── ATtiny GND
   Arduino 5V ───── ATtiny VDD
   ```

3. Use avrdude or pymcuprog with JTAG2UPDI protocol

## Fuse Configuration

The ATtiny1614 uses "fuses" to configure clock source, brown-out detection, etc.

### Default Configuration (Used by firmware)

- **Clock**: Internal 20 MHz oscillator divided by 6 = 3.33 MHz
- **BOD**: Disabled (can be enabled for production)
- **UPDI**: Enabled (GPIO disabled on PA0)

### Reading Fuses

```bash
pymcuprog read -t uart -u /dev/ttyUSB0 -d attiny1614 -m fuses
```

### Setting Fuses (if needed)

**Warning:** Incorrect fuse settings can brick your chip. Only change if necessary.

Example: Enable brown-out detection at 2.6V
```bash
pymcuprog write -t uart -u /dev/ttyUSB0 -d attiny1614 -m fuses -o 0x00:0x00
```

Refer to ATtiny1614 datasheet Section 6 for fuse details.

## Verification

After programming, verify the firmware was written correctly:

```bash
pymcuprog verify -t uart -u /dev/ttyUSB0 -d attiny1614 -f midi2exp.hex
```

## Debugging

The UPDI interface supports debugging with compatible tools:

### Using Atmel-ICE with GDB

1. Connect Atmel-ICE to UPDI pin
2. Start debugging session:
   ```bash
   avarice -g :4242 -P attiny1614 -I
   ```
3. Connect GDB:
   ```bash
   avr-gdb midi2exp.elf
   (gdb) target remote :4242
   (gdb) load
   (gdb) break main
   (gdb) continue
   ```

## Programming During Development

For rapid development cycles:

1. Build and flash in one command:
   ```bash
   make && make flash
   ```

2. Monitor serial output (if UART TX is connected):
   ```bash
   screen /dev/ttyUSB0 31250
   ```

3. Use UART for debugging output (requires TX connection):
   - Connect ATtiny PA2 to serial adapter RX
   - Add debug print statements in firmware

## Production Programming

For production batches:

1. **Pre-program fuses** if different from defaults
2. **Flash firmware** with verified .hex file
3. **Verify** flash contents
4. **Test** with actual MIDI input and measure DAC output

### Automated Testing Script

```bash
#!/bin/bash
# Flash and verify
pymcuprog write -t uart -u /dev/ttyUSB0 -d attiny1614 -f midi2exp.hex
pymcuprog verify -t uart -u /dev/ttyUSB0 -d attiny1614 -f midi2exp.hex

if [ $? -eq 0 ]; then
    echo "✓ Programming successful"
else
    echo "✗ Programming failed"
    exit 1
fi
```

## Common Issues

### "Device not found"
- Check UPDI connection
- Verify ATtiny is powered
- Try different USB port
- Check permissions on /dev/ttyUSB0

### "Verification failed"
- Re-flash with lower baud rate
- Check power supply stability
- Verify .hex file is not corrupted

### "UPDI enable failed"
- Check 4.7kΩ resistor on SerialUPDI
- Ensure correct voltage levels (5V)
- Try dedicated UPDI programmer

## References

- [ATtiny1614 Datasheet](https://www.microchip.com/wwwproducts/en/ATtiny1614)
- [pymcuprog Documentation](https://github.com/microchip-pic-avr-tools/pymcuprog)
- [SerialUPDI Guide](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)
