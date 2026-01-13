# Circuit Schematic

## MIDI Input Stage

### H11L1M Optocoupler Circuit

```
MIDI IN (TRS Jack - Type A)
┌──────────────┐
│  Tip    ─────┼──── 220Ω ──── H11L1M Pin 1 (Anode)
│              │                    │
│  Ring   (NC) │               1N4148 (Protection diode)
│              │                    │ (Cathode to Pin 1)
│  Sleeve ─────┼──────────────── H11L1M Pin 2 (Cathode)
└──────────────┘                    │
                                    └─────── GND

H11L1M Connections:
Pin 1: Anode (via 220Ω from MIDI Tip)
Pin 2: Cathode (to MIDI Sleeve/GND)
Pin 3: NC
Pin 4: VCC (+5V)
Pin 5: GND
Pin 6: Output ── 10kΩ pullup to VCC ── ATtiny1614 PA1 (USART0 RX)
```

**Important Notes:**
- H11L1M output is **inverted** (idle HIGH, data LOW)
- Firmware configures UART for inverted RX
- 1N4148 diode protects optocoupler from reverse voltage
- Verify TRS Type A pinout (Tip=Current Source, Sleeve=Current Sink)

## ATtiny1614 Connections

```
                    ATtiny1614 (SOIC-14)
                    ┌─────────────┐
       VCC (5V) ────┤ 1  VDD  GND ├─ 14 ── GND
  MIDI RX (PA1) ────┤ 2  PA1  PA0 ├─ 13 ── UPDI (Programming)
             NC ────┤ 3  PA2  PA7 ├─ 12 ── NC
             NC ────┤ 4  PA3  PA6 ├─ 11 ── DAC OUT (Expression)
             NC ────┤ 5  PA4  PA5 ├─ 10 ── NC
             NC ────┤ 6  PA5  PA4 ├─  9 ── NC
             NC ────┤ 7  PA7  PA3 ├─  8 ── NC
                    └─────────────┘

Key Pins:
- PA0 (Pin 13): UPDI programming interface
- PA1 (Pin 2):  USART0 RX - MIDI input from H11L1M
- PA6 (Pin 11): DAC0 output - Expression voltage
```

## Expression Output Stage

```
Expression Output (3.5mm TRS Jack)
┌──────────────┐
│  Tip    ─────┼──── (Optional 10kΩ series) ── ATtiny PA6 (DAC OUT)
│              │
│  Ring   ─────┼──── VCC (3.3V or 5V, depending on pedal)
│              │
│  Sleeve ─────┼──── GND
└──────────────┘
```

**Important Notes:**
- Verify your pedal's expression input requirements
- Common pinouts:
  - **Type 1**: Tip=Wiper, Ring=VCC, Sleeve=GND
  - **Type 2**: Tip=VCC, Ring=Wiper, Sleeve=GND
- Adjust wiring accordingly for your pedal
- Optional 10kΩ series resistor protects DAC output

## Power Supply

```
DC Barrel Jack (2.1mm, center-positive)
┌────────────┐
│ Center (+) ├──┬── 7805 Input
│            │  │
│ Sleeve (-) ├──┼── GND
└────────────┘  │
                │
           100µF (electrolytic)
                │
                ├─── 7805 Output ──┬── 0.1µF ── GND
                │                   │
                └─── GND            ├── VCC (+5V to circuit)
                                    │
                               0.1µF at ATtiny VCC
```

**Decoupling:**
- 100µF electrolytic at regulator input
- 0.1µF ceramic at regulator output
- 0.1µF ceramic at ATtiny VCC pin (close to IC)

## Optional: MIDI Activity LED

```
ATtiny PB0/PB1 (if using) ── 330Ω ── LED ── GND
```

Add LED blinking in firmware to indicate MIDI activity.

## Complete System Diagram

```
         ┌──────────────────────────────────────────────────┐
         │  Power Supply                                    │
         │  DC Jack → 7805 → +5V, GND                       │
         └──────────────────────────────────────────────────┘
                           │ +5V, GND
                           ↓
         ┌──────────────────────────────────────────────────┐
         │  MIDI Input                                       │
         │  TRS Jack → H11L1M → ATtiny PA1                  │
         └──────────────────────────────────────────────────┘
                           │
                           ↓
         ┌──────────────────────────────────────────────────┐
         │  ATtiny1614 (Firmware Processing)                │
         │  - UART at 31.25 kbaud                           │
         │  - Parse MIDI CC messages                        │
         │  - Map CC (0-127) to DAC (0-255)                 │
         └──────────────────────────────────────────────────┘
                           │
                           ↓
         ┌──────────────────────────────────────────────────┐
         │  Expression Output                                │
         │  ATtiny PA6 (DAC) → TRS Jack → Pedal             │
         └──────────────────────────────────────────────────┘
```

## PCB Layout Recommendations

1. **Keep optocoupler circuit isolated** from digital circuit
2. **Star ground** from power supply
3. **Short traces** from ATtiny VCC to decoupling cap
4. **Shielded cable** for MIDI input recommended
5. **Keep DAC output trace short** to minimize noise

## Testing Points

- **TP1**: H11L1M output (should pulse with MIDI data)
- **TP2**: ATtiny PA1 (verify MIDI reception)
- **TP3**: ATtiny PA6 (DAC output, should vary 0-5V)
- **TP4**: +5V rail (verify stable power)
