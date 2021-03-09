# Timers, Counting, Frequency, Flags and Output.

All a timer is a register (read variable) that counts up and then resets back to 0 once it meets a criteria.
The ATMEGA328p has three timers Timer 0 and 2 are 8 bit and Timer 1 is 16 bit. This means that the 8 bit timers can count from 0-255 and the 16 bit timer from 0-65535

A lower case "x" refers to that this is the same register accross all three timers and just replace the "x" with the timer number that you are using.

Each timer also has two ports associated with it and for each port there is a compare register which is the same size of the counter (8 bit for timer 0 and 2 and 16 bit for timer 1)

| Timer | OC0A Arduino Pin | OC0B Arduino Pin |
| --- | --- | --- |
| Timer 0 | 6 | 5 |
| Timer 1 | 9 | 10 |
| Timer 2 | 11 | 3 |


## Part 1) Frequency of Count

Firstly I'll be refering to various Clock Select Modes.
To set this mode for a timer you need to to set the corresponding bits in the B Control Register (TCCRxB) where the 3 most insighnificant bits represent the Clock Select. The table for these modes can be found on page 117 for timer 0, page 143 for timer 1 and page 166 for timer 2

This number represents the "prescalar" for the arduino clock.

An Arduino Uno runs at 16 mHz (16,000,000 Hz), this means at base speed the counter would increase once every 0.0625 microseconds (period = 1/frequency). 

*Where does 16mHz come from?*
*This is defined by an external (that is not part of the ATMEGA328 chip) oscillator on the Arduino Uno board hence why you won't find this in the datasheet for ATMEGA328p*
*To be precise an Arduino Uno has two oscillators, 1 crystal oscillator which is extremely precise, due to USB serial having tighter tolerances, hocked up to the bootloader chip and 1 ceramic oscillator, which less precise but cheaper, hocked up to the ATMEGA328p. Also note that this does NOT matter*


The problem with this is that if we let the counter tick up once every clock cycle the timers will overflow extremely quickly, 16 microseconds for timer 0 and 2 (derived from 2^8/16,000,000) and 4 milliseconds for timer 1 (calculated from 2^16/16,000,000). This isn't practical for alot of uses. This is where the prescalar comes in, it slows down the clock by that multiple.
For example if the prescalar is set to 64 then we divide the Arduino clock by that amount to get the new frequency, 16,000,000/64 = 250,000 Hz (once every 4 microseconds)

Notes if Clock select is set to 000 then ATMEGA328 won't count (this could be thought of as turning this timer off)
Timer 0 and Timer 1 can also be incremented from an external source instead of chips clock (pins T0 and T1, which maps to Arduino pins 4 and 5) (either on a falling or rising edge). This could be useful in a number of application such as
A much slower oscillator/oscillating circuit (for example 1 Hz).
Or to conenct it to sensor and only do something every X readings


Important take away is that the timers counter increases once every N/16,000,000 seconds (where N is the prescaler)


## Part 2) Pin modes

Using the Timer Control Register A you can set modes for what happens to the physical pin. Most of the details can be seen on pages 113-114 for Timer 0, pages 140-141 for Timer 1 and pages 162-163 for Timer 2
The important thing to note is that these registers do different things depending on mode (see Part 3).
Terminiolgy that it uses
BOTTOM refers to when the timer overflows and the counter is 0 
TOP depends on the Waverform mode of the timer, to find it look in the TOP column of the Waverform Mode Table. This is the value in which the timer will reset when it hits
SET refers to turning the pin on (even if it is already on)
CLEAR refers to turning the pin off
TOGGLE refers to changing the state, turning it on if it is off and turing the pin off it is on.

Note the corresponding pin needs to be set as an output using DDRx (Data direction register) (see chapter 14 in the Data sheet) for it to be used in this fashion


## Part 3) Waveform Modes
### A) How to set the modes
These modes are detailed in the tables located at the end of each of the timer chapters in the datasheet.
The complicating factor is that the bits decribing the modes (WGMx, Waveform Generation Mode) is split across BOTH the TCCRxA and TCCRxB registers   


### B) Normal Mode
This is quite similar to fast PWM, just without the extra pin modes.
It counts up until it hits the overflow (either 0xff for 8 bit or 0xffff for the 16 bit) and then resets back to 0


