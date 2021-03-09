#include <avr/io.h>

void PWM_init(){

    DDRD    = 0b11111111; //Set OCA as an output
    
    TCCR0A  = 0b10000000; //Set OCA (Arduino pin 6 to be non-inverting, on at bottom and off on compare against OCR0A)
    
    TCCR0A |= 0b00000011; //Set the Fast PWM mode with TOP at 255 (max count)
    
    TCCR0B  = 0b00000100; //Set to a prescalar of 256
    
    OCR0A = 178; //Our calculate value which gives a duty cycle of 70%
    
}

int main(void) {
    PWM_init();

    while (1) {
        ;
    }
}
