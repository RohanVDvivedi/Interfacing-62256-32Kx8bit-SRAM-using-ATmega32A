/*
 * my1stAtmelstudioproject.cpp
 *
 * Created: 5/26/2016 11:20:02 AM
 * Author : devshri
 */ 

#define F_CPU 1000000               // I am using ATmega32A @ 1MHz internal clock

#include <avr/io.h>
#include <util/delay.h>

enum as{ InputPort, OutputPort };   // to get east going with setting the Data Bus Input and Output again and again
void setDataBusAs(as Portvalue)     // pass variable as to set the Data Bus port direction
{
	switch (Portvalue)
	{
		case InputPort :
			DDRB = 0x00;
			break;
		case OutputPort :
			DDRB = 0xFF;
	}
}

void writeDataBus(unsigned char data) // Write Data Bus with a unsigned char AKA byte data 
{
	PORTD&=(~(1<<DDRD6));      // for writing to the RAM the WE-bar pin should be LOW in my case i used pin 6 of PORT D
	_delay_ms(1);              // the RAM response time is 70ns , but i dont need any faster    -- change it as your wish
	PORTB = data;              // I use full PORT B as my data bus
	PORTD|=(1<<DDRD6);         // write 1 to the WE-bar pin to put the RAM in read mode
}

unsigned char readDataBus()
{
	_delay_ms(1);              // wait for some unknown time - RAM response time is 70ns - change this as you wish
	return PINB;               // return the data read on PORT B - PORT B is my Data Bus
}

void writeAddress(unsigned int addr)          // write an unsigned int address to the Address bus
{
	if(addr <= 0xFFFF)                        // write Address Bus only if the address bus is 16 bits or less in size
	{
		PORTA = (addr & 0x00FF);              // write the lower 8 bits on the PORTA ---- A7 - A0 = PORTA7 - PORTA0
		PORTC = ( (addr & 0xFF00) >> 8 );     // write the higher byte to thee PORTC ---- A14- A8 = PORTC6 - PORTC0
	}
}

/*

	data format :
		reading data -
				R-<address>;
		writing data -
				W-<data>-<address>;

*/

// this method reads UART register as many times it is written until it receives ';' at the end,, 
// and writes all characters read to input array integer pointer
void readUART(char* c)
{
	
	char temp=' ';int i=0;
	while(temp!=';' && i<15 )
	{
		while( !(UCSRA & 0b10000000) ){}
		c[i]=UDR;
		temp = c[i++];
	}
}

// this method writes UART communication, the unsigned character array pointed by c as long as ';' is reached
// then sends ';' and '\n' at the end.  -- kind of trivial string formatting
void writeUART(unsigned char* c)
{
	int i=0;
	while( c[i]!=';')
	{
		while( (UCSRA & 0b00100000) != 0b00100000 ){}
		UDR = c[i++];
	}
	while( (UCSRA & 0b00100000) != 0b00100000 ){}
	UDR = ';';           
	while( (UCSRA & 0b00100000) != 0b00100000 ){}
	UDR = '\n';
}

char HighCommand[15];                                  // received string is stored in this array and then decoded
void formCharData(int,unsigned char*);                 // func definaed later

// this method starts reading a unsigned characted array only from a position pointed by array pointer n
// pass the iterators address where you expect a ascii encoded number 
// if c[]="vaevev1456nwvnjp" and *n=6 then the function return with 1456 and the iterator value updated to 10
int getNumber(char* c,int* n)              
{
	unsigned int result=0;
	while(c[(*n)]<='9' && c[(*n)]>='0')
	{
		result=result*10 + c[(*n)++]-'0';
	}
	return result;
}

// this methis turns data=154 to c[] = "Data= 154;"
void formCharData(int data,unsigned char* c)
{
	c[0]='D'; c[1]='a'; c[2]='t'; c[3]='a'; c[4]='='; c[5]=' ';
	// i am sure there is a better way to encode the integer to ascii string -- you can use your algo
	c[6]=data/100;
	c[7]=(data/10) - (c[6]*10);
	c[8]=data - c[6]*100 - c[7]*10;
	c[6]+='0'; c[7]+='0'; c[8]+='0';
	c[9]=';';
}

// in the acquired string from the computer decode the instruction to be performed regarding RAM and execute it
void decodexecute(char* c)
{
	unsigned int addr;unsigned char data;int i=2;
	switch (c[0])
	{
		case 'R':
			addr = getNumber(c,&i);
			writeAddress(addr);          // write address
			setDataBusAs(InputPort);     // set data bus to input
			data=readDataBus();          // read the value to data bus
			
			unsigned char arr[10];
			formCharData(data,arr);      // form output printable string in arr
			writeUART(arr);              // write data to console
			
			break;
			
		case 'W':
			data = getNumber(c,&i);
			i++;
			addr = getNumber(c,&i);
			writeAddress(addr);          // write address value to address bus
			setDataBusAs(OutputPort);    // first set data bus as output
			writeDataBus(data);          // write data value to data bus
			writeUART(((unsigned char*)"data writing successfull;"));
			break;
	}
}



int main(void)
{
	/* setting pins */
	DDRB = 0xFF;   // data bus
	DDRA = 0xFF;   // Address A0-A7
	DDRC = 0xFF;   // Address A8-A14
	// D register is used as control register for our RAM
	DDRD|= 0b11111100;   // 4 = chipselect = CS-bar  ,  5 = outputenable = OE-bar  ,  6 = Writeenable = WE-bar
	PORTD = 0;
	PORTD &= (0b01000011); PORTD|=0b01000000; // write CS and OE to low and WE to high to set RAM to read mode
	
	/* UART communication initialization */
	UBRRL = 0b00001100;    // baudrate is 4800 depends on your F_CPU i.e. your clock speed
	UBRRH = 0;
	UCSRC = 0b10000110;    // data size set to 8 bits
	UCSRB = 0b00011000;    // receiver and transmitter enabled all interrupts disabled
	
	
    while (1) 
    {
		readUART(HighCommand);      // read the coomand
		decodexecute(HighCommand);  // decode and execute the command
    }
}