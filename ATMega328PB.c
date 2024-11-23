#include <avr/io.h>
#include <util/delay.h>

void CLKinit()
{
	CLKPR = 0x80; // You must set this register to 0x80 to be able to change it's value
	CLKPR = 0x04; // value 0x04 means 1/16 oscilator's frequency which is 1Mhz
}

void SpiMasterInit()
{
	PRR0 &= ~(1<<PRSPI0); // Power Reduction SPI bit must be set to 0
	DDRB |= (1<<2) | (1<<3) | (1<<5);//Selecting slave select, mosi and sck as output
	DDRB &= ~(1<<4); // miso as input
	SPCR0 |= (1<<6) | (1<<5) | (1<<4);// enabling SPI, LSB data order, selecting device as master
	
	SPCR0 |= (1<<0); //SPR00 = 1 needed to set the clock rate
	SPCR0 &= ~(1<<1); //SPR01 = 0 needed to set the clock rate
	SPSR0 &= ~(1<<0);// SPI2X0 = 0 resulting in fosc/16 clock rate
	SPCR0 &= ~(1<<3) & ~(1<<2); // setting CPOL and CPHA to 0, leading edge - rising, sampling - on leading edge
}
void SpiMasterTransmit(char Data) //could change later char to uint8_t
{
	SPDR0 = Data;//starting transmision
	while(!(SPSR0 & (1<<SPIF)));// Waiting for transmission complete
}

int main(void)
{
	CLKinit();
	SpiMasterInit();
}
