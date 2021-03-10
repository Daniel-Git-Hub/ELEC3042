#include <avr/io.h>

void serial_init(unsigned long baudrate){ //Common baud rates are 9600 and 112500
  UBRR0 = ((16000000UL / (baudrate * 8UL))-1); //Set our Baud Rate Register using our prescalar formula
      
  UCSR0A |= 0b00100000; //Enable double speed mode, as discussed this is by default 1 anyway and what most serial readers expect
  
  UCSR0C  = 0b00000110; //Set it to transfer 8 bits per message (this is the default anyway). Note the third bit for dictating UCSZ0 is located in the UCSR0B register
  
  UCSR0B  = 0b00011000; //Enable the reiver and the transmitter
}

void serial_transmit(uint8_t DataByte)
{
	while (( UCSR0A & 0b00100000) == 0) {}; //Where is the 5th bit of UCSR0A is UDRE0 (USART Date Register Empty), we wait until the buffer is empty (this bit set) then transmit
	UDR0 = DataByte; //Set the transmit buffer to our value (remebering that we set our transmit size to 8 bits)
}

int main()
{
	serial_init(115200); //initalise using our desired baud rate
	char str[] = "ELEC3042"; //the data we want to send in an array, (note that this is an array that is 9 characters long as it also has a hidden null 
	while (1)
	{
		for(int i = 0; i < sizeof(str)/sizeof(str[0]); i++){  //for each character
			serial_transmit(str[i]); //transmit our character (a character is 8 bits long
		}
		_delay_ms(2000); //a delay function defined elsewhere that waits a certain number of milliseconds
	}
	return 0;
}
