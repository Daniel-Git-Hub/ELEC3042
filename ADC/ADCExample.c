/*
 * File:   ADCExample.c
 * Author: Daniel Watson
 *
 * Created on 10 March 2021, 10:36 AM
 */

#define F_CPU 16000000UL // Defining the CPU Frequency

#include <avr/io.h>
#include <util/delay.h> //not allowed to use this program

void serial_init(unsigned long baudrate){ //Common baud rates are 9600 and 112500
    UBRR0 = ((F_CPU / (baudrate * 8UL))-1); //Set our Baud Rate Register using our prescalar formula
        
    UCSR0A |= 0b00100000; //Enable double speed mode, as discussed this is by default 1 anyway and what most serial readers expect
    
    UCSR0C  = 0b00000110; //Set it to transfer 8 bits per message (this is the default anyway). Note the third bit for dictating UCSZ0 is located in the UCSR0B register
    
    UCSR0B  = 0b00011000; //Enable the reiver and the transmitter
}

void serial_transmit(uint8_t DataByte)
{
	while (( UCSR0A & 0b00100000) == 0) {}; //Where is the 5th bit of UCSR0A is UDRE0 (USART Date Register Empty), we wait until the buffer is empty (this bit set) then transmit
	UDR0 = DataByte; //Set the transmit buffer to our value (remebering that we set our transmit size to 8 bits)
}

void ADC_init(){
    
    ADMUX  |= 0b01000000; //Set the reference voltage to be our 5V (AVcc)
    
    ADMUX  |= 0b00000011; //Set it to use ADC3 (Analog pin 3);

    ADMUX  |= 0b00100000; //For ADLAR mode = 1 (default is 0) (right shifted)

    ADCSRA |= 0b00000111; //Set to use 128 clock factor as per calculated
    
    ADCSRA |= 0b10000000; //Enable the ADC

    DDRB = 0b00111111; //set pins 8-13 as outputs
}

int main(void) {
    serial_init(112500); //can be seen in https://github.com/Daniel-Git-Hub/ELEC3042/wiki/Serial
    ADC_init();
    
    while (1) {
        ADCSRA |= 0b01000000; //sets the ADCS bit to start a conversion
        while(ADCSRA & 0b01000000){;} //waits until the conversion is finished
        serial_transmit(ADCL);   //transmits other serial the result of the low bits then the high bits   
        PORTB = ADCH >> 2; //show 6 most important bits on port B (remeber that we can't use PB7 and PB8)
        serial_transmit(ADCH);
        _delay_ms(1000); //from util/delay, waits 1 second
    }
}
