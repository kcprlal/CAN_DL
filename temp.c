#include<stdlib.h>
#include<avr/io.h>
#include<util/delay.h>

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
#define TXB0_SIDH 0x31 // Standard Identifier High Register
#define TXB0_SIDL 0x32
#define TXB0_DATA 0x36
#define TXB1_SIDH 0x41// Standard Identifier High Register
#define TXB1_SIDL 0x42
#define TXB1_DATA 0x46
#define TXB2_SIDH 0x51  // Standard Identifier High Register
#define TXB2_SIDL 0x52
#define TXB2_DATA 0x56
#define RXB0DLC 0x65
#define RXB1DLC 0x75
#define RXB0xSIDH 0x61
#define RXB0xDn 0x66 //receive buffer moze byc od 0x66 do 0x6D
#define RXB1xSIDH 0x71
#define RXB1xDn 0x76 // od 0x76 do 0x7d
#define TXB0DLC 0x35
#define TXB1DLC 0x45
#define TXB2DLC 0x55
#define RXM0xSIDH 0x20
#define RXM0xSIDL 0x21
#define RXM1xSIDH 0x24
#define RXM1xSIDL 0x25

void CLKinit()
{
	CLKPR = 0x80; // You must set this register to 0x80 to be able to change it's value
	CLKPR = 0x04; // value 0x04 means 1/16 oscilator's frequency which is 1Mhz
}

void spi_arduino_init()
{
  //SPI CHIP SELECT
  DDRB |=(1<<PB2);
  //tutaj zmiana z jakeigos powodu trzrba skontrolowac
  PORTB |=(1<<PB2);
//SPI MISO
  DDRB &=~(1<<PB4);
//SPI MOSI 
  DDRB|=(1<<PB3);
//SPI SCK
  DDRB|=(1<<PB5);


//wybor f zegara
  SPCR0 &=~(1<<SPR1);
  SPCR0 |=(1<<SPR0);
//polarity i faza zegara
  SPCR0 &=~(1<<CPOL);
  SPCR0 &=~(1<<CPHA);
//master
  SPCR0 |=(1<<MSTR);
//data order MSB
  SPCR0 &=~(1<<DORD);
//spi enable
  SPCR0 |=(1<<SPE);
//przerwania moze pozniej 
}

uint8_t SPI_transfer(uint8_t data) {
	SPDR0 = data;
	while (!(SPSR0 & (1 << SPIF)));
	return SPDR0;
}
//spraedzic czy slave wybierany jest za pomoca 0 czy 1?

void SPI_write(uint8_t reg, uint8_t value){
  PORTB &=~(1<<PB2);
  SPI_transfer(0x02);
  SPI_transfer(reg);
	SPI_transfer(value);
  PORTB |=(1<<PB2);
}

void SPI_read(uint8_t reg, uint8_t *reg_data, uint8_t len){
  PORTB &=~(1<<PB2);
  SPI_transfer(0x03);
  SPI_transfer(reg);
		while(len--)
		{
			 *reg_data = SPI_transfer(0x00); // wysylamy cokolwiek zeby rejestr sie przesunal
			//reg_data++;       // Increment as uint8_t* na razie 1 elementowe
		}
  PORTB |=(1<<PB2);
}

void can_standard(){
  for(int i = 0; i<=0x0b; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x10; i<=0x1b; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x20; i<=0x27; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x30; i<=0x3D; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x40; i<=0x4d; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x50; i<=0x5d; i++){
    SPI_write(i, 0);
  }
}

void can_init(uint8_t *data){
  PORTB &= ~(1 << PB2);
  
  SPI_write(CANCTRL, 0x80);
  //can_standard();
SPI_write(CNF1, 0x00);     // BRP = 0, SJW = 1 TQ
SPI_write(CNF2, 0xB3);     // PROPSEG = 7 TQ, PHSEG1 = 4 TQ, BTLMODE = 1
SPI_write(CNF3, 0x03); 
  SPI_write(RXB0xSIDH, 0x12); //ustaw id receive buffera
  SPI_write(RXB1xSIDH, 0x12);
  SPI_write(RXB0CNTRL, 0x00);
  SPI_write(RXB0CNTRL, 0x64);
  SPI_write(CANCTRL, 0x00);
  SPI_write(CANINTF, 0);
  PORTB |=(1<<PB2);
}

void can_send(uint8_t id, uint8_t data, uint8_t len){
  SPI_write(TXBxCTRL, 0);
  SPI_write(TXB0_SIDH, id);
  SPI_write(TXB0_SIDL, 0);
  SPI_write(TXB0_DATA, data);
  SPI_write(TXB0DLC, len & 0x0f);
  // SPI_write(TXB1_SIDH, id);
  // SPI_write(TXB1_DATA, data+1);
  // SPI_write(TXB2_SIDH, id);
  // SPI_write(TXB2_DATA, data+1);

  PORTB &=~(1<<PB2);
  SPI_transfer(RTS_TXB0); //tutaj wysyla txb0
  PORTB |=(1<<PB2);
}

void can_receive(uint8_t *data) {
    uint8_t len = 1; // na razie 1 zeby zobaczyc czy dziala
    SPI_read(RXB0xDn, data, len);
    SPI_write(CANINTF, 0); //tutaj trzeba bedzie cos zmienic bo resetuje wszystkie flagi a chcemy tylko 1 i 2gi bit
}

int main(){
CLKinit();
DDRD = 0xff;
//uint8_t arr=0x0;
uint8_t data = 0x1;
PORTD = data;
spi_arduino_init();
can_init(&data);
//can_send(0x12, 0xf0, 1);

can_receive(&data);
	PORTD = data;

  return 0;
}
