#include <avr/io.h>

void delay_init(){
    TCCR0A = 0b00000010; //CTC mode
    OCR0A  = 0b11111001; //249 (with a prescalar of 64 represents 0.001 seconds)
    TCCR0B = 0b00000000; //make sure the timer is off
    
}
void delay_ms(int ms){
    TCCR0B = 0b00000011; //turn timer on, using a prescaler of 64
    for(volatile int i = 0; i < ms; i++){
        
        TCNT0  = 0;          //reset the count to 0
        TIFR0 |= 0b00000010; //clear the flag register
        
        while((TIFR0 & 0b00000010) == 0){ //while the first bit is 0 (this is the bit of OCR0A compare)
            ; //busy loop
        }
        
    }
    TCCR0B = 0b00000000; //turn the timer off
}


int main(void) {
    delay_init();
    
    DDRB = 255; //set all pins as outputs, to see the output for this code connect LEDs to pins 8-13

    while(1){
        PORTB++; //increase the count (to show that something is happening once every second)
        delay_ms(1000);
    }
}
