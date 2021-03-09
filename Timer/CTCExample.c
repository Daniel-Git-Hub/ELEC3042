#include <avr/io.h>
#include <avr/interrupt.h>

void init_CTC_BuzzerExample(){
    DDRB   |= 0b00001000; //Set OC2A to be output
    
    TCCR2A  = 0b01000000; //Toggle OC2A (Arduino pin 11, or PB3) on Match
    TCCR2A |= 0b00000010; //Set Waveform mode to CTC 
    TCCR2B  = 0b00000110; //Set prescaler for the clock to be 256
    
    OCR2A = 70;           //Our calculated delay    
}


void init_CTC_InteruptExample(){
    //READ THE DATA SHEET to know the extact bit ordering, hint at the end of each chapter it gives a list of each of the registers
    
    DDRD   |= 0b11111111; //enable all of ports D as output (Arduino pins 0-7), note for serious applications we should never use pins 0 and 1 in this way
    
    TCCR1A |= 0b00000000; //OC1A and OC1B are disconnected (so all the COM1A and COM1B are clear) and the WGM11 and WGM10 (least two significant bits of waveform ) are both cleared to 0 for CTC mode
    TCCR1B  = 0b00001000; //Set WGM12 to make the waverform mode CTC
    
    TCCR1B |= 0b00000100; //Set the prescaler to 256
    
    TIMSK1 |= 0b00000010; //Set interrupt enable for on a OCR1A match (that is every time the counter reaches OCR1A the ISR with this interupt name, "TIMER1_COMPA_vect", as an argument is called
    
    OCR1A = 62499; //Set to our calculated delay, note that the compiler will take care of setting to both the high and low register so it is sufficient  in C to just write our 16 bit number
    //We could alternatively do this 
    //OCR1AH = 62499 >> 8; //set the 8 high bits (We MUST set the high bits before the low bits)
    //OCR1AL = 62499;      //set the 8 low bits (note that as we are dumping a 16 bit value into a 8 bit field the 8 most significant bits will just be ignored)
    
    
    sei(); //Globally enable all interrupts
}
ISR(TIMER1_COMPA_vect){
    PORTD++; //count up using the PORTD pins to show the output (hook up LEDs to Arduino pins 0-7) 
}


int main(void) {
    //call our setup functions 
    init_CTC_BuzzerExample();
    init_CTC_InteruptExample();

    while (1) { //do nothing in the loop as everything is handled with interrupts and automatic toggling
        ;
    }
}
