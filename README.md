# MIDI to Expression Pedal Converter

Receives MIDI CC messages via TRS input and outputs analog voltage for guitar pedal expression inputs.

## Hardware

### Microcontroller

**ATtiny1614** (SOIC-14)
- 16 KB flash, 2 KB RAM
- Hardware UART (31.25 kbaud MIDI)
- Built-in 8-bit DAC (direct analog output, no external DAC or filtered PWM needed)
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
                └──── ATtiny RX pin
```

**Note:** Output is inverted. Verify TRS Type A vs B pinout for your MIDI source.

### Expression Output

**3.5mm TRS jack**
- Tip: DAC output (via optional 10kΩ series resistor for protection)
- Ring: VCC (3.3V or 5V depending on pedal)
- Sleeve: GND

**Note:** Verify your pedal's expression input pinout—varies by manufacturer.

### Power

- DC barrel jack (2.1mm center-positive)
- 5V regulated (7805 or LDO if input >5V, or direct 5V supply)
- 100µF + 0.1µF decoupling at regulator
- 0.1µF decoupling at ATtiny VCC pin

## Bill of Materials

| Component    | Value/Part          | Qty |
|--------------|---------------------|-----|
| MCU          | ATtiny1614          | 1   |
| Adapter      | SOIC-14 to DIP      | 1   |
| Optocoupler  | H11L1M              | 1   |
| Diode        | 1N4148              | 1   |
| Resistor     | 220Ω                | 1   |
| Resistor     | 10kΩ                | 2   |
| Capacitor    | 100µF electrolytic  | 1   |
| Capacitor    | 0.1µF ceramic       | 2   |
| Regulator    | 7805 or LDO         | 1   |
| Jack         | 3.5mm TRS (MIDI in) | 1   |
| Jack         | 3.5mm TRS (expr out)| 1   |
| Jack         | DC barrel 2.1mm     | 1   |

**Optional:**
- LED + 330Ω (MIDI activity)
- TVS diode / polyfuse (power protection)

## Programming

- UPDI interface (single pin)
- Use dedicated UPDI programmer or SerialUPDI with USB-serial adapter

## Software Scope

1. Initialize UART at 31.25 kbaud
2. Initialize DAC output
3. Parse incoming MIDI bytes for CC messages
4. Map CC value (0–127) to DAC output (0–255)
5. Optional: filter for specific CC number / channel
