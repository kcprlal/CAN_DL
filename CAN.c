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
  SPCR &=~(1<<SPR1);
  SPCR |=(1<<SPR0);
//polarity i faza zegara
  SPCR &=~(1<<CPOL);
  SPCR &=~(1<<CPHA);
//master
  SPCR |=(1<<MSTR);
//data order MSB
  SPCR &=~(1<<DORD);
//spi enable
  SPCR |=(1<<SPE);
//przerwania moze pozniej 
}

uint8_t SPI_transfer(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
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
			 *(uint8_t*)reg_data = SPI_transfer(0x00); // Cast to uint8_t*
        reg_data = (uint8_t*)reg_data + 1;        // Increment as uint8_t*
        _delay_ms(10);
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

void can_init(){
  PORTB &= ~(1 << PB2);
  SPI_write(CANCTRL, 0x80);
  SPI_write(CNF1, 0x41); 
  SPI_write(CNF2, 0xFB);
  SPI_write(CNF3, 0x86);
  SPI_write(RXBxSIDH, 0xAA); //ustaw id receive buffera
  can_standard();
  SPI_write(RXB0CNTRL, 0x64);
  SPI_write(CANCTRL, 0x00);
  PORTB |=(1<<PB2);
}

void can_send(uint8_t id, uint8_t data, uint8_t len){
  SPI_write(TXB0_SIDH, id);
  SPI_write(TXB0_DATA, data);
  SPI_write(TXB1_SIDH, id);
  SPI_write(TXB1_DATA, data+1);
  SPI_write(TXB2_SIDH, id);
  SPI_write(TXB2_DATA, data+1);

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

uint8_t arr=4;
spi_arduino_init();
can_init();
while(1){
  can_send(0x12, arr, 1);
  _delay_ms(10000);
  
}


  return 0;
}
