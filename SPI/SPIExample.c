/*
 * File:   Briefing_SPI.c
 * Author: Rex di Bona
 * All I have modified is I added a bunch of delays
 * Created on 28 January 2021, 2:19 PM
 */


#include <xc.h>
#include <stdint.h>


int8_t SPI_transfer(int8_t data) {
    
    SPDR = data;
    while ((SPSR & _BV(SPIF)) == 0) {
        ;   // wait until transfer completed
    }
    return SPDR;
}

void SPI_Send_Command(uint8_t reg, uint8_t data) {
    // Send a command + byte to SPI interface
    PORTB &= ~_BV(2);    // SS enabled (low))
    SPI_transfer(0x40);
    SPI_transfer(reg);
    SPI_transfer(data);
    PORTB |= _BV(2);    // SS disabled (high)
}

uint8_t SPI_Read_Command(uint8_t reg) {
    uint8_t data;
    
    // Read a command output from SPI interface
    PORTB &= ~_BV(2);    // SS enabled (low))
    SPI_transfer(0x41);
    SPI_transfer(reg);
    data = SPI_transfer(0);
    PORTB |= _BV(2);    // SS disabled (high)
    return data;
}

void setup_SPI() {
    DDRB =   0b00101111;
	PORTB |= 0b00000100;		// set SS signal high
/*
 * Setup SPI operations
 * See pp176-177 of the data sheet
 */
	SPCR = _BV(SPE)|_BV(MSTR);   // set master SPI, SPI mode 0 operation
	SPSR = 0;                    // clear interrupt flags and oscillator mode.

//
// Now that the SPI interface is configured we need to send SPI commands to configure the MCP23S17
// port expander IC
//
// We will configure port A as all outputs, port B as all inputs.
// use the SPI_send_command to send the SPI commands.
// This will require the register address in r20 and the register data in r21
//
    SPI_Send_Command(0x00, 0x00);   // register IODIRA (port A data direction)
    SPI_Send_Command(0x01, 0xf0);   // register IODIRB (port B data direction)
    SPI_Send_Command(0x0d, 0xff);   // register GPPUB (port B GPIO Pullups)
}

void delay_ms(long num) {
    while (num--) {
        for (volatile long x = 0; x < 468; x++) {
            ;
        }
    }
}

int main(void) {
    uint8_t count = 0;
    
    setup_SPI();
    while (1) {
        SPI_Send_Command(0x14, count);
        SPI_Send_Command(0x15, count);
        count++;
        delay_ms(100);
        while ((SPI_Read_Command(0x13) & _BV(7)) == 0) {
            ;
        }
        if ((~SPI_Read_Command(0x13)) & 0x70) {
            delay_ms(400);
        }
    }
}