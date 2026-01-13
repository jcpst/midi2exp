/*
 * MIDI to Expression Pedal Converter
 * Target: ATtiny1614
 * 
 * Receives MIDI CC messages via TRS input, outputs analog voltage
 * for guitar pedal expression inputs using built-in DAC.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

// Configuration
#define MIDI_BAUD_RATE 31250
#define F_CPU 3333333UL  // Default 3.33 MHz internal oscillator (20MHz/6)

// MIDI message defines
#define MIDI_CC_STATUS 0xB0  // Control Change base status (channel 0)
#define MIDI_CC_NUMBER 11    // Default CC number (Expression)
#define MIDI_CHANNEL 0       // Default MIDI channel (0-15)

// State machine for MIDI parsing
typedef enum {
    MIDI_WAIT_STATUS,
    MIDI_WAIT_DATA1,
    MIDI_WAIT_DATA2
} midi_state_t;

// Global variables
static volatile midi_state_t midi_state = MIDI_WAIT_STATUS;
static volatile uint8_t midi_channel = 0;
static volatile uint8_t midi_data1 = 0;

/*
 * Initialize UART for MIDI reception at 31.25 kbaud
 * Uses inverted RX due to H11L1M optocoupler inversion
 */
void uart_init(void) {
    // Configure PA1 as input for USART0 RX
    PORTA.DIRCLR = PIN1_bm;
    
    // Calculate baud rate register value
    // BAUD = (64 * F_CPU) / (16 * BAUD_RATE)
    uint16_t baud_setting = (64UL * F_CPU) / (16UL * MIDI_BAUD_RATE);
    
    // Set baud rate
    USART0.BAUD = baud_setting;
    
    // Configure for 8N1 (8 data bits, no parity, 1 stop bit)
    USART0.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;
    
    // Enable RX
    // Note: H11L1M optocoupler inverts the signal (idle HIGH, data LOW)
    // This matches standard UART idle-high state, so no RX inversion needed
    USART0.CTRLB = USART_RXEN_bm;
    
    // Enable RX complete interrupt
    USART0.CTRLA = USART_RXCIE_bm;
}

/*
 * Initialize DAC for analog output
 * Configured for 0-VDD output range
 */
void dac_init(void) {
    // Enable DAC output on PA6
    PORTA.DIR |= PIN6_bm;
    
    // Configure VREF for DAC (using VDD as reference for full 0-5V range)
    VREF.CTRLA = VREF_DAC0REFSEL_VDD_gc;
    
    // Enable DAC with output enabled
    DAC0.CTRLA = DAC_ENABLE_bm | DAC_OUTEN_bm;
    
    // Set initial output to 0
    DAC0.DATA = 0;
}

/*
 * Set DAC output value
 * value: 0-255 (8-bit DAC)
 */
void dac_set(uint8_t value) {
    DAC0.DATA = value;
}

/*
 * Map MIDI CC value (0-127) to DAC output (0-255)
 */
uint8_t midi_to_dac(uint8_t midi_value) {
    // Precise linear mapping to utilize full DAC range
    // MIDI 127 -> DAC 255, MIDI 0 -> DAC 0
    return ((uint16_t)midi_value * 255) / 127;
}

/*
 * Process received MIDI byte
 */
void process_midi_byte(uint8_t byte) {
    // Status byte (MSB set)
    if (byte & 0x80) {
        // Check if it's a Control Change message
        uint8_t status = byte & 0xF0;
        if (status == MIDI_CC_STATUS) {
            midi_channel = byte & 0x0F;
            midi_state = MIDI_WAIT_DATA1;
        } else {
            midi_state = MIDI_WAIT_STATUS;
        }
    }
    // Data byte
    else {
        switch (midi_state) {
            case MIDI_WAIT_DATA1:
                // First data byte is CC number
                midi_data1 = byte;
                midi_state = MIDI_WAIT_DATA2;
                break;
                
            case MIDI_WAIT_DATA2:
                // Second data byte is CC value
                // Filter for specific CC number and channel (optional)
                #ifdef FILTER_CC
                if (midi_data1 == MIDI_CC_NUMBER && midi_channel == MIDI_CHANNEL)
                #endif
                {
                    // Update DAC output
                    uint8_t dac_value = midi_to_dac(byte);
                    dac_set(dac_value);
                }
                midi_state = MIDI_WAIT_STATUS;
                break;
                
            default:
                midi_state = MIDI_WAIT_STATUS;
                break;
        }
    }
}

/*
 * UART RX interrupt handler
 */
ISR(USART0_RXC_vect) {
    uint8_t byte = USART0.RXDATAL;
    process_midi_byte(byte);
}

/*
 * Main function
 */
int main(void) {
    // Disable interrupts during initialization
    cli();
    
    // Initialize UART for MIDI
    uart_init();
    
    // Initialize DAC for expression output
    dac_init();
    
    // Enable global interrupts
    sei();
    
    // Main loop (everything handled in interrupts)
    while (1) {
        // Intentionally empty - all processing done in ISR
        // Power saving could be added here if needed
    }
    
    return 0;
}
