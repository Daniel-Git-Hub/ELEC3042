/*
 * File:   PracWeek2.c
 * Author: Daniel (459657598)
 * (pins are Arduino pin numbers)
 *  Buzzer     -> 11
 *  Green LED  -> 9
 *  Orange LED -> 8,10
 *  Red LED    -> 4-7, 12, 13
 *  Button     -> 2
 * Created on 4 March 2021, 8:49 AM
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


int delay =  62499; // represents 1s    


void init(){
    //set timer1 (which we use for delays and shifting the LEDs) to CTC with a prescalar of 256
    //require the use of 16 bit timer as it is the only timer with capacity to delay 0.5+ seconds 
    TCCR1A = 0; 
    TCCR1B = 0b00001100;
    TIMSK1 = 0b00000010;
    OCR1A = delay;
    
    //pin modes
    DDRB   = 0b00000011;  //make pin 8 and 9 as outputs
    DDRD   = 0b00000000;  //make sure that pin 2 is set to input
    PORTD  = 0b00000100; //turn on pullup resistor on pin 2
    
    //interupt for pin 2 (INT0))
    EICRA = 0b00000010; //trigger interupt on low level (could also use falling edge)
    EIMSK = 0b00000001; //enable interupt mask

    
    
    sei(); //globally enable all interupts

    //set up sleep modes
    set_sleep_mode(SLEEP_MODE_IDLE); //It is possible to turn off even more subroutines to save even more power 
    sleep_enable();                  //sets the global sleep flag
   
}

//ISR stands for Interupt Service Routine 
//TIMER1_COMPA_vect tells it that this routine is for for TIMER1_COMPA interupt
ISR(TIMER1_COMPA_vect){
    PORTB ^= 0b00000010 //toggles LED on B1 preserving the state of B0
}

ISR(INT0_vect){
    //NOTE that code to deal with button bouncing needs to be implemented as it is likely that one press of the button will generate more then one signal
    PORTB ^= 0b00000001; //toggles LED on B0 preserving the state of B1
}

int main(void) {
    init();
    
    while(1){ //when a ISR finishes it will go back here (or the line below) and put the chip back to sleep
       sleep_cpu(); //every time a interupt finishes turn off most functions by idling (prevents busy looping). Importantly this keeps both the hardware interupts and the timers on
    }
}
