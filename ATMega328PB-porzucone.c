/*
 * GccApplication1.c
 *
 * Created: 21.11.2024 14:12:57
 * Author : 48510
 */ 
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define CANCTRL 0x0f
#define CANINTF 0x2c
#define CNF1 0x2A
#define CNF2 0x29
#define CNF3 0x28
#define RXB0CNTRL 0x60
#define TXBxCTRL 0x30
#define RTS_TXB0 0x81
#define RTS_TXB1 0x82
#define RTS_TXB2 0x84
#define TXB0_SIDH  0x40  // Standard Identifier High Register
#define TXB0_DATA  0x41
#define TXB1_SIDH  0x42  // Standard Identifier High Register
#define TXB1_DATA  0x43
#define TXB2_SIDH  0x44  // Standard Identifier High Register
#define TXB2_DATA  0x45
#define RXB0DLC 0x65
#define RXB1DLC 0x75
#define RXBxSIDH 0x61
#define RXB0xDn 0x66 //receive buffer moze byc od 0x66 do 0x6D
#define RXB1xDn 0x76 // od 0x76 do 0x76


void CLKinit()
{
	CLKPR = 0x80; // You must set this register to 0x80 to be able to change it's value
	CLKPR = 0x04; // value 0x04 means 1/16 oscilator's frequency which is 1Mhz
}
void DiodesInit()
{
	DDRC |= (1<<0);
	DDRC |= (1<<1);
	DDRC |= (1<<2);
	DDRC |= (1<<3);
	DDRC |= (1<<4);
	DDRC |= (1<<5);
}
void SpiMasterInit()
{
	PRR0 &= ~(1<<PRSPI0); // Power Reduction SPI bit must be set to 0
	DDRB |= (1<<2) | (1<<3) | (1<<5);//Selecting slave select, mosi and sck as output
	DDRB &= ~(1<<4); // miso as input
	SPCR0 |= (1<<6) | (1<<4);// enabling SPI, selecting device as master
	SPCR0 &= ~(1<<5); //  MSB data order
	
	SPCR0 |= (1<<0); //SPR00 = 1 needed to set the clock rate
	SPCR0 &= ~(1<<1); //SPR01 = 0 needed to set the clock rate
	SPSR0 &= ~(1<<0);// SPI2X0 = 0 resulting in fosc/16 clock rate
	SPCR0 &= ~(1<<3); // setting CPOL to 0, leading edge - rising, 
	SPCR0 &= ~(1<<2);// setting CPHA to 0 sampling - on leading edge
}
uint8_t SpiMasterTransmit(uint8_t Data) //could change later char to uint8_t
{
	SPDR0 = Data;//starting transmision
	while(!(SPSR0 & (1<<SPIF)));// Waiting for transmission complete
	return SPDR0;
}
void SpiWrite(uint8_t reg, uint8_t value)
{
	PORTB &= ~(1<<2); // activating slave select
	SpiMasterTransmit(0x02);
	SpiMasterTransmit(reg);	// sending register adress
	SpiMasterTransmit(value); // sending value to be written in the register
	PORTB |= (1<<2);  // deactivating slave select
}

void SpiRead(uint8_t reg, uint8_t *reg_data, uint8_t len)
{
	PORTB &= ~(1<<2); // activating slave select
	SpiMasterTransmit(0x03);
	SpiMasterTransmit(reg);
	while(len--)
	{
		*(uint8_t*)reg_data = SpiMasterTransmit(0x00); // Cast to uint8_t*
		reg_data = (uint8_t*)reg_data + 1; //moving to the next piece of data
		_delay_ms(10);
	}
	PORTB |= (1<<2); // deactivating slave select
}
void CanStandard(){
	for(int i = 0; i<=0x0b; i++){
		SpiWrite(i, 0);
	}
	for(int i = 0x10; i<=0x1b; i++){
		SpiWrite(i, 0);
	}
	for(int i = 0x20; i<=0x27; i++){
		SpiWrite(i, 0);
	}
	for(int i = 0x30; i<=0x3D; i++){
		SpiWrite(i, 0);
	}
	for(int i = 0x40; i<=0x4d; i++){
		SpiWrite(i, 0);
	}
	for(int i = 0x50; i<=0x5d; i++){
		SpiWrite(i, 0);
	}
}
void CanInit(){
	PORTB &= ~(1 << 2);
	SpiWrite(CANCTRL, 0x80);
	SpiWrite(CNF1, 0x41);
	SpiWrite(CNF2, 0xFB);
	SpiWrite(CNF3, 0x86);
	SpiWrite(RXBxSIDH, 0xEE); //ustaw id receive buffera
	CanStandard();
	SpiWrite(RXB0CNTRL, 0x64);
	SpiWrite(CANCTRL, 0x00);
	PORTB |=(1<< 2);
}
void CanSend(uint8_t id, uint8_t data, uint8_t len)
{
	SpiWrite(TXB0_SIDH, id);
	SpiWrite(TXB0_DATA, data);
	/*SpiWrite(TXB1_SIDH, id);
	SpiWrite(TXB1_DATA, data+1);
	SpiWrite(TXB2_SIDH, id);
	SpiWrite(TXB2_DATA, data+1);*/

	PORTB &=~(1<<2);
	SpiMasterTransmit(RTS_TXB0); //tutaj wysyla txb0
	PORTB |=(1<<2);
}
void CanReceive(uint8_t *data) {
	uint8_t len = 1; // na razie 1 zeby zobaczyc czy dziala
	SpiRead(RXB0xDn, data, len);
	SpiWrite(CANINTF, 0); //tutaj trzeba bedzie cos zmienic bo resetuje wszystkie flagi a chcemy tylko 1 i 2gi bit
}
int main(void)
{
	CLKinit();
	SpiMasterInit();
	CanInit();
	DiodesInit();
	uint8_t data;
	CanReceive(&data);
	while(1)
	{
		if (data == 31)
		{
			PORTC |= (1<<0);
			PORTC |= (1<<1);
			PORTC |= (1<<2);
			PORTC |= (1<<3);
			PORTC |= (1<<4);
			PORTC |= (1<<5);
		}
	}
	
}

