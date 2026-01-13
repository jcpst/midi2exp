# MIDI to Expression Pedal Converter

Receives MIDI CC messages via TRS input, outputs analog voltage for guitar pedal expression inputs.

## Features

- ✅ Hardware UART for reliable 31.25 kbaud MIDI reception
- ✅ Built-in 8-bit DAC for analog output (no external DAC needed)
- ✅ Interrupt-driven MIDI parsing
- ✅ Linear mapping of MIDI CC values (0-127) to DAC output (0-255)
- ✅ Optional filtering for specific CC number and MIDI channel
- ✅ Minimal firmware footprint for ATtiny1614

## Hardware

### Microcontroller

**ATtiny1614** (SOIC-14)

- 16 KB flash, 2 KB RAM
- Hardware UART (31.25 kbaud MIDI)
- Built-in 8-bit DAC (direct analog output)
- Requires SOIC-to-DIP adapter for breadboard prototyping

### MIDI Input Circuit

**H11L1M** schmitt-trigger optocoupler

```
TRS Tip ──── 220Ω ──── Pin 1 (anode)
TRS Sleeve ────────── Pin 2 (cathode)
1N4148 across pins 1–2 (cathode to pin 1)

Pin 4 (VCC) ───────── +5V
Pin 5 (GND) ───────── GND
Pin 6 (output) ─┬──── 10kΩ pullup to VCC
                └──── ATtiny PA1 (RX pin)
```

**Note:** Output is inverted. Verify TRS Type A vs B pinout for your MIDI source.

### Expression Output

**3.5mm TRS jack**

- Tip: DAC output on PA6 (via optional 10kΩ series resistor)
- Ring: VCC (3.3V or 5V depending on pedal)
- Sleeve: GND

**Note:** Verify your pedal's expression input pinout—varies by manufacturer.

### Power Supply

- DC barrel jack (2.1mm center-positive)
- 5V regulated (7805 or LDO if input >5V, or direct 5V supply)
- 100µF + 0.1µF decoupling at regulator
- 0.1µF decoupling at ATtiny VCC pin

## Bill of Materials

| Component    | Value/Part         | Qty |
|--------------|-------------------|-----|
| MCU          | ATtiny1614        | 1   |
| Adapter      | SOIC-14 to DIP    | 1   |
| Optocoupler  | H11L1M            | 1   |
| Diode        | 1N4148            | 1   |
| Resistor     | 220Ω              | 1   |
| Resistor     | 10kΩ              | 2   |
| Capacitor    | 100µF electrolytic| 1   |
| Capacitor    | 0.1µF ceramic     | 2   |
| Regulator    | 7805 or LDO       | 1   |
| Jack         | 3.5mm TRS (MIDI)  | 1   |
| Jack         | 3.5mm TRS (expr)  | 1   |
| Jack         | DC barrel 2.1mm   | 1   |

**Optional:**

- LED + 330Ω resistor (MIDI activity indicator)
- TVS diode / polyfuse (power protection)

## Building the Firmware

### Prerequisites

- AVR-GCC toolchain
- avr-libc
- UPDI programmer (or USB-serial adapter for SerialUPDI)

### Compile

```bash
make
```

This produces `midi2exp.hex` ready for flashing.

### Optional: Enable CC/Channel Filtering

To filter for a specific CC number and MIDI channel, uncomment the line in `Makefile`:

```makefile
CFLAGS += -DFILTER_CC
```

Then modify `src/main.c` to set your desired CC number and channel:

```c
#define MIDI_CC_NUMBER 11    // CC number (e.g., 11 for Expression)
#define MIDI_CHANNEL 0       // MIDI channel (0-15)
```

## Programming

The ATtiny1614 uses UPDI (Unified Program and Debug Interface), a single-wire interface.

### Using pymcuprog

```bash
make flash
```

### Using SerialUPDI

```bash
make flash-serialupdi
```

**Note:** Adjust the serial port (`/dev/ttyUSB0`) in the Makefile to match your system.

### UPDI Connection

Connect your UPDI programmer to the ATtiny1614 UPDI pin:

- UPDI pin on ATtiny1614 (pin 13 on SOIC-14, PA0)
- GND
- VCC (5V)

## How It Works

1. **UART Initialization**: Configures USART0 at 31.25 kbaud with inverted RX for H11L1M optocoupler
2. **DAC Initialization**: Sets up built-in 8-bit DAC on PA6
3. **MIDI Parsing**: State machine parses incoming MIDI bytes, looking for Control Change messages
4. **Value Mapping**: CC values (0-127) are linearly mapped to DAC output (0-255)
5. **Interrupt-Driven**: UART RX interrupt ensures reliable MIDI message reception

## Pinout Reference

| Pin | Function        | Description                    |
|-----|----------------|--------------------------------|
| PA1 | USART0 RX      | MIDI input from H11L1M         |
| PA6 | DAC0 OUT       | Expression output              |
| PA0 | UPDI           | Programming interface          |

## Troubleshooting

### No MIDI response
- Check TRS wiring (Type A vs Type B)
- Verify H11L1M connections and orientation
- Test MIDI source with another device

### Expression output not working
- Verify pedal expression input pinout
- Check DAC output with multimeter (should vary 0-5V with CC input)
- Ensure proper power supply voltage

### Programming issues
- Verify UPDI connection
- Check ATtiny1614 power (5V on VCC, GND connected)
- Try lower baud rate for SerialUPDI

## License

This project is open source. Feel free to modify and distribute.