### C) CTC Mode 
This is that I like the best for delays (using the timer without any physical pins) and buzzers. It works by counting up until it hits the compare register for pin A (OCRxA) then reseting back to 0
Commonly you will either toggle the physical pin on this match or run an interupt (perhaps even both)

Case studies  

Toggling an ouptut.
Becuase of the nature that it will count up then turn on, and then it will count up the same amount and turn it off this mode will ALWAYS have a 50% duty cycle on its A port

As discussed in Part A the timer will count upwards at a f=frequency of 16MHz/N (where N is the prescalar) and as seeing that it takes OCRxA+1 steps to reach the count (the + 1 is becuase we start the counting at 0) the port will toggle at a every

t = (1/f) * (OCRxA+1) seconds which equals a frequency of 16MHz/(N*(OCRxA + 1)) 

But for some of our applications such as a SPEAKER it will generate the sound corresponding to the frequency that it turns on. This means that it cares about the frequency that it turns on, and as it takes double the amount of time to turn on then it takes to toggle. This corresponds to half the frequency (as frequency = 1/period)

The speakers will generate a tone dictated to the frequency that it is turned on f = 16Mhz/(2*N*(OCRxA+1))

Code for the next two examples: https://github.com/Daniel-Git-Hub/ELEC3042/blob/main/Timer/CTCExample.c

Example say we want to play a tone at 440 Hz using a prescaler of 256 on timer 2 then

440 = 16,000,000/(2*256*(OCR0A + 1))
ORC2A+1 = 16,000,000/(2*256*440)
ORC2A+1 = 71.0
therefore ORC2A = 70 for a tone frequency of 440Hz, this value is allowed in the 8 bit timer as 70 < 2^8 (note 2^8 = 256)
The relevant code can be seen in the "init_CTC_BuzzerExample()" function

Using it as a delay

Assuming that you have it set so that an interupt is called whenever it matches OCRxA
It will calls this interupt at a frequency of 16MHz/(N*(OCRxA + 1))
With a an interupt call happening every t = (N*(OCRxA+1))/16MHz

Example, say we want to increment PORTB every one second, in this case we will user Timer 1 as it is the only one large enough. We will use a prescalar of 256

1Hz = 16Hz/(256 * (ORCA1A + 1))
ORC1A + 1 = 62500
ORC1A = 62499, this value is allowed as 62499 < 2^16  (note 2^16 = 65,536)
The relevant code can be seen in the "init_CTC_InteruptExample()" and "ISR(TIMER1_COMPA_vect)"


### D) Fast PWM Mode

This is that works best at adjusting the duty cycle of an output. 

*Note that duty cycle is a percentage representing the amount of time it is on verse the time it is off. For example a duty cycle of 80% would represent the pin being on for 48 seconds and off for 12 seconds in a one minute time scale. Typically we only keep it on and off for tiny amounts of time*

The way Fast PWM Mode works is it the counter counts up from the bottom to the defined TOP 
With different Fast PWM Modes have different tops, for example timer 0's Fast PWM mode's TOP can be configered to be either OCR0A OR 255 (the max value for this count)

When it reaches this value it resets back to 0 and starts counting again.

Code for this example: https://github.com/Daniel-Git-Hub/ELEC3042/blob/main/Timer/PWMExample.c

Example turn on an LED for 70% of the time. We will use timer 0 and OCA (Arduino pin 6) with a prescaler of 64.

For this we will set the COM0A (Compare Match Output A Mode, two most signifcant bits of TCCR0A) to be non-inverting mode. This will cause the pin to turn on when it the counter is at the bottom and the pin will turn off when the counter reaches the OCR0A.
This will mean that the timer will be on between 0 and OCR0A and off between OCR0A and 2^8-1 (255)

therefore (OCR0A - 0)/(OCR0A - 0 + 255 - OCR0A) = 0.7 

*time on/(time on + time off) = percentage of time on*

therefore OCR0A/255 = 0.7 -> OCR0A = 256*0.7 = 178

### E) Phase Correct PWM Mode

Similar to Fast PWM except instead of reseting the count back to 0 when it reaches its top it will instead start counting down until it reaches 0, then it will start counting up again



## Part 4) Interrupts and Flags


